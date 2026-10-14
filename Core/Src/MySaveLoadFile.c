#include "MySaveLoadFile.h"
#include "main.h"
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__align(4) 	_SYS_PARA_STRUCT   SysPara;
_ScanFileNameList FileNameList[MAX_FILE_SAVE_NUM] ;




FIL fp;
DIR dir_handle;
static BYTE formatWork[_MAX_SS];

/**
 * @brief 使用 HAL 时钟控制接口及 CRC 外设计算系统参数校验值。
 * @param 无。
 * @return 系统参数结构中校验和字段之前所有 32 位字的 CRC 结果。
 * @note 当前工程未包含 HAL CRC 专用驱动，因此使用 HAL RCC 宏控制外设，并通过设备寄存器写入数据。
 */
static uint32_t calculateSysParameterChecksum(void)
{
    const uint32_t *data = (const uint32_t *)&SysPara;
    uint32_t wordCount = (uint32_t)(sizeof(SysPara) / sizeof(uint32_t) - 1U);
    uint32_t index;

    __HAL_RCC_CRC_CLK_ENABLE();
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();

    for (index = 0U; index < wordCount; ++index)
    {
        CRC->DR = data[index];
    }

    return CRC->DR;
}



/**
 * @brief 初始化 FatFs 用户卷并准备应用所需目录及参数。
 * @param 无。
 * @return 无；错误通过 USERFatFSResult 及日志反馈。
 * @note 仅在卷没有有效文件系统时执行格式化。
 */
void FileInit(void)
{
    static const char * const directoryNames[] = {
        "config", "GuangPu", "ShiJian", "DingLiang", "Calibration", "FORMAT1"
    };
    char directoryPath[32];
    size_t i;
    FRESULT result;

    MX_FATFS_Init();
    if (retUSER != 0U)
    {
        USERFatFSResult = FR_NOT_READY;
        printf("FatFs USER drive initialization failed.\r\n");
        return;
    }

    USERFatFSResult = f_mount(&USERFatFS, USERPath, 1U);
    if (USERFatFSResult == FR_NO_FILESYSTEM)
    {
        USERFatFSResult = f_mkfs(USERPath, FM_ANY, 0U, formatWork,
                                 sizeof(formatWork));
        if (USERFatFSResult != FR_OK)
        {
            printf("FatFs format failed.\r\n");
            return;
        }
        USERFatFSResult = f_mount(&USERFatFS, USERPath, 1U);
    }
    if (USERFatFSResult != FR_OK)
    {
        printf("FatFs USER volume mount failed.\r\n");
        return;
    }

    for (i = 0U; i < (sizeof(directoryNames) / sizeof(directoryNames[0])); ++i)
    {
        int pathLength = snprintf(directoryPath, sizeof(directoryPath), "%s/%s",
                                  USERPath, directoryNames[i]);
        if ((pathLength < 0) || ((size_t)pathLength >= sizeof(directoryPath)))
        {
            USERFatFSResult = FR_INVALID_NAME;
            printf("FatFs directory path is too long.\r\n");
            return;
        }

        result = f_mkdir((const TCHAR *)directoryPath);
        if ((result != FR_OK) && (result != FR_EXIST))
        {
            USERFatFSResult = result;
            printf("FatFs application directory creation failed.\r\n");
            return;
        }
    }

    USERFatFSResult = FR_OK;
    ParameterCheck();
    SysShowAllFileNames("config");
    SysShowAllFileNames("GuangPu");
    SysShowAllFileNames("ShiJian");
    SysShowAllFileNames("DingLiang");
}

/**
 * @brief 将仪器参数恢复为默认值并写入系统参数文件。
 * @param 无。
 * @return 无；文件写入失败时记录错误日志。
 * @note 同时更新 SysPara 校验和，并设置光源及高压输出的默认安全状态。
 */
void ParameterInit(void)
{
    FRESULT result;
    UINT bytesWritten = 0U;
    char parameterPath[32];
    int pathLength;

    memset(&SysPara, 0, sizeof(SysPara));
    SysPara.InitFlag = 0x55AA;
    SysPara.FactoryWaveCorrect = 0;
    SysPara.UserWaveCorrect = 0;
    SysPara.GuPuPar.Speed = SCAN_SPEED_FAST;
    SysPara.GuPuPar.Interval = Unit_0_1nm;
    SysPara.GuPuPar.WaveLenEnd = 500 * Unit_1nm;
    SysPara.GuPuPar.WaveLenStart = 300 * Unit_1nm;
    SysPara.GuPuPar.data_E_Start = 0;
    SysPara.GuPuPar.data_E_End = 2000;
    SysPara.LightPowerS.emit_light = EMIT_LIGHT_1;
    SysPara.LightPowerS.pmt_on_off = PMT_OFF;
    SysPara.LightPowerS.pmt_voltage = 100;
    SysPara.LightPowerS.gain = PMT_GAIN_1;
    SysPara.LightPowerS.sensor = 2;

    SysPara.Checksum = calculateSysParameterChecksum();
    BSP_Light_Sel(1);
    BSP_Set_HV(0);

    pathLength = snprintf(parameterPath, sizeof(parameterPath), "%s/config/Sys.txt",
                          USERPath);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(parameterPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        printf("System parameter path is too long.\r\n");
        return;
    }
    result = f_open(&fp, (const TCHAR *)parameterPath,
                    FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        printf("Failed to open system parameter file for writing.\r\n");
        return;
    }

    result = f_write(&fp, &SysPara, (UINT)sizeof(SysPara), &bytesWritten);
    if ((result == FR_OK) && (bytesWritten != sizeof(SysPara)))
    {
        result = FR_DISK_ERR;
    }
    if (f_close(&fp) != FR_OK && result == FR_OK)
    {
        result = FR_DISK_ERR;
    }
    USERFatFSResult = result;
    if (result != FR_OK)
    {
        printf("Failed to write complete system parameters.\r\n");
    }
}


/**
 * @brief 从系统参数文件读取并校验光谱参数，无效或缺失时恢复默认值。
 * @param 无。
 * @return 无。
 * @note 校验后将光谱参数加载到运行时结构，并确保光源和高压输出处于默认安全状态。
 */
void ParameterCheck(void)
{
    uint32_t checksum = 0U;
    UINT bytesRead = 0U;
    FRESULT result;
    char parameterPath[32];
    int pathLength;

    pathLength = snprintf(parameterPath, sizeof(parameterPath), "%s/config/Sys.txt",
                          USERPath);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(parameterPath)))
    {
        result = FR_INVALID_NAME;
    }
    else
    {
        result = f_open(&fp, (const TCHAR *)parameterPath, FA_READ);
        if (result == FR_OK)
        {
            if (f_size(&fp) == sizeof(SysPara))
            {
                result = f_read(&fp, &SysPara, (UINT)sizeof(SysPara), &bytesRead);
            }
            else
            {
                printf("System parameter file has an unexpected size; using defaults.\r\n");
            }
            if (f_close(&fp) != FR_OK && result == FR_OK)
            {
                result = FR_DISK_ERR;
            }
        }
    }

    if ((result != FR_OK) || (bytesRead != sizeof(SysPara)) ||
        (SysPara.InitFlag != 0x55AA))
    {
        if (result != FR_OK)
        {
            USERFatFSResult = result;
            printf("Failed to read system parameter file; using defaults.\r\n");
        }
        else if (SysPara.InitFlag != 0x55AA)
        {
            printf("System parameter validation failed; using defaults.\r\n");
        }
        ParameterInit();
    }
    else
    {
        checksum = calculateSysParameterChecksum();
        if (SysPara.Checksum != checksum)
        {
            printf("System parameter checksum mismatch; using defaults.\r\n");
            ParameterInit();
        }
    }

    GuPuCeShiStruct.Speed = SysPara.GuPuPar.Speed;
    GuPuCeShiStruct.Interval = SysPara.GuPuPar.Interval;
    GuPuCeShiStruct.WaveLenEnd = SysPara.GuPuPar.WaveLenEnd;
    GuPuCeShiStruct.WaveLenStart = SysPara.GuPuPar.WaveLenStart;
    GuPuCeShiStruct.data_E_Start = SysPara.GuPuPar.data_E_Start;
    GuPuCeShiStruct.data_E_End = SysPara.GuPuPar.data_E_End;



    SysPara.LightPowerS.emit_light = EMIT_LIGHT_1;
    BSP_Light_Sel(1);
    SysPara.LightPowerS.pmt_on_off = PMT_OFF;
    BSP_Set_HV(0);
}
/**
 * @brief 更新系统参数校验和，并将当前参数完整写入参数文件。
 * @param 无。
 * @return 无；写入错误通过日志报告。
 * @note 使用 FatFs 文件写入接口覆盖旧参数文件。
 */
void UpdateSysParameter(void)
{
    FRESULT result;
    UINT bytesWritten = 0U;
    char parameterPath[32];
    int pathLength;

    SysPara.Checksum = calculateSysParameterChecksum();
    pathLength = snprintf(parameterPath, sizeof(parameterPath), "%s/config/Sys.txt",
                          USERPath);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(parameterPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        printf("System parameter path is too long.\r\n");
        return;
    }

    result = f_open(&fp, (const TCHAR *)parameterPath,
                    FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        printf("Failed to open system parameter file for update.\r\n");
        return;
    }

    result = f_write(&fp, &SysPara, (UINT)sizeof(SysPara), &bytesWritten);
    if ((result == FR_OK) && (bytesWritten != sizeof(SysPara)))
    {
        result = FR_DISK_ERR;
    }
    if (f_close(&fp) != FR_OK && result == FR_OK)
    {
        result = FR_DISK_ERR;
    }
    USERFatFSResult = result;
    if (result != FR_OK)
    {
        printf("Failed to write complete system parameters.\r\n");
    }
}


/**
 * @brief 删除指定数据类别目录中保存序号匹配的文件。
 * @param type 文件类别，取 SAVE_LOAD_TYPE_*。
 * @param save_pos 要删除的保存槽序号。
 * @return 成功删除的文件数；类别无效或目录/文件操作失败时返回错误码。
 * @note 使用 FatFs R0.12c 的目录接口，并以完整路径删除目录项。
 */
uint8_t SysDeleteFileAccordingToSavePos(uint8_t type, uint8_t save_pos)
{
    uint8_t deletedCount = 0U;
    char folderName[16];
    char folderPath[32];
    char filePath[288];
    FILINFO fileInfo;
    FRESULT result;
    char *firstSeparator;
    char *secondSeparator;
    char *parseEnd;
    unsigned long filePosition;
    int pathLength;

    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU: strcpy(folderName, "GuangPu"); break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE: strcpy(folderName, "ShiJian"); break;
        case SAVE_LOAD_TYPE_DING_LIANG: strcpy(folderName, "DingLiang"); break;
        case SAVE_LOAD_TYPE_CALIBRATION: strcpy(folderName, "Calibration"); break;
        default: return 0xFFU;
    }
    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        return 0xFEU;
    }

    result = f_opendir(&dir_handle, (const TCHAR *)folderPath);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return 0xFEU;
    }

    for (;;)
    {
        result = f_readdir(&dir_handle, &fileInfo);
        if (result != FR_OK || fileInfo.fname[0] == '\0')
        {
            break;
        }
        if ((fileInfo.fattrib & AM_DIR) != 0U)
        {
            continue;
        }

        firstSeparator = strchr(fileInfo.fname, '_');
        secondSeparator = (firstSeparator != NULL)
            ? strchr(firstSeparator + 1, '_') : NULL;
        if ((firstSeparator == NULL) || (secondSeparator == NULL) ||
            (secondSeparator == firstSeparator + 1))
        {
            continue;
        }
        *secondSeparator = '\0';
        filePosition = strtoul(firstSeparator + 1, &parseEnd, 10);
        *secondSeparator = '_';
        if ((parseEnd != secondSeparator) || (filePosition != save_pos))
        {
            continue;
        }

        pathLength = snprintf(filePath, sizeof(filePath), "%s/%s", folderPath,
                              fileInfo.fname);
        if ((pathLength < 0) || ((size_t)pathLength >= sizeof(filePath)))
        {
            result = FR_INVALID_NAME;
            break;
        }
        result = f_unlink((const TCHAR *)filePath);
        if (result != FR_OK)
        {
            break;
        }
        if (deletedCount < 0xFDU)
        {
            ++deletedCount;
        }
    }

    if (f_closedir(&dir_handle) != FR_OK && result == FR_OK)
    {
        result = FR_DISK_ERR;
    }
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return 0xFEU;
    }
    return deletedCount;
}

/**
 * @brief 将当前光谱或动力学绘图数据保存到指定槽位的文件。
 * @param type 保存数据类别，目前支持光谱和动力学数据。
 * @param save_pos 保存槽序号，用于构造文件名。
 * @param FileName 文件名（不含保存槽前缀）。
 * @param data 保留的兼容参数；文件内容由当前绘图数据结构提供。
 * @param data_len 保留的兼容参数；文件内容长度由对应数据结构确定。
 * @return 0 表示成功；非零值表示参数、类别或 FatFs 文件操作错误。
 * @note 使用 FA_CREATE_ALWAYS | FA_WRITE 创建或覆盖文件，并检查每次写入结果。
 */
uint8_t SysSaveFiles(uint8_t type, uint8_t save_pos, char *FileName, uint8_t *data, uint32_t data_len)
{
    char folderName[16];
    char folderPath[32];
    char filePath[288];
    FRESULT result;
    FRESULT closeResult;
    UINT bytesWritten = 0U;
    UINT dataSize;
    void *scanData;
    int pathLength;
    uint8_t deletedCount;

    (void)data;
    (void)data_len;
    if ((FileName == NULL) || (FileName[0] == '\0') ||
        (strchr(FileName, '/') != NULL) || (strchr(FileName, '\\') != NULL))
    {
        return 0xFFU;
    }

    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU:
            strcpy(folderName, "GuangPu");
            scanData = GuangPuScanLine;
            dataSize = (UINT)sizeof(GuangPuScanLine);
            break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE:
            strcpy(folderName, "ShiJian");
            scanData = DongLiXueScanLine;
            dataSize = (UINT)sizeof(DongLiXueScanLine);
            break;
        default:
            return 0xFFU;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        return 0xFEU;
    }
    pathLength = snprintf(filePath, sizeof(filePath), "%s/_%u_%s", folderPath,
                          (unsigned int)save_pos, FileName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(filePath)))
    {
        return 0xFEU;
    }

    deletedCount = SysDeleteFileAccordingToSavePos(type, save_pos);
    if (deletedCount >= 0xFEU)
    {
        return 0xFEU;
    }

    result = f_open(&fp, (const TCHAR *)filePath, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return (uint8_t)result;
    }

    result = f_write(&fp, &gDraw, (UINT)sizeof(gDraw), &bytesWritten);
    if ((result == FR_OK) && (bytesWritten != sizeof(gDraw)))
    {
        result = FR_DISK_ERR;
    }
    if (result == FR_OK)
    {
        bytesWritten = 0U;
        result = f_write(&fp, scanData, dataSize, &bytesWritten);
        if ((result == FR_OK) && (bytesWritten != dataSize))
        {
            result = FR_DISK_ERR;
        }
    }

    closeResult = f_close(&fp);
    if ((result == FR_OK) && (closeResult != FR_OK))
    {
        result = closeResult;
    }
    USERFatFSResult = result;
    return (result == FR_OK) ? 0U : (uint8_t)result;
}


/**
 * @brief 扫描指定类别目录并更新保存文件名列表。
 * @param type 文件类别，取 SAVE_LOAD_TYPE_*。
 * @return 成功登记的文件数量；类别无效或目录读取失败时返回错误码。
 * @note 仅登记符合“_槽位_文件名”格式且槽位在有效范围内的普通文件。
 */
uint8_t SysLoadAllFileNames(uint8_t type)
{
    uint8_t fileCount = 0U;
    uint8_t index;
    char folderName[16];
    char folderPath[32];
    FILINFO fileInfo;
    FRESULT result;
    char *firstSeparator;
    char *secondSeparator;
    unsigned long filePosition;
    char *parseEnd;
    size_t nameLength;
    int pathLength;

    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU: strcpy(folderName, "GuangPu"); break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE: strcpy(folderName, "ShiJian"); break;
        case SAVE_LOAD_TYPE_DING_LIANG: strcpy(folderName, "DingLiang"); break;
        case SAVE_LOAD_TYPE_CALIBRATION: strcpy(folderName, "Calibration"); break;
        default: return 0xFFU;
    }

    for (index = 0U; index < MAX_FILE_SAVE_NUM; ++index)
    {
        memset(FileNameList[index].name, 0, MAX_FILE_NAME_LEN + 1U);
        FileNameList[index].select = 0xFFU;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        return 0xFEU;
    }
    result = f_opendir(&dir_handle, (const TCHAR *)folderPath);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return 0xFEU;
    }

    for (;;)
    {
        result = f_readdir(&dir_handle, &fileInfo);
        if (result != FR_OK || fileInfo.fname[0] == '\0')
        {
            break;
        }
        if ((fileInfo.fattrib & AM_DIR) != 0U)
        {
            continue;
        }

        firstSeparator = strchr(fileInfo.fname, '_');
        secondSeparator = (firstSeparator != NULL)
            ? strchr(firstSeparator + 1, '_') : NULL;
        if ((firstSeparator == NULL) || (secondSeparator == NULL) ||
            (secondSeparator == firstSeparator + 1))
        {
            continue;
        }

        *secondSeparator = '\0';
        filePosition = strtoul(firstSeparator + 1, &parseEnd, 10);
        *secondSeparator = '_';
        if ((parseEnd != secondSeparator) || (filePosition >= MAX_FILE_SAVE_NUM))
        {
            continue;
        }

        nameLength = strlen(secondSeparator + 1);
        if (nameLength > MAX_FILE_NAME_LEN)
        {
            nameLength = MAX_FILE_NAME_LEN;
        }
        memcpy(FileNameList[filePosition].name, secondSeparator + 1, nameLength);
        FileNameList[filePosition].name[nameLength] = '\0';
        FileNameList[filePosition].select = 0U;
        if (fileCount < 0xFEU)
        {
            ++fileCount;
        }
    }

    if (f_closedir(&dir_handle) != FR_OK && result == FR_OK)
    {
        result = FR_DISK_ERR;
    }
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return 0xFEU;
    }
    USERFatFSResult = FR_OK;
    return fileCount;
}

/**
 * @brief 按数据类别、保存槽位和文件名读取绘图数据。
 * @param type 文件类别，目前支持光谱和动力学数据。
 * @param load_pos 保存槽序号。
 * @param Filename 文件名（不含保存槽前缀）。
 * @param read_len 输出实际读取的字节数，不能为 NULL。
 * @return 0 表示完整读取成功；非零值表示参数、FatFs 或数据长度错误。
 * @note 依照保存时的数据布局恢复绘图参数和对应扫描缓冲区。
 */
uint8_t SysLoadFileByTypeAndName(uint8_t type, uint8_t load_pos, char *Filename, uint32_t *read_len)
{
    char folderName[16];
    char folderPath[32];
    char filePath[288];
    FRESULT result;
    FRESULT closeResult;
    UINT bytesRead = 0U;
    uint32_t totalBytesRead = 0U;
    uint32_t expectedSize;
    int pathLength;

    if ((Filename == NULL) || (read_len == NULL) || (Filename[0] == '\0') ||
        (strchr(Filename, '/') != NULL) || (strchr(Filename, '\\') != NULL))
    {
        return 0xFFU;
    }
    *read_len = 0U;

    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU:
            strcpy(folderName, "GuangPu");
            expectedSize = (uint32_t)sizeof(gDraw) + (uint32_t)sizeof(GuangPuScanLine);
            break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE:
            strcpy(folderName, "ShiJian");
            expectedSize = (uint32_t)sizeof(gDraw) + (uint32_t)sizeof(DongLiXueScanLine);
            break;
        default:
            return 0xFEU;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        return 0xFEU;
    }
    pathLength = snprintf(filePath, sizeof(filePath), "%s/_%u_%s", folderPath,
                          (unsigned int)load_pos, Filename);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(filePath)))
    {
        return 0xFEU;
    }

    result = f_open(&fp, (const TCHAR *)filePath, FA_READ);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return (uint8_t)result;
    }
    if (f_size(&fp) != expectedSize)
    {
        (void)f_close(&fp);
        return 0xFBU;
    }

    result = f_read(&fp, &gDraw, (UINT)sizeof(gDraw), &bytesRead);
    totalBytesRead += bytesRead;
    if ((result == FR_OK) && (bytesRead == sizeof(gDraw)))
    {
        if (type == SAVE_LOAD_TYPE_GUANG_PU)
        {
            result = f_read(&fp, GuangPuScanLine,
                            (UINT)sizeof(GuangPuScanLine), &bytesRead);
            if (bytesRead != sizeof(GuangPuScanLine))
            {
                result = (result == FR_OK) ? FR_DISK_ERR : result;
            }
        }
        else
        {
            result = f_read(&fp, DongLiXueScanLine,
                            (UINT)sizeof(DongLiXueScanLine), &bytesRead);
            if (bytesRead != sizeof(DongLiXueScanLine))
            {
                result = (result == FR_OK) ? FR_DISK_ERR : result;
            }
        }
        totalBytesRead += bytesRead;
    }
    else if (result == FR_OK)
    {
        result = FR_DISK_ERR;
    }

    closeResult = f_close(&fp);
    if (result == FR_OK)
    {
        result = closeResult;
    }
    *read_len = totalBytesRead;
    USERFatFSResult = result;
    if (result != FR_OK)
    {
        return (uint8_t)result;
    }
    return (totalBytesRead == expectedSize) ? 0U : 0xFBU;
}



/**
 * @brief 删除指定类别目录中由槽位和文件名确定的文件。
 * @param type 文件类别，取 SAVE_LOAD_TYPE_*。
 * @param delete_pos 保存槽序号。
 * @param Filename 文件名（不含保存槽前缀）。
 * @return 0 表示删除成功；非零值表示参数、类别或 FatFs 错误。
 * @note 通过完整逻辑卷路径调用 FatFs f_unlink。
 */
uint8_t SysDeleteFileByTypeAndName(uint8_t type, uint8_t delete_pos, char *Filename)
{
    char folderName[16];
    char folderPath[32];
    char filePath[288];
    FRESULT result;
    int pathLength;

    if ((Filename == NULL) || (Filename[0] == '\0') ||
        (strchr(Filename, '/') != NULL) || (strchr(Filename, '\\') != NULL))
    {
        return 0xFFU;
    }
    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU: strcpy(folderName, "GuangPu"); break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE: strcpy(folderName, "ShiJian"); break;
        case SAVE_LOAD_TYPE_DING_LIANG: strcpy(folderName, "DingLiang"); break;
        case SAVE_LOAD_TYPE_CALIBRATION: strcpy(folderName, "Calibration"); break;
        default: return 0xFEU;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        return 0xFEU;
    }
    pathLength = snprintf(filePath, sizeof(filePath), "%s/_%u_%s", folderPath,
                          (unsigned int)delete_pos, Filename);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(filePath)))
    {
        return 0xFEU;
    }

    result = f_unlink((const TCHAR *)filePath);
    USERFatFSResult = result;
    return (result == FR_OK) ? 0U : (uint8_t)result;
}


/**
 * @brief 在指定类别目录中重命名具有唯一旧名称的保存文件。
 * @param type 文件类别，取 SAVE_LOAD_TYPE_*。
 * @param old_filename 原文件名，不包含槽位前缀和目录路径。
 * @param new_filename 新文件名，不包含槽位前缀和目录路径。
 * @return 0 表示成功；非零值表示参数、类别、名称歧义或 FatFs 错误。
 * @note 由于接口不接收槽位号，目录中存在多个同名文件时拒绝重命名以避免误操作。
 */
uint8_t SysRenameFileByTypeAndName(uint8_t type, char *old_filename, char *new_filename)
{
    char folderName[16];
    char folderPath[32];
    char oldPath[288];
    char newPath[288];
    char oldBaseName[256];
    FILINFO fileInfo;
    FRESULT result;
    FRESULT closeResult;
    char *firstSeparator;
    char *secondSeparator;
    char *digit;
    size_t prefixLength;
    uint8_t matchCount = 0U;
    int pathLength;

    if ((old_filename == NULL) || (old_filename[0] == '\0') ||
        (new_filename == NULL) || (new_filename[0] == '\0') ||
        (strchr(old_filename, '/') != NULL) || (strchr(old_filename, '\\') != NULL) ||
        (strchr(new_filename, '/') != NULL) || (strchr(new_filename, '\\') != NULL))
    {
        return 0xFFU;
    }
    switch (type)
    {
        case SAVE_LOAD_TYPE_GUANG_PU: strcpy(folderName, "GuangPu"); break;
        case SAVE_LOAD_TYPE_DONG_LI_XUE: strcpy(folderName, "ShiJian"); break;
        case SAVE_LOAD_TYPE_DING_LIANG: strcpy(folderName, "DingLiang"); break;
        case SAVE_LOAD_TYPE_CALIBRATION: strcpy(folderName, "Calibration"); break;
        default: return 0xFEU;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          folderName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        return 0xFEU;
    }
    result = f_opendir(&dir_handle, (const TCHAR *)folderPath);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return (uint8_t)result;
    }

    for (;;)
    {
        result = f_readdir(&dir_handle, &fileInfo);
        if (result != FR_OK || fileInfo.fname[0] == '\0')
        {
            break;
        }
        if ((fileInfo.fattrib & AM_DIR) != 0U || fileInfo.fname[0] != '_')
        {
            continue;
        }

        firstSeparator = fileInfo.fname;
        secondSeparator = strchr(firstSeparator + 1, '_');
        if ((secondSeparator == NULL) || (secondSeparator == firstSeparator + 1))
        {
            continue;
        }
        for (digit = firstSeparator + 1; digit < secondSeparator; ++digit)
        {
            if ((*digit < '0') || (*digit > '9'))
            {
                break;
            }
        }
        if ((digit != secondSeparator) ||
            (strcmp(secondSeparator + 1, old_filename) != 0))
        {
            continue;
        }

        ++matchCount;
        if (matchCount > 1U)
        {
            break;
        }
        pathLength = snprintf(oldPath, sizeof(oldPath), "%s/%s", folderPath,
                              fileInfo.fname);
        if ((pathLength < 0) || ((size_t)pathLength >= sizeof(oldPath)))
        {
            result = FR_INVALID_NAME;
            break;
        }
        prefixLength = (size_t)(secondSeparator - fileInfo.fname) + 1U;
        pathLength = snprintf(oldBaseName, sizeof(oldBaseName), "%.*s%s",
                              (int)prefixLength, fileInfo.fname, new_filename);
        if ((pathLength < 0) || ((size_t)pathLength >= sizeof(oldBaseName)))
        {
            result = FR_INVALID_NAME;
            break;
        }
    }

    closeResult = f_closedir(&dir_handle);
    if (result == FR_OK && closeResult != FR_OK)
    {
        result = closeResult;
    }
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        return (uint8_t)result;
    }
    if (matchCount == 0U)
    {
        USERFatFSResult = FR_NO_FILE;
        return (uint8_t)FR_NO_FILE;
    }
    if (matchCount > 1U)
    {
        USERFatFSResult = FR_INVALID_NAME;
        printf("Cannot rename a file because its name is not unique.\r\n");
        return (uint8_t)FR_INVALID_NAME;
    }

    pathLength = snprintf(newPath, sizeof(newPath), "%s/%s", folderPath,
                          oldBaseName);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(newPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        return 0xFEU;
    }
    result = f_rename((const TCHAR *)oldPath, (const TCHAR *)newPath);
    USERFatFSResult = result;
    return (result == FR_OK) ? 0U : (uint8_t)result;
}


/**
 * @brief 在日志输出中列出指定目录下的普通文件。
 * @param dir_name 用户卷根目录下的目录名，不包含路径分隔符。
 * @return 无；目录打开或读取失败时记录错误日志。
 * @note 使用 FatFs R0.12c 的 f_opendir、f_readdir 和 f_closedir 接口。
 */
void SysShowAllFileNames(char *dir_name)
{
    FRESULT result;
    FILINFO fileInfo;
    char folderPath[32];
    int pathLength;

    if ((dir_name == NULL) || (dir_name[0] == '\0') ||
        (strchr(dir_name, '/') != NULL) || (strchr(dir_name, '\\') != NULL))
    {
        printf("Invalid FatFs directory name.\r\n");
        return;
    }

    pathLength = snprintf(folderPath, sizeof(folderPath), "%s/%s", USERPath,
                          dir_name);
    if ((pathLength < 0) || ((size_t)pathLength >= sizeof(folderPath)))
    {
        USERFatFSResult = FR_INVALID_NAME;
        printf("FatFs directory path is too long.\r\n");
        return;
    }

    result = f_opendir(&dir_handle, (const TCHAR *)folderPath);
    if (result != FR_OK)
    {
        USERFatFSResult = result;
        printf("Failed to open FatFs directory.\r\n");
        return;
    }

    printf("\r\n=== Files in directory: %s ===\r\n", folderPath);
    for (;;)
    {
        result = f_readdir(&dir_handle, &fileInfo);
        if (result != FR_OK || fileInfo.fname[0] == '\0')
        {
            break;
        }
        if ((fileInfo.fattrib & AM_DIR) == 0U)
        {
            printf("  %s\r\n", fileInfo.fname);
        }
    }

    if (f_closedir(&dir_handle) != FR_OK && result == FR_OK)
    {
        result = FR_DISK_ERR;
    }
    USERFatFSResult = result;
    if (result != FR_OK)
    {
        printf("Failed while reading FatFs directory.\r\n");
        return;
    }
    printf("=== End of directory listing ===\r\n");
}


