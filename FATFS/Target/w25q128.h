#ifndef W25Q128_H
#define W25Q128_H

#include <stdint.h>

#define W25Q128_CAPACITY_BYTES       (16UL * 1024UL * 1024UL)
#define W25Q128_PAGE_SIZE             256U
#define W25Q128_SECTOR_SIZE           4096U

uint8_t W25Q128_Init(void);
uint8_t W25Q128_Read(uint32_t address, uint8_t *data, uint32_t size);
uint8_t W25Q128_Write(uint32_t address, const uint8_t *data, uint32_t size);
uint8_t W25Q128_EraseSector(uint32_t address);

#endif
