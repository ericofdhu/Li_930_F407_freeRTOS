#include "w25q128.h"

#include <string.h>
#include "main.h"
#include "spi.h"

#define W25Q128_CMD_READ_DATA         0x03U
#define W25Q128_CMD_PAGE_PROGRAM      0x02U
#define W25Q128_CMD_WRITE_ENABLE      0x06U
#define W25Q128_CMD_READ_STATUS       0x05U
#define W25Q128_CMD_SECTOR_ERASE      0x20U
#define W25Q128_CMD_READ_JEDEC_ID     0x9FU

#define W25Q128_SPI_TIMEOUT_MS        100U
#define W25Q128_WRITE_TIMEOUT_MS      500U

static void W25Q128_Select(void)
{
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
}

static void W25Q128_Deselect(void)
{
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

static uint8_t W25Q128_TransferByte(uint8_t txData, uint8_t *rxData)
{
  uint8_t received;

  if (HAL_SPI_TransmitReceive(&hspi1, &txData, &received, 1U,
                              W25Q128_SPI_TIMEOUT_MS) != HAL_OK)
  {
    return 0U;
  }

  if (rxData != NULL)
  {
    *rxData = received;
  }
  return 1U;
}

static uint8_t W25Q128_ReadStatus(uint8_t *status)
{
  uint8_t result;

  W25Q128_Select();
  result = W25Q128_TransferByte(W25Q128_CMD_READ_STATUS, NULL);
  if ((result != 0U) && (W25Q128_TransferByte(0xFFU, status) == 0U))
  {
    result = 0U;
  }
  W25Q128_Deselect();
  return result;
}

static uint8_t W25Q128_WaitReady(uint32_t timeoutMs)
{
  uint32_t startTick;
  uint8_t status;

  startTick = HAL_GetTick();
  do
  {
    if (W25Q128_ReadStatus(&status) == 0U)
    {
      return 0U;
    }
    if ((status & 0x01U) == 0U)
    {
      return 1U;
    }
    HAL_Delay(1U);
  } while ((HAL_GetTick() - startTick) < timeoutMs);

  return 0U;
}

static uint8_t W25Q128_WriteEnable(void)
{
  uint8_t status;
  uint8_t result;

  W25Q128_Select();
  result = W25Q128_TransferByte(W25Q128_CMD_WRITE_ENABLE, NULL);
  W25Q128_Deselect();

  if ((result == 0U) || (W25Q128_ReadStatus(&status) == 0U))
  {
    return 0U;
  }
  return ((status & 0x02U) != 0U) ? 1U : 0U;
}

static uint8_t W25Q128_SendAddress(uint8_t command, uint32_t address)
{
  uint8_t header[4];

  header[0] = command;
  header[1] = (uint8_t)(address >> 16);
  header[2] = (uint8_t)(address >> 8);
  header[3] = (uint8_t)address;
  return (HAL_SPI_Transmit(&hspi1, header, sizeof(header),
                           W25Q128_SPI_TIMEOUT_MS) == HAL_OK) ? 1U : 0U;
}

uint8_t W25Q128_Init(void)
{
  uint8_t command;
  uint8_t dummy[3] = {0xFFU, 0xFFU, 0xFFU};
  uint8_t jedecId[3];
  uint8_t result;

  W25Q128_Deselect();
  HAL_Delay(1U);

  command = W25Q128_CMD_READ_JEDEC_ID;
  W25Q128_Select();
  result = (HAL_SPI_Transmit(&hspi1, &command, 1U,
                             W25Q128_SPI_TIMEOUT_MS) == HAL_OK) ? 1U : 0U;
  if (result != 0U)
  {
    result = (HAL_SPI_TransmitReceive(&hspi1, dummy, jedecId, sizeof(jedecId),
                                      W25Q128_SPI_TIMEOUT_MS) == HAL_OK) ?
             1U : 0U;
  }
  W25Q128_Deselect();

  if ((result == 0U) || (jedecId[0] != 0xEFU) ||
      (jedecId[1] != 0x40U) || (jedecId[2] != 0x18U))
  {
    return 0U;
  }

  return W25Q128_WaitReady(W25Q128_WRITE_TIMEOUT_MS);
}

uint8_t W25Q128_Read(uint32_t address, uint8_t *data, uint32_t size)
{
  uint8_t dummy[W25Q128_PAGE_SIZE];
  uint32_t chunkSize;

  if ((data == NULL) || (size == 0U) ||
      (address >= W25Q128_CAPACITY_BYTES) ||
      (size > (W25Q128_CAPACITY_BYTES - address)))
  {
    return 0U;
  }

  memset(dummy, 0xFF, sizeof(dummy));
  W25Q128_Select();
  if (W25Q128_SendAddress(W25Q128_CMD_READ_DATA, address) == 0U)
  {
    W25Q128_Deselect();
    return 0U;
  }

  while (size > 0U)
  {
    chunkSize = (size > sizeof(dummy)) ? sizeof(dummy) : size;
    if (HAL_SPI_TransmitReceive(&hspi1, dummy, data, (uint16_t)chunkSize,
                                W25Q128_SPI_TIMEOUT_MS) != HAL_OK)
    {
      W25Q128_Deselect();
      return 0U;
    }
    data += chunkSize;
    size -= chunkSize;
  }

  W25Q128_Deselect();
  return 1U;
}

uint8_t W25Q128_Write(uint32_t address, const uint8_t *data, uint32_t size)
{
  uint32_t pageOffset;
  uint32_t chunkSize;
  uint8_t header[4];

  if ((data == NULL) || (size == 0U) ||
      (address >= W25Q128_CAPACITY_BYTES) ||
      (size > (W25Q128_CAPACITY_BYTES - address)))
  {
    return 0U;
  }

  while (size > 0U)
  {
    pageOffset = address % W25Q128_PAGE_SIZE;
    chunkSize = W25Q128_PAGE_SIZE - pageOffset;
    if (chunkSize > size)
    {
      chunkSize = size;
    }

    if ((W25Q128_WaitReady(W25Q128_WRITE_TIMEOUT_MS) == 0U) ||
        (W25Q128_WriteEnable() == 0U))
    {
      return 0U;
    }

    header[0] = W25Q128_CMD_PAGE_PROGRAM;
    header[1] = (uint8_t)(address >> 16);
    header[2] = (uint8_t)(address >> 8);
    header[3] = (uint8_t)address;

    W25Q128_Select();
    if ((HAL_SPI_Transmit(&hspi1, header, sizeof(header),
                          W25Q128_SPI_TIMEOUT_MS) != HAL_OK) ||
        (HAL_SPI_Transmit(&hspi1, (uint8_t *)data, (uint16_t)chunkSize,
                          W25Q128_SPI_TIMEOUT_MS) != HAL_OK))
    {
      W25Q128_Deselect();
      return 0U;
    }
    W25Q128_Deselect();

    if (W25Q128_WaitReady(W25Q128_WRITE_TIMEOUT_MS) == 0U)
    {
      return 0U;
    }

    address += chunkSize;
    data += chunkSize;
    size -= chunkSize;
  }

  return 1U;
}

uint8_t W25Q128_EraseSector(uint32_t address)
{
  uint8_t header[4];

  if ((address >= W25Q128_CAPACITY_BYTES) ||
      ((address % W25Q128_SECTOR_SIZE) != 0U))
  {
    return 0U;
  }

  if ((W25Q128_WaitReady(W25Q128_WRITE_TIMEOUT_MS) == 0U) ||
      (W25Q128_WriteEnable() == 0U))
  {
    return 0U;
  }

  header[0] = W25Q128_CMD_SECTOR_ERASE;
  header[1] = (uint8_t)(address >> 16);
  header[2] = (uint8_t)(address >> 8);
  header[3] = (uint8_t)address;

  W25Q128_Select();
  if (HAL_SPI_Transmit(&hspi1, header, sizeof(header),
                       W25Q128_SPI_TIMEOUT_MS) != HAL_OK)
  {
    W25Q128_Deselect();
    return 0U;
  }
  W25Q128_Deselect();

  return W25Q128_WaitReady(W25Q128_WRITE_TIMEOUT_MS);
}
