#include "ads1256.h"
#include "main.h"
#include "spi.h"
#include "MyLib.h"
#include <stdio.h>
#include <limits.h>

typedef uint16_t uint16;
typedef int16_t int16;

#define ADS1256_SPI_TIMEOUT_MS 10U
#define ADS1256_CALIBRATION_TIMEOUT_MS 5000U
#define ADS1256_REFERENCE_MV 2500L

volatile uint8_t g_ads1256Flag = 0U;
static osSemaphoreId_t g_ads1256DataReadySemaphore = NULL;
static volatile ADS1256_Status g_ads1256LastStatus = ADS1256_OK;

void ADS1256_SetCS(uint8_t state);
void ADS1256_SetRST(uint8_t state);

/**
 * 确认数据就绪信号量已经创建。
 * @return 成功返回 ADS1256_OK，创建失败返回 ADS1256_ERROR_RTOS。
 */
static ADS1256_Status ADS1256_EnsureDataReadySemaphore(void)
{
    if (g_ads1256DataReadySemaphore == NULL)
    {
        g_ads1256DataReadySemaphore = osSemaphoreNew(1U, 0U, NULL);
    }
    return (g_ads1256DataReadySemaphore != NULL) ? ADS1256_OK : ADS1256_ERROR_RTOS;
}

/**
 * 按毫秒延时；调度器运行时使用 CMSIS-RTOS2 延时，否则使用 HAL 延时。
 * @param milliseconds 延时时长，单位为毫秒。
 * @return 无。
 */
static void ADS1256_DelayMilliseconds(uint32_t milliseconds)
{
    if (osKernelGetState() == osKernelRunning)
    {
        uint64_t ticks = ((uint64_t)milliseconds * osKernelGetTickFreq() + 999U) / 1000U;
        (void)osDelay((ticks == 0U) ? 1U : (uint32_t)ticks);
    }
    else
    {
        HAL_Delay(milliseconds);
    }
}

/**
 * 检查数据速率寄存器值是否受 ADS1256 支持。
 * @param dataRate DRATE 寄存器配置值。
 * @return 支持时返回 1，不支持时返回 0。
 */
static uint8_t ADS1256_IsDataRateSupported(uint8_t dataRate)
{
    switch (dataRate)
    {
        case DATA_RATE_2_5: case DATA_RATE_5: case DATA_RATE_10:
        case DATA_RATE_15: case DATA_RATE_25: case DATA_RATE_30:
        case DATA_RATE_50: case DATA_RATE_60: case DATA_RATE_100:
        case DATA_RATE_500: case DATA_RATE_1000: case DATA_RATE_2000:
        case DATA_RATE_3750: case DATA_RATE_7500: case DATA_RATE_15000:
        case DATA_RATE_30000:
            return 1U;
        default:
            return 0U;
    }
}

/**
 * 通过 HAL SPI2 发送一个字节并接收一个字节。
 * @param txData 要发送的数据。
 * @param rxData 接收数据的输出指针，不得为空。
 * @return 成功返回 ADS1256_OK，参数或 SPI 错误时返回相应错误码。
 */
static ADS1256_Status ADS1256_TransferByte(uint8_t txData, uint8_t *rxData)
{
    uint8_t received = 0U;
    if ((rxData == NULL) || (hspi2.Instance != SPI2))
    {
        return (rxData == NULL) ? ADS1256_ERROR_PARAMETER : ADS1256_ERROR_SPI;
    }
    if (HAL_SPI_TransmitReceive(&hspi2, &txData, &received, 1U,
                                ADS1256_SPI_TIMEOUT_MS) != HAL_OK)
    {
        g_ads1256LastStatus = ADS1256_ERROR_SPI;
        return ADS1256_ERROR_SPI;
    }
    *rxData = received;
    g_ads1256LastStatus = ADS1256_OK;
    return ADS1256_OK;
}

/**
 * 向 ADS1256 指定寄存器写入一个字节，并在操作后释放片选。
 * @param regName 目标寄存器地址。
 * @param regData 要写入的寄存器值。
 * @return 成功返回 ADS1256_OK，否则返回 SPI 错误码。
 */
static ADS1256_Status ADS1256_WriteRegister(uint8_t regName, uint8_t regData)
{
    uint8_t ignored;
    ADS1256_Status status;
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte((uint8_t)(CMD_WREG | regName), &ignored);
    if (status == ADS1256_OK) status = ADS1256_TransferByte(0U, &ignored);
    if (status == ADS1256_OK) status = ADS1256_TransferByte(regData, &ignored);
    ADS1256_SetCS(1U);
    _delay_us(10);
    return status;
}

/**
 * 读取 ADS1256 指定寄存器的一个字节。
 * @param regName 目标寄存器地址。
 * @param regData 接收寄存器值的输出指针，不得为空。
 * @return 成功返回 ADS1256_OK，参数或 SPI 错误时返回相应错误码。
 */
static ADS1256_Status ADS1256_ReadRegister(uint8_t regName, uint8_t *regData)
{
    uint8_t ignored;
    ADS1256_Status status;
    if (regData == NULL) return ADS1256_ERROR_PARAMETER;
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte((uint8_t)(CMD_RREG | regName), &ignored);
    if (status == ADS1256_OK) status = ADS1256_TransferByte(0U, &ignored);
    if (status == ADS1256_OK)
    {
        _delay_us(10);
        status = ADS1256_TransferByte(0xFFU, regData);
    }
    ADS1256_SetCS(1U);
    _delay_us(10);
    return status;
}

/**
 * 复位芯片并配置输入、增益和数据速率，完成自校准后进入连续读数模式。
 * @param muxValue MUX 寄存器配置值。
 * @param pgaValue ADCON 寄存器配置值。
 * @param dataRate DRATE 寄存器配置值。
 * @return 成功返回 ADS1256_OK，否则返回参数、SPI 或 RTOS 错误码。
 */
static ADS1256_Status ADS1256_Configure(uint8_t muxValue, uint8_t pgaValue,
                                         uint8_t dataRate)
{
    static const uint8_t calibration[][2] = {
        {REG_OFC0, 0x00U}, {REG_OFC1, 0x00U}, {REG_OFC2, 0x00U},
        {REG_FSC0, 0xA0U}, {REG_FSC1, 0x99U}, {REG_FSC2, 0x3AU}
    };
    uint8_t ignored;
    uint8_t i;
    ADS1256_Status status;
    osStatus_t waitStatus;
    uint32_t timeoutTicks;

    if (ADS1256_IsDataRateSupported(dataRate) == 0U) return ADS1256_ERROR_PARAMETER;
    status = ADS1256_EnsureDataReadySemaphore();
    if (status != ADS1256_OK) return status;
    while (osSemaphoreAcquire(g_ads1256DataReadySemaphore, 0U) == osOK) { }

    g_ads1256Flag = 0U;
    ADS1256_SetCS(1U);
    ADS1256_SetRST(0U);
    ADS1256_DelayMilliseconds(1U);
    ADS1256_SetRST(1U);
    ADS1256_DelayMilliseconds(1U);
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte(CMD_RESET, &ignored);
    ADS1256_SetCS(1U);
    if (status != ADS1256_OK) return status;
    ADS1256_DelayMilliseconds(1U);

    status = ADS1256_WriteRegister(REG_STATUS, (uint8_t)(BUFFER_ENABLE | MSB_FIRST));
    if (status == ADS1256_OK) status = ADS1256_WriteRegister(REG_MUX, muxValue);
    if (status == ADS1256_OK) status = ADS1256_WriteRegister(REG_ADCON, pgaValue);
    if (status == ADS1256_OK) status = ADS1256_WriteRegister(REG_DRATE, dataRate);
    for (i = 0U; (status == ADS1256_OK) && (i < sizeof(calibration) / sizeof(calibration[0])); i++)
    {
        status = ADS1256_WriteRegister(calibration[i][0], calibration[i][1]);
    }
    if (status != ADS1256_OK) return status;

    g_ads1256Flag = 0U;
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte(CMD_SELFCAL, &ignored);
    ADS1256_SetCS(1U);
    if (status != ADS1256_OK) return status;

    timeoutTicks = (uint32_t)(((uint64_t)ADS1256_CALIBRATION_TIMEOUT_MS *
                               osKernelGetTickFreq() + 999U) / 1000U);
    waitStatus = ADS1256_WaitDataReady(timeoutTicks);
    if (waitStatus != osOK)
    {
        g_ads1256LastStatus = ADS1256_ERROR_RTOS;
        return ADS1256_ERROR_RTOS;
    }
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte(CMD_RDATAC, &ignored);
    ADS1256_SetCS(1U);
    g_ads1256LastStatus = status;
    return status;
}

/**
 * 通过 HAL 将 ADS1256 数据就绪引脚配置为下降沿外部中断。
 * @return 无。
 */
void EXTI12_Config(void)
{
    GPIO_InitTypeDef gpioInit = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    gpioInit.Pin = ADS_DRDY_Pin;
    gpioInit.Mode = GPIO_MODE_IT_FALLING;
    gpioInit.Pull = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADS_DRDY_GPIO_Port, &gpioInit);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
 * 在 HAL SPI2 尚未初始化时调用工程生成的初始化函数。
 * @return 无。
 */
void SPI2_Init(void)
{
    if (hspi2.Instance != SPI2) MX_SPI2_Init();
}

/**
 * 设置 ADS1256 片选引脚电平。
 * @param state 非零时拉高片选，零时拉低片选。
 * @return 无。
 */
void ADS1256_SetCS(uint8_t state)
{
    HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin,
                      (state != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * 设置 ADS1256 硬件复位引脚电平。
 * @param state 非零时释放复位，零时保持复位。
 * @return 无。
 */
void ADS1256_SetRST(uint8_t state)
{
    HAL_GPIO_WritePin(ADS_RSR_GPIO_Port, ADS_RSR_Pin,
                      (state != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * 通过 HAL SPI2 写入一个字节并返回同时接收的字节。
 * @param byte 要发送的数据。
 * @return SPI 同时接收的数据；传输失败时返回零。
 */
unsigned char SPI_Byte_Write(unsigned char byte)
{
    uint8_t received = 0U;
    ADS1256_Status status = ADS1256_TransferByte((uint8_t)byte, &received);
    if (status != ADS1256_OK)
    {
        printf("ADS1256 SPI byte transfer failed: %u\r\n", (unsigned int)status);
        return 0U;
    }
    return (unsigned char)received;
}

/**
 * 通过 HAL SPI2 发送空闲字节并读取一个字节。
 * @return 成功时返回接收的数据，失败时返回零。
 */
unsigned char SPI_Byte_Read(void)
{
    return SPI_Byte_Write(0xFFU);
}

/**
 * 写入 ADS1256 寄存器，并在失败时输出错误状态。
 * @param reg_name 目标寄存器地址。
 * @param reg_data 要写入的寄存器值。
 * @return 无。
 */
void ADS1255_Write_Reg(unsigned char reg_name, unsigned char reg_data)
{
    ADS1256_Status status = ADS1256_WriteRegister((uint8_t)reg_name, (uint8_t)reg_data);
    if (status != ADS1256_OK) printf("ADS1256 register write failed: %u\r\n", (unsigned int)status);
}

/**
 * 读取 ADS1256 寄存器，并在失败时输出错误状态。
 * @param reg_name 目标寄存器地址。
 * @return 成功时返回寄存器值，失败时返回零。
 */
unsigned char ADS1255_Read_Reg(unsigned char reg_name)
{
    uint8_t value = 0U;
    ADS1256_Status status = ADS1256_ReadRegister((uint8_t)reg_name, &value);
    if (status != ADS1256_OK) printf("ADS1256 register read failed: %u\r\n", (unsigned int)status);
    return (unsigned char)value;
}

/**
 * 调用工程提供的 DWT 微秒延时实现。
 * @param count 延时时长，单位为微秒。
 * @return 无。
 */
void i2c_delay_us(uint16 count)
{
    _delay_us((int32_t)count);
}

/**
 * 以兼容 FreeRTOS 的方式延时指定毫秒数。
 * @param count 延时时长，单位为毫秒。
 * @return 无。
 */
void i2c_delay_ms(uint16 count)
{
    while (count > 0U)
    {
        ADS1256_DelayMilliseconds(1U);
        count--;
    }
}

/**
 * 等待数据就绪并读取 ADS1256 样本的高 16 位。
 * @return 成功时返回样本高 16 位，等待或读取失败时返回零。
 */
uint16 AD_read_data(void)
{
    int32_t sample = 0;
    uint32_t timeout = osKernelGetTickFreq();
    osStatus_t waitStatus = ADS1256_WaitDataReady(timeout);
    ADS1256_Status readStatus;
    if (waitStatus != osOK)
    {
        printf("ADS1256 data-ready wait failed: %u\r\n", (unsigned int)waitStatus);
        return 0U;
    }
    readStatus = ADS1256_ReadData(&sample);
    if (readStatus != ADS1256_OK)
    {
        printf("ADS1256 data read failed: %u\r\n", (unsigned int)readStatus);
        return 0U;
    }
    return (uint16)((uint32_t)sample >> 8);
}

/**
 * 初始化默认 AIN0-AIN1、PGA=1 配置，并使用指定数据速率。
 * @param RATEx ADS1256 支持的数据速率寄存器值。
 * @return 无；初始化状态保存在内部状态中，失败时输出错误信息。
 */
void ADS1255_Init(unsigned char RATEx)
{
    ADS1256_Status status = ADS1256_EnsureDataReadySemaphore();

    if (status == ADS1256_OK)
    {
        EXTI12_Config();
        SPI2_Init();
        status = ADS1256_Configure(
            (uint8_t)(POSITIVE_AIN0 | NEGTIVE_AIN1),
            (uint8_t)(PGA_1 | SENSOR_DETECT_OFF | CLOCK_OUT_OFF),
            (uint8_t)RATEx);
    }
    g_ads1256LastStatus = status;
    if (status != ADS1256_OK) printf("ADS1256 initialization failed: %u\r\n", (unsigned int)status);
}

/**
 * 向 ADS1256 发送停止连续读数命令。
 * @return 成功返回 ADS1256_OK，SPI 传输失败时返回 ADS1256_ERROR_SPI。
 */
ADS1256_Status ADS1256_StopContinuousRead(void)
{
    uint8_t ignored;
    ADS1256_Status status;
    ADS1256_SetCS(0U);
    status = ADS1256_TransferByte(CMD_SDATAC, &ignored);
    ADS1256_SetCS(1U);
    g_ads1256LastStatus = status;
    return status;
}

/**
 * 使用工程默认数据速率初始化 ADS1256。
 * @return 成功返回 ADS1256_OK，否则返回初始化错误码。
 */
ADS1256_Status ADS1256_Init(void)
{
    return ADS1256_InitWithDataRate(MY_SAMPLE_TIME_RATE);
}

/**
 * 初始化信号量、外部中断、SPI2，并按指定数据速率配置 ADS1256。
 * @param dataRate ADS1256 支持的数据速率寄存器值。
 * @return 成功返回 ADS1256_OK，否则返回初始化错误码。
 */
ADS1256_Status ADS1256_InitWithDataRate(uint8_t dataRate)
{
    ADS1256_Status status;
    if (ADS1256_IsDataRateSupported(dataRate) == 0U) return ADS1256_ERROR_PARAMETER;
    status = ADS1256_EnsureDataReadySemaphore();
    if (status != ADS1256_OK) return status;
    EXTI12_Config();
    SPI2_Init();
    status = ADS1256_Configure((uint8_t)(POSITIVE_AIN0 | NEGTIVE_AIN1),
                               (uint8_t)(PGA_1 | SENSOR_DETECT_OFF | CLOCK_OUT_OFF),
                               dataRate);
    g_ads1256LastStatus = status;
    return status;
}

/**
 * 通过 CMSIS-RTOS2 信号量等待数据就绪中断通知。
 * @param timeout CMSIS-RTOS2 等待超时节拍数，可使用 osWaitForever。
 * @return 成功返回 osOK，超时或 RTOS 错误时返回对应状态。
 */
osStatus_t ADS1256_WaitDataReady(uint32_t timeout)
{
    if (ADS1256_EnsureDataReadySemaphore() != ADS1256_OK) return osErrorResource;
    return osSemaphoreAcquire(g_ads1256DataReadySemaphore, timeout);
}

/**
 * 通过 HAL SPI2 读取 24 位二进制补码样本并扩展为有符号 32 位值。
 * @param sample 接收样本的输出指针，不得为空。
 * @return 成功返回 ADS1256_OK，参数或 SPI 错误时返回相应错误码。
 */
ADS1256_Status ADS1256_ReadData(int32_t *sample)
{
    uint8_t tx[3] = {0xFFU, 0xFFU, 0xFFU};
    uint8_t rx[3] = {0U, 0U, 0U};
    HAL_StatusTypeDef halStatus;
    if (sample == NULL) return ADS1256_ERROR_PARAMETER;
    if (hspi2.Instance != SPI2) return ADS1256_ERROR_SPI;
    _delay_us(10);
    ADS1256_SetCS(0U);
    halStatus = HAL_SPI_TransmitReceive(&hspi2, tx, rx, (uint16_t)sizeof(rx),
                                        ADS1256_SPI_TIMEOUT_MS);
    ADS1256_SetCS(1U);
    if (halStatus != HAL_OK)
    {
        g_ads1256LastStatus = ADS1256_ERROR_SPI;
        return ADS1256_ERROR_SPI;
    }
    *sample = ((int32_t)rx[0] << 16) | ((int32_t)rx[1] << 8) | rx[2];
    if ((*sample & 0x00800000L) != 0L) *sample |= (int32_t)0xFF000000UL;
    g_ads1256Flag = 0U;
    g_ads1256LastStatus = ADS1256_OK;
    return ADS1256_OK;
}

/**
 * 将 16 位二进制补码 ADC 码转换为以毫伏为单位的有符号电压值。
 * @param ad_value 16 位 ADC 二进制补码数据。
 * @return 换算后的毫伏值，并限制在 int16_t 范围内。
 */
int16 ADS1256_ConvertToVoltage(uint16 ad_value)
{
    int32_t signedValue = ((ad_value & 0x8000U) != 0U) ?
        ((int32_t)ad_value - 65536L) : (int32_t)ad_value;
    int32_t voltage = (signedValue * ADS1256_REFERENCE_MV) / 32768L;
    if (voltage > INT16_MAX) voltage = INT16_MAX;
    if (voltage < INT16_MIN) voltage = INT16_MIN;
    return (int16)voltage;
}

/**
 * 按指定输入通道、PGA 和数据速率配置 ADS1256。
 * @param AINx MUX 寄存器输入通道配置值。
 * @param PGAx ADCON 寄存器增益配置值。
 * @param RATEx DRATE 寄存器数据速率值。
 * @return 无；初始化失败时输出错误信息。
 */
void ADS1255_Init_Pra(unsigned char AINx, unsigned char PGAx, unsigned char RATEx)
{
    ADS1256_Status status = ADS1256_EnsureDataReadySemaphore();
    if (status == ADS1256_OK)
    {
        EXTI12_Config();
        SPI2_Init();
        status = ADS1256_Configure((uint8_t)AINx, (uint8_t)PGAx, (uint8_t)RATEx);
    }
    g_ads1256LastStatus = status;
    if (status != ADS1256_OK)
        printf("ADS1256 parameter initialization failed: %u\r\n", (unsigned int)status);
}

/**
 * 在 HAL 外部中断回调中通知等待任务 ADS1256 数据已经就绪。
 * @param GPIO_Pin 触发中断的 GPIO 引脚编号。
 * @return 无。
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ADS_DRDY_Pin)
    {
        g_ads1256Flag = 1U;
        if ((g_ads1256DataReadySemaphore != NULL) &&
            (osSemaphoreRelease(g_ads1256DataReadySemaphore) != osOK))
            g_ads1256LastStatus = ADS1256_ERROR_RTOS;
    }
}


