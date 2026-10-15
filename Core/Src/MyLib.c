//
#include "MyLib.h"
#include "system_runtime.h"
#include "stm32f4xx_hal.h"

/**
 * @brief 使用 Cortex-M4 的 DWT 周期计数器实现微秒级忙等待。
 * @param n 需要延时的微秒数；小于或等于零时立即返回。
 * @note 延时时长依据当前 SystemCoreClock 计算，调用期间会占用 CPU。
 */
void _delay_us(int32_t n)
{
	uint64_t remainingCycles;
	uint32_t cycleCount;
	uint32_t startCycle;

	if (n <= 0)
	{
		return;
	}

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	remainingCycles = (((uint64_t)SystemCoreClock * (uint32_t)n) + 999999ULL) / 1000000ULL;
	while (remainingCycles > 0U)
	{
		cycleCount = (remainingCycles > 0x7FFFFFFFULL) ? 0x7FFFFFFFUL : (uint32_t)remainingCycles;
		startCycle = DWT->CYCCNT;
		while ((uint32_t)(DWT->CYCCNT - startCycle) < cycleCount)
		{
		}
		remainingCycles -= cycleCount;
	}
}

/**
 * @brief 将压缩 BCD 编码转换为十进制数值。
 * @param bcd 待转换的压缩 BCD 字节，高、低半字节分别表示十位和个位。
 * @return 转换后的十进制数值。
 */
uint8_t Bcd2Hex(uint8_t bcd)
{
	uint8_t ret = 0;
	ret = (bcd>>4)*10 + (bcd & 0x0f);
	return ret;

}

/**
 * @brief 将十进制数值转换为压缩 BCD 编码。
 * @param hex 待转换的十进制数值，要求输入值在 0 至 99 范围内。
 * @return 转换后的压缩 BCD 字节。
 */
uint8_t Hex2Bcd(uint8_t hex)
{
	uint8_t ret = 0;
	ret = ((hex/10)<<4) + (hex%10);
	return ret;

}



/**
 * @brief 将带可选小数点和负号的 ASCII 数字转换为按类型缩放的整数。
 * @param pASCII 以空字符结尾的数字字符串，可包含一个小数点及可选的前导负号。
 * @param type 数据类型，用于确定小数位数及无小数点时的缩放倍数。
 * @return 转换并按数据类型缩放后的有符号整数。
 */
int32_t EnterData_ASCIItoHEX(uint8_t *pASCII,int8_t type)
{
	int32_t temp = 0;
	uint8_t PointFlag = 0;
	uint8_t flag = 0;

	if(*pASCII == '-')
	{
		flag = 1;
		pASCII++;	
	}

	while (*pASCII != '\0')
	{
		if(*pASCII != '.')
		{
			temp = temp * 10 + (*pASCII++ & 0x0f);
		}
		else
		{
			int8_t i = 0;
			pASCII++;
			PointFlag = 1;
			for(i=0;i<type;i++)
			{
				if(*pASCII == '\0')
				{
					break;	
				}
				else
				{
					temp = temp * 10 + (*pASCII & 0x0f);
				}
				pASCII++;
			}
			while(type-i)
			{
				temp = temp * 10;
				i++;
			}
			break;
		}
	}
	if(PointFlag == 0)
	{
		if(type == DATA_TYPE_E)
		{temp = temp*1000;}
		else if (type == DATA_TYPE_A)
		{temp = temp*10000;}
		else if (type == DATA_TYPE_T)
		{temp = temp*1000;}
		else if (type == DATA_TYPE_WAVELEN)
		{temp = temp*10;}
		else if (type == DATA_TYPE_TIME)
		{temp = temp;}
		else
		{temp = temp;}

		
	}
	if(flag == 1)
	{
		temp = 0- temp;
	}
	return temp;   
}


/**
 * @brief 将数组按升序排列，并依据首项的有效范围移除最多五个前置异常值。
 * @param p 待处理的无符号整数数组；数组长度及 num 均应至少为五。
 * @param num 数组中参与升序排序的元素个数。
 * @return 无返回值，排序及异常值移除结果直接写回数组。
 */
void SortMinToMax(uint32_t* p,uint16_t num)
{
	uint16_t i = 0;
	uint16_t j=0;	
	for(i=0;i<num-1;i++)
	{
		for(j=0;j<num-1-i;j++)
		{
			if(p[j]>p[j+1])
			{
				p[j] = p[j]+p[j+1];
				p[j+1] = p[j]-p[j+1];
				p[j] = p[j]-p[j+1];
			}
		}
	}

	for(i=0;i<5;i++)
	{
		if(p[0]<190 || p[0]>11000)
		{
			p[0] = p[1];		
			p[1] = p[2];		
			p[2] = p[3];		
			p[3] = p[4];		
			p[4] = 0;		
		}
		else
		{
			break;
		}
	}
}

/**
 * @brief 对两路输入数据分别计算滑动平均；窗口满后剔除最大值和最小值再求平均。
 * @param dat1 第一通道的当前采样值。
 * @param dat2 第二通道的当前采样值。
 * @param ret1 用于接收第一通道平均值的输出指针。
 * @param ret2 用于接收第二通道平均值的输出指针。
 * @return 无返回值，计算结果写入 ret1 和 ret2 指向的位置。
 */
void GetEverageDataT(uint32_t dat1,uint32_t dat2,uint32_t* ret1,uint32_t* ret2)
{
	static uint8_t num = 0;
	static uint32_t max_f1=0;
	static uint32_t min_f1=0x7fffff;
	static uint32_t max_f2=0;
	static uint32_t min_f2=0x7fffff;
	static uint32_t f1_buf[17] ;
	static uint32_t f2_buf[17] ;
	
	
	
	uint8_t i = 0;
	uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;
	
	
	if(num<16)
	{
		f1_buf[num] = dat1;
		f2_buf[num] = dat2;
		num++;
		for(i=0;i<num;i++)
		{
			tmp1 += f1_buf[i];
			tmp2 += f2_buf[i];
		}
		*ret1 = tmp1/num;
		*ret2 = tmp2/num;
	}
	else
	{
		for(i=0;i<15;i++)
		{
			f1_buf[i] = f1_buf[i+1];
			f2_buf[i] = f2_buf[i+1];
		}
		f1_buf[15] = dat1;
		f2_buf[15] = dat2;

		max_f1 = f1_buf[0];
		min_f1 = f1_buf[0];
		max_f2 = f2_buf[0];
		min_f2 = f2_buf[0];
		for(i=0;i<16;i++)
		{
			tmp1 += f1_buf[i];
			tmp2 += f2_buf[i];
			
			if(max_f1<f1_buf[i])
			{
				max_f1=f1_buf[i];
			}
			if(min_f1>f1_buf[i])
			{
				min_f1=f1_buf[i];
			}

			if(max_f2<f2_buf[i])
			{
				max_f2=f2_buf[i];
			}
			if(min_f2>f2_buf[i])
			{
				min_f2=f2_buf[i];
			}
			
			
		}
		f1_buf[16] = (tmp1-max_f1-min_f1)/14;
		f2_buf[16] = (tmp2-max_f2-min_f2)/14;
		*ret1 = f1_buf[16];
		*ret2 = f2_buf[16];
	}
}



/**
 * @brief 查找数组中的最大值和最小值。
 * @param p 待查找的无符号整数数组，num 必须大于零。
 * @param num 数组元素个数。
 * @param retMax 用于接收最大值的输出指针。
 * @param retMin 用于接收最小值的输出指针。
 * @return 无返回值，最大值和最小值分别写入 retMax 和 retMin 指向的位置。
 */
void FindMaxMin(uint32_t* p,uint8_t num,uint32_t* retMax,uint32_t* retMin)
{
	uint32_t max,min;
	uint8_t i=0;	
	max = p[0];
	min = p[0];

	for(i=0;i<num;i++)
	{
			if(max<p[i])
				max = p[i];
			if(min>p[i])
				min = p[i];
	}
	*retMax = max;
	*retMin = min;
	
}






/**
 * @brief 根据阈值变化检测数据序列中的发射峰和吸收谷。
 * @param data 待检测的双精度数据序列，至少包含一个元素。
 * @param data_count 数据序列的元素个数。
 * @param emi_peaks 用于保存发射峰位置的数组。
 * @param num_emi_peaks 用于接收实际检测到的发射峰数量。
 * @param max_emi_peaks 发射峰数组可容纳的最大数量。
 * @param absop_peaks 用于保存吸收谷位置的数组。
 * @param num_absop_peaks 用于接收实际检测到的吸收谷数量。
 * @param max_absop_peaks 吸收谷数组可容纳的最大数量。
 * @param delta 判定峰或谷所需的最小变化阈值。
 * @param emi_first 非零时先检测发射峰，零时先检测吸收谷。
 * @return 检测成功返回 0；发射峰数组空间不足返回 1；吸收谷数组空间不足返回 2。
 */
int detect_peak(
        const double*   data, /* the data */ 
        int             data_count, /* row count of data */ 
        int*            emi_peaks, /* emission peaks will be put here */ 
        int*            num_emi_peaks, /* number of emission peaks found */
        int             max_emi_peaks, /* maximum number of emission peaks */ 
        int*            absop_peaks, /* absorption peaks will be put here */ 
        int*            num_absop_peaks, /* number of absorption peaks found */
        int             max_absop_peaks, /* maximum number of absorption peaks                                            */ 
        double          delta, /* delta used for distinguishing peaks */
        int             emi_first /* should we search emission peak first of
                                     absorption peak first? */
        )
{

    int     i;
    double  mx;
    double  mn;
    int     mx_pos = 0;
    int     mn_pos = 0;
    int     is_detecting_emi = emi_first;
    mx = data[0];
    mn = data[0];
    *num_emi_peaks = 0;
    *num_absop_peaks = 0;

    for(i = 1; i < data_count; ++i)
    {
        if(data[i] > mx)
        {
            mx_pos = i;
            mx = data[i];
        }
        if(data[i] < mn)
        {
            mn_pos = i;
            mn = data[i];
        }


        if(is_detecting_emi &&  data[i] < mx - delta)
        {
            if(*num_emi_peaks >= max_emi_peaks) /* not enough spaces */
                return 1;

            emi_peaks[*num_emi_peaks] = mx_pos;
            ++ (*num_emi_peaks);

            is_detecting_emi = 0;

            i = mx_pos - 1;
            mn = data[mx_pos];
            mn_pos = mx_pos;

        }

        else if((!is_detecting_emi) && data[i] > mn + delta)
        {

            if(*num_absop_peaks >= max_absop_peaks)
                return 2;

            absop_peaks[*num_absop_peaks] = mn_pos;
            ++ (*num_absop_peaks);
            is_detecting_emi = 1;
            i = mn_pos - 1;
            mx = data[mn_pos];
            mx_pos = data[mn_pos];
        }
    }
    return 0;

}



















