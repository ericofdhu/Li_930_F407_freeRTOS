/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */
#include "w25q128.h"

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static uint8_t SectorBuffer[W25Q128_SECTOR_SIZE];

#define USER_SECTOR_SIZE       512U
#define USER_SECTORS_PER_BLOCK (W25Q128_SECTOR_SIZE / USER_SECTOR_SIZE)
#define USER_SECTOR_COUNT      (W25Q128_CAPACITY_BYTES / USER_SECTOR_SIZE)

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
    if (pdrv != 0U)
    {
      return STA_NOINIT;
    }

    Stat = (W25Q128_Init() != 0U) ? 0U : STA_NOINIT;
    return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    if (pdrv != 0U)
    {
      return STA_NOINIT;
    }
    return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
    uint32_t byteAddress;
    uint32_t byteCount;

    if ((pdrv != 0U) || (buff == NULL) || (count == 0U))
    {
      return RES_PARERR;
    }
    if (Stat & STA_NOINIT)
    {
      return RES_NOTRDY;
    }
    if ((sector >= USER_SECTOR_COUNT) ||
        (count > (USER_SECTOR_COUNT - sector)))
    {
      return RES_PARERR;
    }

    byteAddress = sector * USER_SECTOR_SIZE;
    byteCount = (uint32_t)count * USER_SECTOR_SIZE;
    return (W25Q128_Read(byteAddress, buff, byteCount) != 0U) ?
           RES_OK : RES_ERROR;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
    uint32_t blockAddress;
    uint32_t blockOffset;
    uint32_t sectorsInBlock;
    uint32_t bytesInBlock;
    uint32_t remaining;

    if ((pdrv != 0U) || (buff == NULL) || (count == 0U))
    {
      return RES_PARERR;
    }
    if (Stat & STA_NOINIT)
    {
      return RES_NOTRDY;
    }
    if ((sector >= USER_SECTOR_COUNT) ||
        (count > (USER_SECTOR_COUNT - sector)))
    {
      return RES_PARERR;
    }

    remaining = count;
    while (remaining > 0U)
    {
      blockAddress = (sector / USER_SECTORS_PER_BLOCK) *
                     W25Q128_SECTOR_SIZE;
      blockOffset = (sector % USER_SECTORS_PER_BLOCK) * USER_SECTOR_SIZE;
      sectorsInBlock = USER_SECTORS_PER_BLOCK -
                       (blockOffset / USER_SECTOR_SIZE);
      if (sectorsInBlock > remaining)
      {
        sectorsInBlock = remaining;
      }
      bytesInBlock = sectorsInBlock * USER_SECTOR_SIZE;

      if (W25Q128_Read(blockAddress, SectorBuffer,
                       W25Q128_SECTOR_SIZE) == 0U)
      {
        return RES_ERROR;
      }
      memcpy(&SectorBuffer[blockOffset], buff, bytesInBlock);

      if (W25Q128_EraseSector(blockAddress) == 0U)
      {
        return RES_ERROR;
      }

      {
        uint32_t pageOffset;
        uint8_t pageIsErased;
        uint32_t index;

        for (pageOffset = 0U; pageOffset < W25Q128_SECTOR_SIZE;
             pageOffset += W25Q128_PAGE_SIZE)
        {
          pageIsErased = 1U;
          for (index = 0U; index < W25Q128_PAGE_SIZE; index++)
          {
            if (SectorBuffer[pageOffset + index] != 0xFFU)
            {
              pageIsErased = 0U;
              break;
            }
          }
          if ((pageIsErased == 0U) &&
              (W25Q128_Write(blockAddress + pageOffset,
                             &SectorBuffer[pageOffset],
                             W25Q128_PAGE_SIZE) == 0U))
          {
            return RES_ERROR;
          }
        }
      }

      buff += bytesInBlock;
      sector += sectorsInBlock;
      remaining -= sectorsInBlock;
    }
    return RES_OK;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
    if (pdrv != 0U)
    {
      return RES_PARERR;
    }
    if (Stat & STA_NOINIT)
    {
      return RES_NOTRDY;
    }

    switch (cmd)
    {
      case CTRL_SYNC:
        return RES_OK;

      case GET_SECTOR_COUNT:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(DWORD *)buff = USER_SECTOR_COUNT;
        return RES_OK;

      case GET_SECTOR_SIZE:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(WORD *)buff = USER_SECTOR_SIZE;
        return RES_OK;

      case GET_BLOCK_SIZE:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(DWORD *)buff = USER_SECTORS_PER_BLOCK;
        return RES_OK;

      default:
        return RES_PARERR;
    }
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */
