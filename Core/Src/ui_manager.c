#include "ui_manager.h"

#include "ads1256.h"
#include "cmsis_os2.h"
#include "dwin.h"
#include "system_runtime.h"
#include "tmc5130a.h"

#define UI_COMMAND_WRITE_VP         0x82U
#define UI_VP_PAGE_CONTROL          0x0084U
#define UI_PAGE_SWITCH_MAGIC       0x5A01U
#define UI_KEY_QUEUE_LENGTH         8U
#define UI_ACTION_QUEUE_LENGTH      2U
#define UI_REFRESH_INTERVAL_MS      250U
#define UI_DEFAULT_BEEP_TICKS       5U
#define UI_MIN_BEEP_TICKS           1U
#define UI_MAX_BEEP_TICKS           600U

#define UI_HOME_WORD_COUNT          7U
#define UI_SETTINGS_WORD_COUNT      2U
#define UI_CALIBRATION_WORD_COUNT   5U
#define UI_ALARM_WORD_COUNT         8U

typedef enum
{
  UI_ACTION_HOME = 1,
  UI_ACTION_CALIBRATE
} UI_Action;

typedef struct
{
  uint16_t address;
  uint8_t word_count;
  uint16_t value;
} UI_KeyEvent;

static osMessageQueueId_t UI_KeyQueue;
static osMessageQueueId_t UI_ActionQueue;
static osThreadId_t UI_TaskHandle;
static osThreadId_t UI_ActionTaskHandle;
static uint8_t UI_Initialized;
static uint8_t UI_PageReady;
static uint8_t UI_HomeInProgress;
static uint16_t UI_LastKeyCode;
static uint16_t UI_BeepDurationTicks = UI_DEFAULT_BEEP_TICKS;
static uint16_t UI_AcknowledgedAlarmCode;
static uint32_t UI_AcknowledgedDwinUartErrors;
static uint32_t UI_AcknowledgedDwinInvalidFrames;
static UI_CalibrationHandler UI_CalibrationCallback;

static const osThreadAttr_t UI_TaskAttributes = {
  .name = "UI",
  .stack_size = 1024U * 2U,
  .priority = (osPriority_t)osPriorityBelowNormal
};
static const osThreadAttr_t UI_ActionTaskAttributes = {
  .name = "UIAction",
  .stack_size = 1024U * 2U,
  .priority = (osPriority_t)osPriorityBelowNormal
};

volatile UI_Status UI_ManagerStartResult = UI_ERROR_DWIN;
volatile UI_Page UI_CurrentPage = UI_PAGE_HOME;
volatile UI_CalibrationState UI_CalibrationResult =
    UI_CALIBRATION_UNAVAILABLE;
volatile uint32_t UI_DroppedKeyEvents;
volatile uint32_t UI_UpdateErrors;

static uint16_t UI_VoltageToMillivolts(float voltage)
{
  float millivolts = voltage * 1000.0f;

  if (millivolts > 32767.0f)
  {
    return (uint16_t)32767;
  }
  if (millivolts < -32768.0f)
  {
    return (uint16_t)32768;
  }
  if (!(millivolts >= -32768.0f))
  {
    return 0U;
  }

  return (uint16_t)((millivolts >= 0.0f) ?
                    (millivolts + 0.5f) : (millivolts - 0.5f));
}

static UI_Status UI_WriteWords(uint16_t address, const uint16_t *words,
                               uint8_t word_count)
{
  uint8_t data[2U + (2U * UI_ALARM_WORD_COUNT)];
  uint16_t index;
  uint8_t data_length;

  if ((words == NULL) || (word_count == 0U) ||
      (word_count > UI_ALARM_WORD_COUNT))
  {
    return UI_ERROR_ARGUMENT;
  }

  data[0] = (uint8_t)(address >> 8);
  data[1] = (uint8_t)address;
  for (index = 0U; index < word_count; index++)
  {
    data[2U + (2U * index)] = (uint8_t)(words[index] >> 8);
    data[3U + (2U * index)] = (uint8_t)words[index];
  }
  data_length = (uint8_t)(2U + (2U * word_count));

  if (DWIN_SendCommand(UI_COMMAND_WRITE_VP, data, data_length) != DWIN_OK)
  {
    UI_UpdateErrors++;
    return UI_ERROR_DWIN;
  }

  return UI_OK;
}

static UI_Status UI_SendPageChange(UI_Page page)
{
  uint16_t words[2];
  uint16_t page_address;

  switch (page)
  {
    case UI_PAGE_HOME:
      page_address = UI_DWIN_PAGE_HOME;
      break;
    case UI_PAGE_SETTINGS:
      page_address = UI_DWIN_PAGE_SETTINGS;
      break;
    case UI_PAGE_CALIBRATION:
      page_address = UI_DWIN_PAGE_CALIBRATION;
      break;
    case UI_PAGE_ALARM:
      page_address = UI_DWIN_PAGE_ALARM;
      break;
    default:
      return UI_ERROR_ARGUMENT;
  }

  words[0] = UI_PAGE_SWITCH_MAGIC;
  words[1] = page_address;
  if (UI_WriteWords(UI_VP_PAGE_CONTROL, words, 2U) != UI_OK)
  {
    UI_PageReady = 0U;
    return UI_ERROR_DWIN;
  }

  UI_CurrentPage = page;
  UI_PageReady = 1U;
  return UI_OK;
}

static UI_Status UI_ChangePage(UI_Page page)
{
  if ((page == UI_CurrentPage) && (UI_PageReady != 0U))
  {
    return UI_OK;
  }

  if (UI_SendPageChange(page) != UI_OK)
  {
    return UI_ERROR_DWIN;
  }

  return UI_OK;
}

static uint16_t UI_GetActiveAlarmCode(void)
{
  if (ADS1256_LastStatus != ADS1256_OK)
  {
    return 1U;
  }
  if ((TMC5130A_HomingResult != TMC5130A_HOME_NOT_STARTED) &&
      (TMC5130A_HomingResult != TMC5130A_HOME_SUCCESS))
  {
    return 2U;
  }
  if ((DWIN_UartErrors != UI_AcknowledgedDwinUartErrors) ||
      (DWIN_InvalidFrames != UI_AcknowledgedDwinInvalidFrames))
  {
    return 3U;
  }

  return 0U;
}

static uint16_t UI_GetAlarmCode(void)
{
  uint16_t alarm_code = UI_GetActiveAlarmCode();

  if (alarm_code == 0U)
  {
    UI_AcknowledgedAlarmCode = 0U;
    return 0U;
  }
  return (alarm_code == UI_AcknowledgedAlarmCode) ? 0U : alarm_code;
}

static UI_Status UI_UpdateHomePage(void)
{
  uint16_t words[UI_HOME_WORD_COUNT];
  uint32_t raw_sample = (uint32_t)ADS1256_LastRawSample;

  words[0] = (uint16_t)(raw_sample >> 16);
  words[1] = (uint16_t)raw_sample;
  words[2] = UI_VoltageToMillivolts(ADS1256_LastVoltage);
  words[3] = (uint16_t)TMC5130A_HomingResult;
  words[4] = (uint16_t)ADS1256_LastStatus;
  words[5] = UI_LastKeyCode;
  words[6] = (UI_HomeInProgress != 0U) ? 1U : 0U;
  return UI_WriteWords(UI_VP_SAMPLE_RAW, words, UI_HOME_WORD_COUNT);
}

static UI_Status UI_UpdateSettingsPage(void)
{
  uint16_t words[UI_SETTINGS_WORD_COUNT];

  words[0] = UI_BeepDurationTicks;
  words[1] = (uint16_t)(UI_CalibrationCallback != NULL);
  return UI_WriteWords(UI_VP_SETTINGS_BEEP_TICKS, words,
                       UI_SETTINGS_WORD_COUNT);
}

static UI_Status UI_UpdateCalibrationPage(void)
{
  uint16_t words[UI_CALIBRATION_WORD_COUNT];
  uint32_t raw_sample = (uint32_t)ADS1256_LastRawSample;

  words[0] = (uint16_t)(raw_sample >> 16);
  words[1] = (uint16_t)raw_sample;
  words[2] = UI_VoltageToMillivolts(ADS1256_LastVoltage);
  words[3] = (uint16_t)ADS1256_LastStatus;
  words[4] = (uint16_t)UI_CalibrationResult;
  return UI_WriteWords(UI_VP_CALIBRATION_RAW, words,
                       UI_CALIBRATION_WORD_COUNT);
}

static UI_Status UI_UpdateAlarmPage(void)
{
  uint16_t words[UI_ALARM_WORD_COUNT];

  words[0] = UI_GetAlarmCode();
  words[1] = (uint16_t)ADS1256_LastStatus;
  words[2] = (uint16_t)TMC5130A_HomingResult;
  words[3] = (uint16_t)DWIN_DroppedRxBytes;
  words[4] = (uint16_t)DWIN_InvalidFrames;
  words[5] = (uint16_t)DWIN_UartErrors;
  words[6] = (uint16_t)UI_UpdateErrors;
  words[7] = (uint16_t)UI_DroppedKeyEvents;
  return UI_WriteWords(UI_VP_ALARM_CODE, words, UI_ALARM_WORD_COUNT);
}

static UI_Status UI_UpdateDisplay(void)
{
  if (UI_PageReady == 0U)
  {
    return UI_ERROR_DWIN;
  }

  switch (UI_CurrentPage)
  {
    case UI_PAGE_HOME:
      return UI_UpdateHomePage();
    case UI_PAGE_SETTINGS:
      return UI_UpdateSettingsPage();
    case UI_PAGE_CALIBRATION:
      return UI_UpdateCalibrationPage();
    case UI_PAGE_ALARM:
      return UI_UpdateAlarmPage();
    default:
      return UI_ERROR_ARGUMENT;
  }
}

static void UI_QueueAction(UI_Action action)
{
  if (osMessageQueuePut(UI_ActionQueue, &action, 0U, 0U) != osOK)
  {
    UI_DroppedKeyEvents++;
    if (action == UI_ACTION_HOME)
    {
      UI_HomeInProgress = 0U;
    }
    else if (action == UI_ACTION_CALIBRATE)
    {
      UI_CalibrationResult = UI_CALIBRATION_ERROR;
    }
  }
}

static void UI_HandleHomeKey(uint16_t key_code)
{
  if (key_code == UI_KEY_HOME)
  {
    if (UI_HomeInProgress == 0U)
    {
      UI_HomeInProgress = 1U;
      UI_QueueAction(UI_ACTION_HOME);
    }
  }
  else if (key_code == UI_KEY_BEEP)
  {
    SysRun_SetBeep((uint32_t)UI_BeepDurationTicks);
  }
  else if (key_code == UI_KEY_OPEN_SETTINGS)
  {
    (void)UI_ChangePage(UI_PAGE_SETTINGS);
  }
  else if (key_code == UI_KEY_OPEN_CALIBRATION)
  {
    (void)UI_ChangePage(UI_PAGE_CALIBRATION);
  }
  else if (key_code == UI_KEY_OPEN_ALARMS)
  {
    (void)UI_ChangePage(UI_PAGE_ALARM);
  }
}

static void UI_HandleSettingsKey(uint16_t key_code)
{
  if (key_code == UI_KEY_BACK_HOME)
  {
    (void)UI_ChangePage(UI_PAGE_HOME);
  }
  else if ((key_code == UI_KEY_BEEP_INCREASE) &&
           (UI_BeepDurationTicks < UI_MAX_BEEP_TICKS))
  {
    UI_BeepDurationTicks++;
  }
  else if ((key_code == UI_KEY_BEEP_DECREASE) &&
           (UI_BeepDurationTicks > UI_MIN_BEEP_TICKS))
  {
    UI_BeepDurationTicks--;
  }
  else if (key_code == UI_KEY_BEEP)
  {
    SysRun_SetBeep((uint32_t)UI_BeepDurationTicks);
  }
}

static void UI_HandleCalibrationKey(uint16_t key_code)
{
  if (key_code == UI_KEY_BACK_HOME)
  {
    (void)UI_ChangePage(UI_PAGE_HOME);
  }
  else if (key_code == UI_KEY_CALIBRATE)
  {
    if ((UI_CalibrationCallback != NULL) &&
        (UI_CalibrationResult != UI_CALIBRATION_RUNNING))
    {
      UI_CalibrationResult = UI_CALIBRATION_RUNNING;
      UI_QueueAction(UI_ACTION_CALIBRATE);
    }
    else if (UI_CalibrationCallback == NULL)
    {
      UI_CalibrationResult = UI_CALIBRATION_UNAVAILABLE;
    }
  }
}

static void UI_HandleAlarmKey(uint16_t key_code)
{
  if (key_code == UI_KEY_ALARM_ACKNOWLEDGE)
  {
    UI_AcknowledgedAlarmCode = UI_GetActiveAlarmCode();
    if (UI_AcknowledgedAlarmCode == 3U)
    {
      UI_AcknowledgedDwinUartErrors = DWIN_UartErrors;
      UI_AcknowledgedDwinInvalidFrames = DWIN_InvalidFrames;
    }
    (void)UI_ChangePage(UI_PAGE_HOME);
  }
  else if (key_code == UI_KEY_BACK_HOME)
  {
    (void)UI_ChangePage(UI_PAGE_HOME);
  }
}

static void UI_HandleKey(uint16_t key_code)
{
  UI_LastKeyCode = key_code;

  switch (UI_CurrentPage)
  {
    case UI_PAGE_HOME:
      UI_HandleHomeKey(key_code);
      break;
    case UI_PAGE_SETTINGS:
      UI_HandleSettingsKey(key_code);
      break;
    case UI_PAGE_CALIBRATION:
      UI_HandleCalibrationKey(key_code);
      break;
    case UI_PAGE_ALARM:
      UI_HandleAlarmKey(key_code);
      break;
    default:
      (void)UI_ChangePage(UI_PAGE_HOME);
      break;
  }
}

static void UI_OnDwinEvent(const DWIN_Event *event)
{
  UI_KeyEvent key_event;

  if ((event == NULL) || (event->has_vp_data == 0U) ||
      (event->address != UI_VP_KEY_CODE) || (event->word_count == 0U))
  {
    return;
  }

  key_event.address = event->address;
  key_event.word_count = event->word_count;
  key_event.value = event->words[0];
  if (osMessageQueuePut(UI_KeyQueue, &key_event, 0U, 0U) != osOK)
  {
    UI_DroppedKeyEvents++;
  }
}

static void UI_Task(void *argument)
{
  UI_KeyEvent key_event;

  (void)argument;
  (void)UI_SendPageChange(UI_PAGE_HOME);
  (void)UI_UpdateDisplay();
  for (;;)
  {
    if (osMessageQueueGet(UI_KeyQueue, &key_event, NULL,
                          UI_REFRESH_INTERVAL_MS) == osOK)
    {
      if ((key_event.address == UI_VP_KEY_CODE) &&
          (key_event.word_count > 0U))
      {
        UI_HandleKey(key_event.value);
      }
    }

    if (UI_PageReady == 0U)
    {
      (void)UI_SendPageChange(UI_CurrentPage);
    }
    (void)UI_UpdateDisplay();
  }
}

static void UI_ActionTask(void *argument)
{
  UI_Action action;

  (void)argument;
  for (;;)
  {
    if (osMessageQueueGet(UI_ActionQueue, &action, NULL,
                          osWaitForever) != osOK)
    {
      continue;
    }

    if (action == UI_ACTION_HOME)
    {
      if (TMC5130A_Init() == 0U)
      {
        TMC5130A_HomingResult = TMC5130A_HOME_INIT_ERROR;
      }
      else
      {
        TMC5130A_HomingResult = TMC5130A_FindHome();
      }
      UI_HomeInProgress = 0U;
    }
    else if ((action == UI_ACTION_CALIBRATE) &&
             (UI_CalibrationCallback != NULL))
    {
      UI_CalibrationResult =
          (UI_CalibrationCallback() != 0U) ?
          UI_CALIBRATION_SUCCESS : UI_CALIBRATION_ERROR;
    }
  }
}

UI_Status UI_SetCalibrationHandler(UI_CalibrationHandler handler)
{
  if (UI_Initialized != 0U)
  {
    return UI_ERROR_ARGUMENT;
  }

  UI_CalibrationCallback = handler;
  return UI_OK;
}

UI_Status UI_Manager_Init(void)
{
  if (UI_Initialized != 0U)
  {
    return UI_OK;
  }
  if (DWIN_StartResult != DWIN_OK)
  {
    UI_ManagerStartResult = UI_ERROR_DWIN;
    return UI_ManagerStartResult;
  }

  UI_CalibrationResult = (UI_CalibrationCallback != NULL) ?
                         UI_CALIBRATION_IDLE :
                         UI_CALIBRATION_UNAVAILABLE;
  UI_ManagerStartResult = UI_ERROR_RTOS;
  UI_KeyQueue = osMessageQueueNew(UI_KEY_QUEUE_LENGTH,
                                  sizeof(UI_KeyEvent), NULL);
  UI_ActionQueue = osMessageQueueNew(UI_ACTION_QUEUE_LENGTH,
                                     sizeof(UI_Action), NULL);
  if ((UI_KeyQueue == NULL) || (UI_ActionQueue == NULL))
  {
    if (UI_KeyQueue != NULL)
    {
      (void)osMessageQueueDelete(UI_KeyQueue);
      UI_KeyQueue = NULL;
    }
    if (UI_ActionQueue != NULL)
    {
      (void)osMessageQueueDelete(UI_ActionQueue);
      UI_ActionQueue = NULL;
    }
    return UI_ManagerStartResult;
  }

  UI_PageReady = 0U;
  DWIN_SetEventCallback(UI_OnDwinEvent);
  UI_ActionTaskHandle =
      osThreadNew(UI_ActionTask, NULL, &UI_ActionTaskAttributes);
  if (UI_ActionTaskHandle == NULL)
  {
    DWIN_SetEventCallback(NULL);
    (void)osMessageQueueDelete(UI_KeyQueue);
    (void)osMessageQueueDelete(UI_ActionQueue);
    UI_KeyQueue = NULL;
    UI_ActionQueue = NULL;
    return UI_ManagerStartResult;
  }

  UI_TaskHandle = osThreadNew(UI_Task, NULL, &UI_TaskAttributes);
  if (UI_TaskHandle == NULL)
  {
    DWIN_SetEventCallback(NULL);
    (void)osThreadTerminate(UI_ActionTaskHandle);
    (void)osMessageQueueDelete(UI_KeyQueue);
    (void)osMessageQueueDelete(UI_ActionQueue);
    UI_ActionTaskHandle = NULL;
    UI_KeyQueue = NULL;
    UI_ActionQueue = NULL;
    return UI_ManagerStartResult;
  }

  UI_Initialized = 1U;
  UI_ManagerStartResult = UI_OK;
  return UI_ManagerStartResult;
}
