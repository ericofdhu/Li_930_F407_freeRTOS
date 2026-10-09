#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <stdint.h>

/*
 * The page numbers must match the page IDs in the DWIN HMI project.
 * VP ranges below are a sample project map; keep screen controls aligned.
 */
#define UI_DWIN_PAGE_HOME          0U
#define UI_DWIN_PAGE_SETTINGS      1U
#define UI_DWIN_PAGE_CALIBRATION   2U
#define UI_DWIN_PAGE_ALARM         3U

/* Home page: raw sample (2 words), mV, homing, ADC, key, home-busy. */
#define UI_VP_SAMPLE_RAW           0x1000U

/* Settings page: beep duration in 100 ms ticks, calibration-hook present. */
#define UI_VP_SETTINGS_BEEP_TICKS  0x1100U

/* Calibration page: raw sample (2 words), mV, ADC, calibration state. */
#define UI_VP_CALIBRATION_RAW      0x1200U

/* Alarm page: summary, ADC, homing, DWIN and UI diagnostic counters. */
#define UI_VP_ALARM_CODE           0x1300U

/* Configure DWIN touch controls to write a key code to this VP. */
#define UI_VP_KEY_CODE             0x2000U

/* Home-page key codes. */
#define UI_KEY_HOME                0x0001U
#define UI_KEY_BEEP                0x0002U
#define UI_KEY_OPEN_SETTINGS       0x0010U
#define UI_KEY_OPEN_CALIBRATION    0x0011U
#define UI_KEY_OPEN_ALARMS         0x0012U

/* Settings/calibration/alarm page keys. */
#define UI_KEY_BACK_HOME           0x0020U
#define UI_KEY_BEEP_INCREASE       0x0021U
#define UI_KEY_BEEP_DECREASE       0x0022U
#define UI_KEY_CALIBRATE           0x0031U
#define UI_KEY_ALARM_ACKNOWLEDGE   0x0041U

typedef enum
{
  UI_PAGE_HOME = UI_DWIN_PAGE_HOME,
  UI_PAGE_SETTINGS = UI_DWIN_PAGE_SETTINGS,
  UI_PAGE_CALIBRATION = UI_DWIN_PAGE_CALIBRATION,
  UI_PAGE_ALARM = UI_DWIN_PAGE_ALARM
} UI_Page;

typedef enum
{
  UI_OK = 0,
  UI_ERROR_ARGUMENT,
  UI_ERROR_RTOS,
  UI_ERROR_DWIN
} UI_Status;

typedef enum
{
  UI_CALIBRATION_UNAVAILABLE = 0,
  UI_CALIBRATION_IDLE,
  UI_CALIBRATION_RUNNING,
  UI_CALIBRATION_SUCCESS,
  UI_CALIBRATION_ERROR
} UI_CalibrationState;

/* Runs in the UI action task; return nonzero on successful calibration. */
typedef uint8_t (*UI_CalibrationHandler)(void);

extern volatile UI_Status UI_ManagerStartResult;
extern volatile UI_Page UI_CurrentPage;
extern volatile UI_CalibrationState UI_CalibrationResult;
extern volatile uint32_t UI_DroppedKeyEvents;
extern volatile uint32_t UI_UpdateErrors;

/* Register before UI_Manager_Init; NULL leaves calibration unavailable. */
UI_Status UI_SetCalibrationHandler(UI_CalibrationHandler handler);
UI_Status UI_Manager_Init(void);

#endif
