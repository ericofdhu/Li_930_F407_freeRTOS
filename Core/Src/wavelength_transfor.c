#include "wavelength_transfor.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
//#include <math.h>

#define WL_TABLE_FILE_NAME "wl.txt"


// const WL_TRANS_TABLE default_wl_table[10] = 
// {
//   {0,0},                      //1   0.0nm
//   {2537,4685*8*39/504},                //2   253.7nm
//   {3650,6820*8*39/504},                //3   365.0nm
//   {4046,7555*8*39/504},                //4   404.6nm
//   {4358,8145*8*39/504},                //5   435.8nm
//   {5461,10310*8*39/504},                //6   546.1nm
//   {3650*2,6820*2*8*39/504},            //7   365.0nm*2
// //   {3650*3,6820*3*8*39/504},             //8   ��������
//   {-1,-1},              //9   ����һ��������
//   {-1,-1},              //10   ����һ��������
//   {-1,-1},              //10   ����һ��������
// };



const WL_TRANS_TABLE default_wl_table[10] = 
{
//  {0,0},                      //1   0.0nm
  {2537,2941},                //2   253.7nm
  {3130,3625},                //2   253.7nm
  {3650,4259},                //3   365.0nm
  {4047,4733},                //4   404.6nm
  {4358,5098},                //5   435.8nm
  {5461,6444},                //6   546.1nm
  {-1,-1},              //9   ����һ��������
  {-1,-1},              //10   ����һ��������
  {-1,-1},              //10   ����һ��������
  {-1,-1},              //10   ����һ��������
};


WL_TRANS_TABLE gNew_wl_table[10] = {0};
static WL_TRANS_TABLE wl_table[10] = {0};

static QuadraticFitResult_t gFit_result = {0};

static FRESULT wl_table_get_path(char *path, size_t path_size)
{
    size_t drive_length;
    const size_t file_name_size = sizeof(WL_TABLE_FILE_NAME);

    if (path == NULL)
    {
        return FR_INVALID_PARAMETER;
    }
    if (retUSER != 0U)
    {
        return FR_NOT_READY;
    }

    drive_length = strlen(USERPath);
    if ((drive_length == 0U) ||
        (drive_length + file_name_size > path_size))
    {
        return FR_INVALID_NAME;
    }

    memcpy(path, USERPath, drive_length);
    memcpy(&path[drive_length], WL_TABLE_FILE_NAME, file_name_size);
    return FR_OK;
}

static FRESULT wl_table_write_file(const WL_TRANS_TABLE *table)
{
    FRESULT result;
    FRESULT close_result;
    FIL file;
    UINT bytes_written = 0U;
    char path[sizeof(USERPath) + sizeof(WL_TABLE_FILE_NAME)];

    result = wl_table_get_path(path, sizeof(path));
    if (result != FR_OK)
    {
        return result;
    }

    result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        return result;
    }

    result = f_write(&file, table, (UINT)sizeof(wl_table), &bytes_written);
    if ((result == FR_OK) && (bytes_written != (UINT)sizeof(wl_table)))
    {
        result = FR_DISK_ERR;
    }

    close_result = f_close(&file);
    if (result == FR_OK)
    {
        result = close_result;
    }
    return result;
}

static FRESULT wl_table_read_file(WL_TRANS_TABLE *table)
{
    FRESULT result;
    FRESULT close_result;
    FIL file;
    UINT bytes_read = 0U;
    char path[sizeof(USERPath) + sizeof(WL_TABLE_FILE_NAME)];

    result = wl_table_get_path(path, sizeof(path));
    if (result != FR_OK)
    {
        return result;
    }

    result = f_open(&file, path, FA_READ);
    if (result != FR_OK)
    {
        return result;
    }

    result = f_read(&file, table, (UINT)sizeof(wl_table), &bytes_read);
    if ((result == FR_OK) && (bytes_read != (UINT)sizeof(wl_table)))
    {
        result = FR_INT_ERR;
    }

    close_result = f_close(&file);
    if (result == FR_OK)
    {
        result = close_result;
    }
    return result;
}

int get_wl_table_size(void)
{
    for(int i = 0;i<10;i++)
    {
        if(wl_table[i].wl <= 0)
        {
            return i;
        }
    }
    return 10;
}

FRESULT wl_table_init(void)
{
    FRESULT result;
    WL_TRANS_TABLE loaded_table[10];

    memcpy(wl_table, default_wl_table, sizeof(wl_table));
    result = wl_table_read_file(loaded_table);
    if (result == FR_NO_FILE)
    {
        result = wl_table_write_file(default_wl_table);
    }
    else if (result == FR_OK)
    {
        memcpy(wl_table, loaded_table, sizeof(wl_table));
    }

//    wl_quadratic_fit(wl_table, get_wl_table_size(), &gFit_result);

    for(int i = 0;i<10;i++)
    {
        printf("wl=%d,pos=%d\n",wl_table[i].wl,wl_table[i].pos);
    }

    
    return result;
}


void wl_table_load(WL_TRANS_TABLE* new_wl_table)
{
    memcpy(new_wl_table, default_wl_table, sizeof(default_wl_table));
}



FRESULT wl_table_save(const WL_TRANS_TABLE* new_wl_table)
{
    FRESULT result;
    WL_TRANS_TABLE tmp_wl_table[10] = {0};
    int i;
    int j = 0;

    if (new_wl_table == NULL)
    {
        return FR_INVALID_PARAMETER;
    }

    for (i = 0; i < 10; i++)
    {
        tmp_wl_table[i].pos = -1;
        tmp_wl_table[i].wl = -1;
    }

    for (i = 0; i < 10; i++)
    {
        if (new_wl_table[i].wl > 0)
        {
            tmp_wl_table[j++] = new_wl_table[i];
        }
    }

    result = wl_table_write_file(tmp_wl_table);
    if (result == FR_OK)
    {
        memcpy(wl_table, tmp_wl_table, sizeof(wl_table));
//            wl_quadratic_fit(wl_table, get_wl_table_size(), &gFit_result);
    }
    return result;
}






int WavelengthToPosition( int wl)
{
    int i;
    int wl1, wl2, pos1, pos2;   
    int TABLE_LEN = get_wl_table_size();


/* ���߽���� */
    if (wl <= wl_table[0].wl)
    {
        /* ����С�ڵ�����Сֵ��������Сλ�� */
        // return wl_table[0].pos;

        wl1 = wl_table[0].wl;
        wl2 = wl_table[1].wl;
        pos1 = wl_table[0].pos;
        pos2 = wl_table[1].pos;
        
        /* ���Բ�ֵ��ʽ: pos = pos1 + (wl - wl1) * (pos2 - pos1) / (wl2 - wl1) */        
        return pos1 + (int)((double)(wl - wl1) * (double)(pos2 - pos1) / (double)(wl2 - wl1)+0.5);
    }
    
    if (wl >= wl_table[TABLE_LEN - 1].wl)
    {
        /* �������ڵ������ֵ���������λ�� */
        // return wl_table[TABLE_LEN - 1].pos;

        wl1 = wl_table[TABLE_LEN - 2].wl;
        wl2 = wl_table[TABLE_LEN - 1].wl;
        pos1 = wl_table[TABLE_LEN - 2].pos;
        pos2 = wl_table[TABLE_LEN - 1].pos;
        
        /* ���Բ�ֵ��ʽ: pos = pos1 + (wl - wl1) * (pos2 - pos1) / (wl2 - wl1) */        
        return pos1 + (int)((double)(wl - wl1) * (double)(pos2 - pos1) / (double)(wl2 - wl1)+0.5);
    }
    
    /* ���Ҳ������ڵ����� */
    for (i = 0; i < TABLE_LEN - 1; i++)
    {
        if (wl >= wl_table[i].wl && wl <= wl_table[i + 1].wl)
        {
            /* �ҵ����䣬�������Բ�ֵ */
            wl1 = wl_table[i].wl;
            wl2 = wl_table[i + 1].wl;
            pos1 = wl_table[i].pos;
            pos2 = wl_table[i + 1].pos;
            
            /* ���Բ�ֵ��ʽ: pos = pos1 + (wl - wl1) * (pos2 - pos1) / (wl2 - wl1) */
            if (wl2 == wl1)
            {
                /* ���������󣬷����������λ�� */
                return pos1;
            }
            
            return pos1 + (int)((double)(wl - wl1) * (double)(pos2 - pos1) / (double)(wl2 - wl1)+0.5);
        }
    }
    
    /* δ�ҵ�ƥ�����䣬���ش���ֵ */
    return -1;

}



int PositionToWavelength( int pos)
{
    int i;
    int wl1, wl2, pos1, pos2;   
    int TABLE_LEN = get_wl_table_size();


/* ���߽���� */
    if (pos <= wl_table[0].pos)
    {
        /* λ��С�ڵ�����Сֵ��������С���� */
        wl1 = wl_table[0].wl;
        wl2 = wl_table[1].wl;
        pos1 = wl_table[0].pos;
        pos2 = wl_table[1].pos;     
        
        /* ���Բ�ֵ��ʽ: wl = wl1 + (pos - pos1) * (wl2 - wl1) / (pos2 - pos1) */        
        return wl1 + (int)((double)(pos - pos1) * (double)(wl2 - wl1) / (double)(pos2 - pos1)+0.5);
    }
    
    if (pos >= wl_table[TABLE_LEN - 1].pos)
    {
        /* λ�ó����ڵ������ֵ��������󲨳� */
        wl1 = wl_table[TABLE_LEN - 2].wl;
        wl2 = wl_table[TABLE_LEN - 1].wl;
        pos1 = wl_table[TABLE_LEN - 2].pos;
        pos2 = wl_table[TABLE_LEN - 1].pos;     
        
        /* ���Բ�ֵ��ʽ: wl = wl1 + (pos - pos1) * (wl2 - wl1) / (pos2 - pos1) */        
        return wl1 + (int)((double)(pos - pos1) * (double)(wl2 - wl1) / (double)(pos2 - pos1)+0.5);
    }
    
    /* ����λ�����ڵ����� */
    for (i = 0; i < TABLE_LEN - 1; i++)
    {
        if (pos >= wl_table[i].pos && pos <= wl_table[i + 1].pos)
        {
            /* �ҵ����䣬�������Բ�ֵ */
            wl1 = wl_table[i].wl;
            wl2 = wl_table[i + 1].wl;
            pos1 = wl_table[i].pos;
            pos2 = wl_table[i + 1].pos;     
            
            /* ���Բ�ֵ��ʽ: wl = wl1 + (pos - pos1) * (wl2 - wl1) / (pos2 - pos1) */
            if (pos2 == pos1)
            {
                /* ���������󣬷���������㲨�� */
                return wl1;
            }
            
            return wl1 + (int)((double)(pos - pos1) * (double)(wl2 - wl1) / (double)(pos2 - pos1)+0.5);
        }
    }
    
    /* δ�ҵ�ƥ�����䣬���ش���ֵ */
    return -1;
}   

//==========================================================================================================================================================
//
//�������
//
//==========================================================================================================================================================



// /**
//  * @brief ����������Ϻ�������С���˷���
//  * 
//  * ʹ����С���˷�����������ݵ���ж���������ϣ�
//  * ��Ϻ�����ʽ: y = a*x? + b*x + c
//  * 
//  * @param data      �����������飨WL_TRANS_TABLE���ͣ�
//  * @param count     ���ݵ�������������Ҫ3���㣩
//  * @param result    �����Ͻ����a, b, cϵ����
//  * @return          0��ʾ�ɹ���-1��ʾ��������-2��ʾ���ݲ���
//  */
// int wl_quadratic_fit(const WL_TRANS_TABLE* data, int count, QuadraticFitResult_t* result)
// {
//     // ������Ч�Լ��
//     if (data == NULL || result == NULL) {
//         return -1;
//     }
    
//     // �����������������Ҫ3�����ݵ�
//     if (count < 3) {
//         return -2;
//     }
    
//     float sum_x = 0.0f, sum_y = 0.0f;
//     float sum_x2 = 0.0f, sum_xy = 0.0f;
//     float sum_x3 = 0.0f, sum_x2y = 0.0f;
//     float sum_x4 = 0.0f;
    
//     // ��������ۼӺ�
//     for (int i = 0; i < count; i++) {
//         float x = (float)data[i].wl;
//         float y = (float)data[i].pos;
        
//         sum_x += x;
//         sum_y += y;
//         sum_x2 += x * x;
//         sum_xy += x * y;
//         sum_x3 += x * x * x;
//         sum_x2y += x * x * y;
//         sum_x4 += x * x * x * x;
//     }
    
//     // �������淽�����ϵ������
//     // ��������ʽ:
//     // [n    sum_x  sum_x2] [c]   [sum_y  ]
//     // [sum_x sum_x2 sum_x3] [b] = [sum_xy ]
//     // [sum_x2 sum_x3 sum_x4] [a]   [sum_x2y]
    
//     float n = (float)count;
    
//     // ��������ʽ D
//     float D = n * (sum_x2 * sum_x4 - sum_x3 * sum_x3)
//             - sum_x * (sum_x * sum_x4 - sum_x3 * sum_x2)
//             + sum_x2 * (sum_x * sum_x3 - sum_x2 * sum_x2);
    
//     // �������ʽ�Ƿ�ӽ��㣨��ֹ�������
//     if (fabs(D) < 1e-10) {
//         return -3;  // �������죬�޷����
//     }
    
//     // �����δ֪��������ʽ
//     float D_c = sum_y * (sum_x2 * sum_x4 - sum_x3 * sum_x3)
//               - sum_xy * (sum_x * sum_x4 - sum_x3 * sum_x2)
//               + sum_x2y * (sum_x * sum_x3 - sum_x2 * sum_x2);
    
//     float D_b = n * (sum_xy * sum_x4 - sum_x3 * sum_x2y)
//               - sum_x * (sum_y * sum_x4 - sum_x2 * sum_x2y)
//               + sum_x2 * (sum_y * sum_x3 - sum_xy * sum_x2);
    
//     float D_a = n * (sum_x2 * sum_x2y - sum_x3 * sum_xy)
//               - sum_x * (sum_x * sum_x2y - sum_x3 * sum_y)
//               + sum_x2 * (sum_x * sum_xy - sum_x2 * sum_y);
    
//     // ���ϵ��
//     result->c = D_c / D;
//     result->b = D_b / D;
//     result->a = D_a / D;
    
//     return 0;  // �ɹ�
// }


// /**
//  * @brief ʹ����Ͻ�������ֵ
//  * 
//  * ������ϵõ��Ķ�������ϵ��������ָ��������Ӧ��ת��ֵ
//  * 
//  * @param result    ��Ͻ��������a, b, cϵ����
//  * @param wavelength ���벨��ֵ
//  * @return          ����õ���ת��ֵ
//  */
// // float wl_quadratic_interpolate(const QuadraticFitResult_t* result, int wavelength)
// int WavelengthToPosition( int wl)
// {

//     QuadraticFitResult_t* result = &gFit_result;
    
//     float x = (float)(wl);
//     return (int)(result->a * x * x + result->b * x + result->c);
// }

/**
 * @brief ����������ϲ������ֵ��һվʽ�ӿڣ�
 * 
 * @param data       ������������
 * @param count      ���ݵ�����
 * @param wavelength Ҫ��ֵ�Ĳ���
 * @param output     �����ֵ���
 * @return          0��ʾ�ɹ���������ʾ����
 */
// int wl_quadratic_fit_and_interpolate(const WL_TRANS_TABLE* data, 
//                                       int count, 
//                                       int wavelength, 
//                                       float* output)
// {
//     if (output == NULL) {
//         return -1;
//     }
    
//     QuadraticFitResult_t fit_result;
//     int ret = wl_quadratic_fit(data, count, &fit_result);
    
//     if (ret == 0) {
//         *output = wl_quadratic_interpolate(&fit_result, wavelength);
//     }
    
//     return ret;
// }
