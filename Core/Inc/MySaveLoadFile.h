//MySaveLoadFile.h
#ifndef __MY_SAVE_LOAD_FILE_H
#define __MY_SAVE_LOAD_FILE_H


#include <stdint.h>
#include "system_runtime.h"
#include "ColorWm_8inch.h"



#define SAVE_LOAD_TYPE_GUANG_PU      0
#define SAVE_LOAD_TYPE_DING_LIANG    1
#define SAVE_LOAD_TYPE_CALIBRATION   2
#define SAVE_LOAD_TYPE_DONG_LI_XUE   3

#define MAX_FILE_SAVE_NUM			14
#define MAX_FILE_NAME_LEN			20


typedef struct {
		uint8_t gain;//gain
		uint8_t pmt_on_off;
		uint16_t pmt_voltage;
	
		uint8_t emit_light;
		uint8_t sensor;
}_LIGHT_POWER_SCONFIG;

typedef struct {

	uint16_t  InitFlag;
	int32_t  FactoryWaveCorrect;//出厂波长修正
	int32_t  UserWaveCorrect;//用户波长修正
	uint8_t  language;
	char   SN[16];
	_LIGHT_POWER_SCONFIG LightPowerS;
	_GuPuCeShiStruct GuPuPar;
	// _DongLiXueCeShiStruct DongLiXuePar; 	
	// _SnrCeShiStruct snrPar; 	

	uint32_t Checksum;

}_SYS_PARA_STRUCT;

extern __align(4) 	_SYS_PARA_STRUCT   SysPara;


typedef struct ScanFileNameList{
	uint8_t	select;
	char	name[MAX_FILE_NAME_LEN+1];
}_ScanFileNameList;

extern _ScanFileNameList FileNameList[MAX_FILE_SAVE_NUM] ;



void UpdateSysParameter(void);

void ParameterCheck(void);
void ParameterInit(void);





uint8_t SysDeleteFileByTypeAndName(uint8_t type, uint8_t delete_pos,char *Filename);
uint8_t SysLoadFileByTypeAndName(uint8_t type, uint8_t load_pos, char *Filename,  uint32_t *read_len);
uint8_t SysLoadAllFileNames(uint8_t type);
uint8_t SysSaveFiles(uint8_t type, uint8_t save_pos, char *FileName, uint8_t *data, uint32_t data_len);
uint8_t SysRenameFileByTypeAndName(uint8_t type, char *old_filename, char *new_filename);

void FileInit(void);
void SysShowAllFileNames(char* dir_name);


#endif
