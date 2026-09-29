#include "ads1256.h"

#include "main.h"
#include "spi.h"

#define ADS1256_CMD_RDATA               0x01U
#define ADS1256_CMD_SDATAC              0x0FU
#define ADS1256_CMD_WREG                0x50U
#define ADS1256_CMD_SELFCAL             0xF0U
#define ADS1256_CMD_RESET               0xFEU

#define ADS1256_REG_STATUS              0x00U
#define ADS1256_STATUS_ACAL             0x04U
#define ADS1256_MUX_AIN0_AIN1           0x01U
#define ADS1256_ADCON_GAIN_1            0x00U
#define ADS1256_DRATE_100_SPS           0x82U
#define ADS1256_CONFIG_TIMEOUT_MS       1000U

volatile int32_t ADS1256_LastRawSample;
volatile float ADS1256_LastVoltage;
volatile ADS1256_Status ADS1256_LastStatus = ADS1256_ERROR_CONFIG;

static osSemaphoreId_t ADS1256_DataReadySemaphore;
static uint8_t ADS1256_Initialized;

static void ADS1256_Select(void)
{
  HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_RESET);
}

static void ADS1256_Deselect(void)
{
  HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef ADS1256_SendCommand(uint8_t command)
{
  return HAL_SPI_Transmit(&hspi2, &command, 1U, 100U);
}

static ADS1256_Status ADS1256_WaitForReadyPin(uint32_t timeout)
{
  uint32_t startTick = HAL_GetTick();

  while (HAL_GPIO_ReadPin(ADS_DRDY_GPIO_Port, ADS_DRDY_Pin) != GPIO_PIN_RESET)
  {
    if ((HAL_GetTick() - startTick) >= timeout)
    {
      return ADS1256_ERROR_TIMEOUT;
    }
    osDelay(1U);
  }
  return ADS1256_OK;
}

ADS1256_Status ADS1256_Init(void)
{
  uint8_t config[4];
  ADS1256_Status waitStatus;

  ADS1256_Initialized = 0U;
  if (ADS1256_DataReadySemaphore == NULL)
  {
    ADS1256_DataReadySemaphore = osSemaphoreNew(1U, 0U, NULL);
    if (ADS1256_DataReadySemaphore == NULL)
    {
      ADS1256_LastStatus = ADS1256_ERROR_RTOS;
      return ADS1256_LastStatus;
    }
  }

  ADS1256_Deselect();
  HAL_GPIO_WritePin(ADS_RSR_GPIO_Port, ADS_RSR_Pin, GPIO_PIN_RESET);
  osDelay(2U);
  HAL_GPIO_WritePin(ADS_RSR_GPIO_Port, ADS_RSR_Pin, GPIO_PIN_SET);

  waitStatus = ADS1256_WaitForReadyPin(ADS1256_CONFIG_TIMEOUT_MS);
  if (waitStatus != ADS1256_OK)
  {
    ADS1256_LastStatus = waitStatus;
    return waitStatus;
  }

  ADS1256_Select();
  if (ADS1256_SendCommand(ADS1256_CMD_RESET) != HAL_OK)
  {
    ADS1256_Deselect();
    ADS1256_LastStatus = ADS1256_ERROR_SPI;
    return ADS1256_LastStatus;
  }
  ADS1256_Deselect();
  osDelay(5U);

  waitStatus = ADS1256_WaitForReadyPin(ADS1256_CONFIG_TIMEOUT_MS);
  if (waitStatus != ADS1256_OK)
  {
    ADS1256_LastStatus = waitStatus;
    return waitStatus;
  }

  ADS1256_Select();
  if (ADS1256_SendCommand(ADS1256_CMD_SDATAC) != HAL_OK)
  {
    ADS1256_Deselect();
    ADS1256_LastStatus = ADS1256_ERROR_SPI;
    return ADS1256_LastStatus;
  }
  ADS1256_Deselect();

  config[0] = ADS1256_STATUS_ACAL;
  config[1] = ADS1256_MUX_AIN0_AIN1;
  config[2] = ADS1256_ADCON_GAIN_1;
  config[3] = ADS1256_DRATE_100_SPS;
  ADS1256_Select();
  {
    uint8_t writeCommand[2] = {
      ADS1256_CMD_WREG | ADS1256_REG_STATUS,
      (uint8_t)(sizeof(config) - 1U)
    };
    if ((HAL_SPI_Transmit(&hspi2, writeCommand, sizeof(writeCommand),
                          100U) != HAL_OK) ||
        (HAL_SPI_Transmit(&hspi2, config, sizeof(config), 100U) != HAL_OK))
    {
      ADS1256_Deselect();
      ADS1256_LastStatus = ADS1256_ERROR_SPI;
      return ADS1256_LastStatus;
    }
  }
  ADS1256_Deselect();

  (void)osSemaphoreAcquire(ADS1256_DataReadySemaphore, 0U);
  ADS1256_Select();
  if (ADS1256_SendCommand(ADS1256_CMD_SELFCAL) != HAL_OK)
  {
    ADS1256_Deselect();
    ADS1256_LastStatus = ADS1256_ERROR_SPI;
    return ADS1256_LastStatus;
  }
  ADS1256_Deselect();

  if (osSemaphoreAcquire(ADS1256_DataReadySemaphore,
                         ADS1256_CONFIG_TIMEOUT_MS) != osOK)
  {
    ADS1256_LastStatus = ADS1256_ERROR_TIMEOUT;
    return ADS1256_LastStatus;
  }

  ADS1256_Initialized = 1U;
  ADS1256_LastStatus = ADS1256_OK;
  return ADS1256_OK;
}

osStatus_t ADS1256_WaitDataReady(uint32_t timeout)
{
  if ((ADS1256_Initialized == 0U) ||
      (ADS1256_DataReadySemaphore == NULL))
  {
    return osErrorResource;
  }
  return osSemaphoreAcquire(ADS1256_DataReadySemaphore, timeout);
}

ADS1256_Status ADS1256_ReadData(int32_t *sample)
{
  uint8_t data[3];
  uint32_t rawData;

  if (sample == NULL)
  {
    ADS1256_LastStatus = ADS1256_ERROR_ARGUMENT;
    return ADS1256_LastStatus;
  }
  if (ADS1256_Initialized == 0U)
  {
    ADS1256_LastStatus = ADS1256_ERROR_CONFIG;
    return ADS1256_LastStatus;
  }

  ADS1256_Select();
  if (ADS1256_SendCommand(ADS1256_CMD_RDATA) != HAL_OK)
  {
    ADS1256_Deselect();
    ADS1256_LastStatus = ADS1256_ERROR_SPI;
    return ADS1256_LastStatus;
  }
  osDelay(1U);
  if (HAL_SPI_Receive(&hspi2, data, sizeof(data), 100U) != HAL_OK)
  {
    ADS1256_Deselect();
    ADS1256_LastStatus = ADS1256_ERROR_SPI;
    return ADS1256_LastStatus;
  }
  ADS1256_Deselect();

  rawData = ((uint32_t)data[0] << 16) |
            ((uint32_t)data[1] << 8) |
            (uint32_t)data[2];
  if ((rawData & 0x00800000UL) != 0U)
  {
    rawData |= 0xFF000000UL;
  }

  *sample = (int32_t)rawData;
  ADS1256_LastRawSample = *sample;
  ADS1256_LastVoltage = ((float)(*sample) * ADS1256_REFERENCE_VOLTAGE) /
                        8388608.0f;
  ADS1256_LastStatus = ADS1256_OK;
  return ADS1256_OK;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == ADS_DRDY_Pin) &&
      (ADS1256_DataReadySemaphore != NULL))
  {
    (void)osSemaphoreRelease(ADS1256_DataReadySemaphore);
  }
}
