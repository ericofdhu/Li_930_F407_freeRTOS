#include "tmc5130a.h"

#include "main.h"
#include "spi.h"
#include "cmsis_os.h"

#define TMC5130A_REG_GCONF             0x00U
#define TMC5130A_REG_GSTAT             0x01U
#define TMC5130A_REG_IOIN              0x04U
#define TMC5130A_REG_IHOLD_IRUN        0x10U
#define TMC5130A_REG_TPOWERDOWN        0x11U
#define TMC5130A_REG_RAMPMODE          0x20U
#define TMC5130A_REG_XACTUAL           0x21U
#define TMC5130A_REG_VACTUAL           0x22U
#define TMC5130A_REG_AMAX              0x26U
#define TMC5130A_REG_VMAX              0x27U
#define TMC5130A_REG_DMAX              0x28U
#define TMC5130A_REG_XTARGET           0x2DU
#define TMC5130A_REG_SW_MODE           0x34U
#define TMC5130A_REG_RAMP_STAT          0x35U
#define TMC5130A_REG_CHOPCONF          0x6CU

#define TMC5130A_IOIN_REFL             (1UL << 0)
#define TMC5130A_IOIN_VERSION_MASK     (0xFFUL << 24)
#define TMC5130A_IOIN_VERSION          (0x11UL << 24)
#define TMC5130A_SW_MODE_STOP_L_ENABLE (1UL << 0)
#define TMC5130A_SW_MODE_POL_STOP_L    (1UL << 2)
#define TMC5130A_RAMP_STAT_EVENT_STOP_L (1UL << 4)
#define TMC5130A_RAMP_STAT_VZERO       (1UL << 10)
#define TMC5130A_HOME_SWITCH_MASK      (TMC5130A_IOIN_REFL)
#define TMC5130A_HOME_DEBOUNCE_COUNT   3U

static uint8_t TMC5130A_Initialized;

static void TMC5130A_Select(void)
{
  HAL_GPIO_WritePin(TMC_SPI_CS_GPIO_Port, TMC_SPI_CS_Pin, GPIO_PIN_RESET);
}

static void TMC5130A_Deselect(void)
{
  HAL_GPIO_WritePin(TMC_SPI_CS_GPIO_Port, TMC_SPI_CS_Pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef TMC5130A_Transfer(uint8_t address,
                                           uint32_t writeData,
                                           uint32_t *readData)
{
  uint8_t tx[5];
  uint8_t rx[5];
  HAL_StatusTypeDef status;

  tx[0] = address;
  tx[1] = (uint8_t)(writeData >> 24);
  tx[2] = (uint8_t)(writeData >> 16);
  tx[3] = (uint8_t)(writeData >> 8);
  tx[4] = (uint8_t)writeData;

  TMC5130A_Select();
  status = HAL_SPI_TransmitReceive(&hspi3, tx, rx, sizeof(tx), 100U);
  TMC5130A_Deselect();
  if ((status != HAL_OK) || (readData == NULL))
  {
    return status;
  }

  osDelay(1U);
  TMC5130A_Select();
  status = HAL_SPI_TransmitReceive(&hspi3, tx, rx, sizeof(tx), 100U);
  TMC5130A_Deselect();
  if (status == HAL_OK)
  {
    *readData = ((uint32_t)rx[1] << 24) |
                ((uint32_t)rx[2] << 16) |
                ((uint32_t)rx[3] << 8) |
                (uint32_t)rx[4];
  }
  return status;
}

static HAL_StatusTypeDef TMC5130A_WriteRegister(uint8_t address,
                                                uint32_t value)
{
  return TMC5130A_Transfer((uint8_t)(address | 0x80U), value, NULL);
}

static HAL_StatusTypeDef TMC5130A_ReadRegister(uint8_t address,
                                               uint32_t *value)
{
  if (value == NULL)
  {
    return HAL_ERROR;
  }
  return TMC5130A_Transfer((uint8_t)(address & 0x7FU), 0U, value);
}

uint8_t TMC5130A_Init(void)
{
  uint32_t ioInput;
  uint32_t holdRun;

  TMC5130A_Initialized = 0U;
  HAL_GPIO_WritePin(TMC_SPI_CS_GPIO_Port, TMC_SPI_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(TMC_EN_GPIO_Port, TMC_EN_Pin, GPIO_PIN_SET);
  osDelay(10U);

  if ((TMC5130A_ReadRegister(TMC5130A_REG_IOIN, &ioInput) != HAL_OK) ||
      ((ioInput & TMC5130A_IOIN_VERSION_MASK) != TMC5130A_IOIN_VERSION))
  {
    return 0U;
  }

  if ((TMC5130A_WriteRegister(TMC5130A_REG_GSTAT, 0x00000007UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_GCONF, 0x00000000UL) != HAL_OK))
  {
    return 0U;
  }

  holdRun = ((uint32_t)TMC5130A_IHOLD & 0x1FUL) |
            (((uint32_t)TMC5130A_IRUN & 0x1FUL) << 8) |
            (6UL << 16);
  if ((TMC5130A_WriteRegister(TMC5130A_REG_IHOLD_IRUN, holdRun) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_TPOWERDOWN, 10UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_CHOPCONF, 0x040100C3UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_XACTUAL, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_XTARGET, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_SW_MODE,
                              TMC5130A_SW_MODE_POL_STOP_L) != HAL_OK))
  {
    return 0U;
  }

  HAL_GPIO_WritePin(TMC_EN_GPIO_Port, TMC_EN_Pin, GPIO_PIN_RESET);
  osDelay(10U);
  TMC5130A_Initialized = 1U;
  return 1U;
}

TMC5130A_HomeResult TMC5130A_FindHome(void)
{
  uint32_t ioInput;
  uint32_t rampStatus;
  uint32_t actualPosition;
  int32_t signedPosition;
  uint32_t startTick;
  uint8_t debounceCount;
  HAL_StatusTypeDef status;

  if (TMC5130A_Initialized == 0U)
  {
    return TMC5130A_HOME_INIT_ERROR;
  }

  if (TMC5130A_ReadRegister(TMC5130A_REG_IOIN, &ioInput) != HAL_OK)
  {
    return TMC5130A_HOME_SPI_ERROR;
  }

  if ((ioInput & TMC5130A_HOME_SWITCH_MASK) != 0U)
  {
    if ((TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL) != HAL_OK) ||
        (TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL) != HAL_OK) ||
        (TMC5130A_WriteRegister(TMC5130A_REG_XACTUAL, 0UL) != HAL_OK) ||
        (TMC5130A_WriteRegister(TMC5130A_REG_XTARGET, 0UL) != HAL_OK))
    {
      return TMC5130A_HOME_SPI_ERROR;
    }
    return TMC5130A_HOME_SUCCESS;
  }

  if ((TMC5130A_WriteRegister(TMC5130A_REG_SW_MODE,
                              TMC5130A_SW_MODE_STOP_L_ENABLE |
                              TMC5130A_SW_MODE_POL_STOP_L) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_XACTUAL, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_AMAX,
                              TMC5130A_HOME_AMAX) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_DMAX,
                              TMC5130A_HOME_AMAX) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_VMAX,
                              TMC5130A_HOME_VMAX) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 2UL) != HAL_OK))
  {
    return TMC5130A_HOME_SPI_ERROR;
  }

  startTick = HAL_GetTick();
  debounceCount = 0U;
  for (;;)
  {
    status = TMC5130A_ReadRegister(TMC5130A_REG_IOIN, &ioInput);
    if (status != HAL_OK)
    {
      break;
    }
    status = TMC5130A_ReadRegister(TMC5130A_REG_RAMP_STAT, &rampStatus);
    if (status != HAL_OK)
    {
      break;
    }

    if ((ioInput & TMC5130A_HOME_SWITCH_MASK) != 0U)
    {
      debounceCount++;
      if ((debounceCount >= TMC5130A_HOME_DEBOUNCE_COUNT) ||
          ((rampStatus & TMC5130A_RAMP_STAT_EVENT_STOP_L) != 0U))
      {
        break;
      }
    }
    else
    {
      debounceCount = 0U;
    }

    status = TMC5130A_ReadRegister(TMC5130A_REG_XACTUAL, &actualPosition);
    if (status != HAL_OK)
    {
      break;
    }
    signedPosition = (int32_t)actualPosition;
    if ((signedPosition <= -TMC5130A_HOME_MAX_STEPS) ||
        (signedPosition >= TMC5130A_HOME_MAX_STEPS))
    {
      TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
      TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
      return TMC5130A_HOME_TRAVEL_LIMIT;
    }
    if ((HAL_GetTick() - startTick) >= TMC5130A_HOME_TIMEOUT_MS)
    {
      TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
      TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
      return TMC5130A_HOME_TIMEOUT;
    }

    osDelay(10U);
  }

  if (status != HAL_OK)
  {
    TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
    TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
    return TMC5130A_HOME_SPI_ERROR;
  }

  status = TMC5130A_ReadRegister(TMC5130A_REG_VACTUAL, &actualPosition);
  if (status != HAL_OK)
  {
    TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
    TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
    return TMC5130A_HOME_SPI_ERROR;
  }

  startTick = HAL_GetTick();
  while ((actualPosition != 0U) &&
         ((HAL_GetTick() - startTick) < 1000U))
  {
    osDelay(10U);
    status = TMC5130A_ReadRegister(TMC5130A_REG_VACTUAL, &actualPosition);
    if (status != HAL_OK)
    {
      TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
      TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
      return TMC5130A_HOME_SPI_ERROR;
    }
  }

  if (actualPosition != 0U)
  {
    TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL);
    TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL);
    return TMC5130A_HOME_TIMEOUT;
  }

  if ((TMC5130A_WriteRegister(TMC5130A_REG_RAMPMODE, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_VMAX, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_XACTUAL, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_XTARGET, 0UL) != HAL_OK) ||
      (TMC5130A_WriteRegister(TMC5130A_REG_SW_MODE,
                              TMC5130A_SW_MODE_POL_STOP_L) != HAL_OK))
  {
    return TMC5130A_HOME_SPI_ERROR;
  }

  return TMC5130A_HOME_SUCCESS;
}
