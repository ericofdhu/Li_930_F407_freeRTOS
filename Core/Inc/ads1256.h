#ifndef __ADS1256_H
#define __ADS1256_H

#include <stdint.h>
#include "cmsis_os2.h"

/* ADS1256 寄存器地址 */
#define REG_STATUS 0x00U
#define REG_MUX    0x01U
#define REG_ADCON  0x02U
#define REG_DRATE  0x03U
#define REG_IO     0x04U
#define REG_OFC0   0x05U
#define REG_OFC1   0x06U
#define REG_OFC2   0x07U
#define REG_FSC0   0x08U
#define REG_FSC1   0x09U
#define REG_FSC2   0x0AU

/* ADS1256 指令码 */
#define CMD_WAKEUP    0x00U
#define CMD_RDATA     0x01U
#define CMD_RDATAC    0x03U
#define CMD_SDATAC    0x0FU
#define CMD_RREG      0x10U
#define CMD_WREG      0x50U
#define CMD_SELFCAL   0xF0U
#define CMD_SELFOCAL  0xF1U
#define CMD_SELFGCAL  0xF2U
#define CMD_SYSOCAL   0xF3U
#define CMD_SYSGCAL   0xF4U
#define CMD_SYNC      0xFCU
#define CMD_STANDBY   0xFDU
#define CMD_RESET     0xFEU

/* STATUS 寄存器配置位 */
#define DRDY            0x01U
#define BUFFER_ENABLE   (0x01U << 1)
#define BUFFER_DISABLE  (0x00U << 1)
#define AUTO_CAL        (0x01U << 2)
#define MSB_FIRST       (0x00U << 3)
#define LSB_FIRST       (0x01U << 3)

/* MUX 寄存器正输入通道 */
#define POSITIVE_AIN0    (0x00U << 4)
#define POSITIVE_AIN1    (0x01U << 4)
#define POSITIVE_AIN2    (0x02U << 4)
#define POSITIVE_AIN3    (0x03U << 4)
#define POSITIVE_AIN4    (0x04U << 4)
#define POSITIVE_AIN5    (0x05U << 4)
#define POSITIVE_AIN6    (0x06U << 4)
#define POSITIVE_AIN7    (0x07U << 4)
#define POSITIVE_AINCOM  (0x08U << 4)

/* MUX 寄存器负输入通道 */
#define NEGTIVE_AIN0     0x00U
#define NEGTIVE_AIN1     0x01U
#define NEGTIVE_AIN2     0x02U
#define NEGTIVE_AIN3     0x03U
#define NEGTIVE_AIN4     0x04U
#define NEGTIVE_AIN5     0x05U
#define NEGTIVE_AIN6     0x06U
#define NEGTIVE_AIN7     0x07U
#define NEGTIVE_AINCOM   0x08U

/* ADCON 寄存器 PGA 增益 */
#define PGA_1   0x00U
#define PGA_2   0x01U
#define PGA_4   0x02U
#define PGA_8   0x03U
#define PGA_16  0x04U
#define PGA_32  0x05U
#define PGA_64  0x06U

/* ADCON 寄存器传感器检测电流 */
#define SENSOR_DETECT_OFF    (0x00U << 3)
#define SENSOR_DETECT_0_5UA  (0x01U << 3)
#define SENSOR_DETECT_2UA    (0x01U << 3)
#define SENSOR_DETECT_10UA   (0x03U << 3)

/* ADCON 寄存器时钟输出配置 */
#define CLOCK_OUT_OFF        (0x00U << 5)
#define CLOCK_OUT_FCLK       (0x01U << 5)
#define CLOCK_OUT_FCLK_HALF  (0x02U << 5)
#define CLOCK_OUT_FCLK_QUAD  (0x03U << 5)

/* DRATE 寄存器支持的数据速率 */
#define DATA_RATE_2_5    0x03U
#define DATA_RATE_5      0x13U
#define DATA_RATE_10     0x23U
#define DATA_RATE_15     0x33U
#define DATA_RATE_25     0x43U
#define DATA_RATE_30     0x53U
#define DATA_RATE_50     0x63U
#define DATA_RATE_60     0x72U
#define DATA_RATE_100    0x82U
#define DATA_RATE_500    0x92U
#define DATA_RATE_1000   0xA1U
#define DATA_RATE_2000   0xB0U
#define DATA_RATE_3750   0xC0U
#define DATA_RATE_7500   0xD0U
#define DATA_RATE_15000  0xE0U
#define DATA_RATE_30000  0xF0U

#define MY_SAMPLE_TIME_RATE  DATA_RATE_10
#define MY_SAMPLE_WAVE_RATE  DATA_RATE_2000

/* ADS1256 操作状态码 */
typedef enum
{
    ADS1256_OK = 0,
    ADS1256_ERROR_PARAMETER,
    ADS1256_ERROR_SPI,
    ADS1256_ERROR_RTOS
} ADS1256_Status;

/**
 * 使用工程默认数据速率初始化 ADS1256。
 * @return 成功返回 ADS1256_OK，否则返回初始化错误码。
 */
ADS1256_Status ADS1256_Init(void);

/**
 * 初始化 HAL 接口并按指定数据速率配置 ADS1256。
 * @param dataRate ADS1256 支持的数据速率寄存器值。
 * @return 成功返回 ADS1256_OK，否则返回初始化错误码。
 */
ADS1256_Status ADS1256_InitWithDataRate(uint8_t dataRate);

/**
 * 等待 ADS1256 数据就绪信号。
 * @param timeout CMSIS-RTOS2 等待超时节拍数，可使用 osWaitForever。
 * @return 成功返回 osOK，超时或 RTOS 错误时返回对应状态。
 */
osStatus_t ADS1256_WaitDataReady(uint32_t timeout);

/**
 * 读取 ADS1256 的 24 位二进制补码样本并扩展为有符号 32 位值。
 * @param sample 接收样本的输出指针，不得为空。
 * @return 成功返回 ADS1256_OK，否则返回参数或 SPI 错误码。
 */
ADS1256_Status ADS1256_ReadData(int32_t *sample);

/**
 * 将 16 位二进制补码 ADC 码转换为毫伏值。
 * @param ad_value 16 位 ADC 二进制补码数据。
 * @return 换算后的有符号毫伏值。
 */
int16_t ADS1256_ConvertToVoltage(uint16_t ad_value);

/**
 * 向 ADS1256 发送停止连续读数命令。
 * @return 成功返回 ADS1256_OK，SPI 错误时返回 ADS1256_ERROR_SPI。
 */
ADS1256_Status ADS1256_StopContinuousRead(void);


unsigned short AD_read_data(void);


#endif /* __ADS1256_H */