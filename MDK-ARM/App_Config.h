#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * 代码更新说明（示例）
 * 变更版本：1.0.0
 * 变更标题：从ucos移植到FReeRtos，从stm32f103ze升级到stm32f407ze
 * 变更编号/关联问题：无
 * 作者：待填写
 * 日期：2026-10-12
 *
 * 一、变更背景与目的
 * 更新MCU为stm32f407ze，并用stm32cubemx 生成工程，同时移植到 FreeRTOS。并用最新的 FatFs 接口处理文件操作。
 *
 * 二、变更内容
 * 涉及文件/模块：
 * - 全部
 * - Core/Inc/wavelength_transfor.h
 * - WindowsManage/ColorWM_8inch.c
 *
 * 修改说明：
 * - 更新工程配置以适配 STM32F407 和 FreeRTOS。
 *
 */

// 固件版本与日期
#define FIRM_WARE_VERSION       "1.0.0"
#define FIRM_WARE_DATE          "2026-10-12"

// PCB版本
#define PCB_VERSION             "1.0.0"

// 通信协议版本
#define PROTOCAL_VERSION		104u


#define  DEVICE_TYPE_LI_930			1    //930		
#define  DEVICE_TYPE_LI_950			2    //950		

// 定义器件类型
#define  DEVICE_TYPE				DEVICE_TYPE_LI_930 //定义器件类型



// APP其他配置


























#endif /* APP_CONFIG_H */





