//
#include "MyLib.h"
#include "system_runtime.h"
/*
****************************************************************************************************
* 描述: ASCII转化成hex数据
* 函数原型:  uint16 Transform_ASCIItoHEX(uint8 *pASCII,uint8 len)
* 入口参数:    
* 出口参数:
* 附加信息:
****************************************************************************************************
*/






uint8_t Bcd2Hex(uint8_t bcd)
{
	uint8_t ret = 0;
	ret = (bcd>>4)*10 + (bcd & 0x0f);
	return ret;

}

uint8_t Hex2Bcd(uint8_t hex)
{
	uint8_t ret = 0;
	ret = ((hex/10)<<4) + (hex%10);
	return ret;

}



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



















