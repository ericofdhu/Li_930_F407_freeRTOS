#include "CircularBuffer.h"






int32_t  wave_Buffer[WAVE_BUFF_LEN];
CircularBuffer cb_wave;



int32_t  time_Buffer[TIME_BUFF_LEN];
CircularBuffer cb_time;




int32_t  normal_Buffer[NORMAL_BUFF_LEN];
CircularBuffer cb_normal;




/*
*********************************************************************************************************
*                                             CircularBuffer_Init()
*
* Description : 初始化环形缓冲区
*
* Argument(s) : buffer - 缓冲区结构体指针
*               data_buffer - 数据存储数组
*               size - 缓冲区大小
*
* Return(s)   : 无
*
* Caller(s)   : 需要初始化环形缓冲区的函数
*
* Note(s)     : 
*               - 缓冲区大小必须大于0
*               - 数据存储数组必须足够大（size个元素）
*               - 初始化后缓冲区为空
*********************************************************************************************************
*/


void CircularBuffer_Init(CircularBuffer *buffer, int32_t *data_buffer, int size)
{
    buffer->buffer = data_buffer;
    buffer->size = size;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    buffer->sum = 0;
    
    /* 可选：清空缓冲区 */
    int i;
    for (i = 0; i < size; i++) {
        data_buffer[i] = 0;
    }
}
/*
*********************************************************************************************************
*                                             CircularBuffer_Push()
*
* Description : 向环形缓冲区添加新数据
*
* Argument(s) : buffer - 缓冲区结构体指针
*               value - 要添加的数据值
*
* Return(s)   : 无
*
* Caller(s)   : 需要向缓冲区添加数据的函数
*
* Note(s)     : 
*               - 如果缓冲区已满，会覆盖最旧的数据
*               - 自动更新总和，便于快速计算平均值
*********************************************************************************************************
*/
void CircularBuffer_Push(CircularBuffer *buffer, float value)
{
    if (buffer->count == buffer->size) {
        /* 缓冲区已满，移除最旧的数据 */
        buffer->sum -= buffer->buffer[buffer->tail];
        buffer->tail = (buffer->tail + 1) % buffer->size;
        buffer->count--;
    }
    
    /* 添加新数据 */
    buffer->buffer[buffer->head] = value;
    buffer->head = (buffer->head + 1) % buffer->size;
    buffer->count++;
    buffer->sum += value;
}
/*
*********************************************************************************************************
*                                             CircularBuffer_GetAverage()
*
* Description : 计算环形缓冲区中数据的平均值
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 平均值（整数，四舍五入）
*
* Caller(s)   : 需要获取平均值的函数
*
* Note(s)     : 
*               - 如果缓冲区为空，返回0
*               - 使用预计算的总和，计算效率高
*               - 结果四舍五入为整数
*********************************************************************************************************
*/
int CircularBuffer_GetAverage(CircularBuffer *buffer)
{
    if (buffer->count == 0) {
        return 0;
    }
    
    /* 计算平均值并四舍五入 */
    return (int)((buffer->sum + buffer->count / 2) / buffer->count);
}
/*
*********************************************************************************************************
*                                             CircularBuffer_GetAverageFloat()
*
* Description : 计算环形缓冲区中数据的平均值（浮点数）
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 平均值（浮点数）
*
* Caller(s)   : 需要高精度平均值的函数
*
* Note(s)     : 
*               - 如果缓冲区为空，返回0.0
*               - 提供更高的精度
*********************************************************************************************************
*/
float CircularBuffer_GetAverageFloat(CircularBuffer *buffer)
{
    if (buffer->count == 0) {
        return 0.0f;
    }
    
    return (float)buffer->sum / (float)buffer->count;
}
/*
*********************************************************************************************************
*                                             CircularBuffer_Clear()
*
* Description : 清空环形缓冲区
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 无
*
* Caller(s)   : 需要重置缓冲区的函数
*
* Note(s)     : 
*               - 清空所有数据，重置为初始状态
*               - 总和归零，计数归零
*********************************************************************************************************
*/
void CircularBuffer_Clear(CircularBuffer *buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    buffer->sum = 0;
}
/*
*********************************************************************************************************
*                                             CircularBuffer_GetCount()
*
* Description : 获取环形缓冲区中当前元素个数
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 元素个数
*
* Caller(s)   : 需要知道缓冲区大小的函数
*
* Note(s)     : 
*               - 返回当前缓冲区中的有效数据个数
*********************************************************************************************************
*/
int CircularBuffer_GetCount(CircularBuffer *buffer)
{
    return buffer->count;
}
/*
*********************************************************************************************************
*                                             CircularBuffer_IsFull()
*
* Description : 检查环形缓冲区是否已满
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 1-已满，0-未满
*
* Caller(s)   : 需要检查缓冲区状态的函数
*
* Note(s)     : 
*               - 缓冲区满时，新数据会覆盖旧数据
*********************************************************************************************************
*/
int CircularBuffer_IsFull(CircularBuffer *buffer)
{
    return buffer->count == buffer->size;
}
/*
*********************************************************************************************************
*                                             CircularBuffer_IsEmpty()
*
* Description : 检查环形缓冲区是否为空
*
* Argument(s) : buffer - 缓冲区结构体指针
*
* Return(s)   : 1-为空，0-不为空
*
* Caller(s)   : 需要检查缓冲区状态的函数
*
* Note(s)     : 
*               - 缓冲区空时，无法获取有效平均值
*********************************************************************************************************
*/
int CircularBuffer_IsEmpty(CircularBuffer *buffer)
{
    return buffer->count == 0;
}











//低通滤波二阶巴特沃斯，截止频率=10Hz（假设光谱扫描频率约 1~5 Hz，保留信号，滤除 >10 Hz 噪声）


const float LPF_100Hz_5Hz[8] = {
    0.0201f, 0.0402f, 0.0201f,   // b0, b1, b2
    1.0f,   -1.5610f, 0.6414f,   // a0, a1, a2
    1.0f,    1.0f                 // gain0, gain1
};
const float LPF_100Hz_10Hz[8] = {
    0.0675f, 0.1349f, 0.0675f,   // b0, b1, b2
    1.0f,   -1.1430f, 0.4128f,   // a0, a1, a2
    1.0f,    1.0f                 // gain0, gain1
};
//低通滤波二阶巴特沃斯，截止频率=30Hz
const float LPF_100Hz_30Hz[8] = {
    0.1936f, 0.3872f, 0.1936f,   // b0, b1, b2
    1.0f,   -0.4304f, 0.2046f,   // a0, a1, a2
    1.0f,    1.0f                 // gain0, gain1
};
//低通滤波二阶巴特沃斯，截止频率=50Hz
const float LPF_1000Hz[8] = {
    0.0201f, 0.0402f, 0.0201f,   // b0, b1, b2
    1.0f,   -1.5610f, 0.6414f,    // a0, a1, a2
    1.0f,    1.0f                  // gain0, gain1
};
//采样率 1000 Hz，截止频率 10 Hz
const float LPF_1000Hz_10Hz[8] = {
    0.0009448f, 0.0018896f, 0.0009448f,   // b0, b1, b2
    1.0f,       -1.9112f,    0.9149f,      // a0, a1, a2
    1.0f,        1.0f                      // gain0, gain1
};
//采样率 1000 Hz，截止频率 30 Hz
const float LPF_1000Hz_30Hz[8] = {
    0.0078f, 0.0156f, 0.0078f,   // b0, b1, b2
    1.0f,   -1.7347f, 0.7658f,    // a0, a1, a2
    1.0f,    1.0f                  // gain0, gain1
};
//采样率 10000 Hz，截止频率 20 Hz
const float LPF_10000Hz_20Hz[8] = {
    0.00003913f, 0.00007826f, 0.00003913f,   // b0, b1, b2
    1.0f,        -1.9823f,     0.9824f,       // a0, a1, a2
    1.0f,         1.0f                        // gain0, gain1
};
//一阶低通滤波器（RC 型）
const float LPF_1st_1000Hz_10Hz[8] = {
    0.0628f, 0.0000f, 0.0000f,   // b0, b1, b2
    1.0000f, 0.9372f, 0.0000f,   // a0, a1, a2
    1.0000f, 1.0000f              // gain0, gain1
};
//二阶贝塞尔低通滤波器（Bessel）
const float LPF_Bessel_1000Hz_10Hz[8] = {
    0.0065f, 0.0130f, 0.0065f,   // b0, b1, b2
    1.0000f, -1.6548f, 0.6808f,   // a0, a1, a2
    1.0000f, 1.0000f              // gain0, gain1
};
//高通滤波二阶巴特沃斯，截止频率=0.5Hz（抑制低于 0.5 Hz 的漂移，保留快速变化的谱峰）
const float HPF_Spectrum_Coeff[8] = {
    0.9878f, -1.9756f, 0.9878f,  // b0, b1, b2
    1.0f, -1.9556f, 0.9565f,     // a0, a1, a2
    1.0f, 1.0f                    // gain0, gain1
};
const float HPF_1000Hz[8] = {
    0.9978f, -1.9956f, 0.9978f,   // b0, b1, b2
    1.0f,    -1.9956f, 0.9956f,   // a0, a1, a2
    1.0f,     1.0f                 // gain0, gain1
};
//带阻滤波 抑制 50 Hz 或 60 Hz 工频干扰，二阶巴特沃斯带阻，中心频率 50 Hz，品质因数 Q=10（带宽约 5 Hz）
const float BSF_50Hz_Coeff[8] = {
    0.9605f, -1.8095f, 0.9605f,   // b0, b1, b2
    1.0f, -1.8095f, 0.9210f,      // a0, a1, a2
    1.0f, 1.0f                     // gain0, gain1
};
const float BSF_1000Hz[8] = {
    0.9848f, -1.8732f, 0.9848f,   // b0, b1, b2
    1.0f,    -1.8732f, 0.9696f,   // a0, a1, a2
    1.0f,     1.0f                 // gain0, gain1
};






//巴特沃兹算法
IIR_BUTTERWORTH_DEF DATA_IIR_LPF_Par,DATA_IIR_HPF_Par,DATA_IIR_BSF_Par;

void IIR_Par_Init(unsigned char scan_step)
{

	if(scan_step < 5)
	{
		IIR_Butterworth_Coefficient_Init(LPF_100Hz_5Hz,&DATA_IIR_LPF_Par);	
		IIR_Butterworth_Coefficient_Init(HPF_1000Hz,&DATA_IIR_HPF_Par);	
		IIR_Butterworth_Coefficient_Init(BSF_1000Hz,&DATA_IIR_BSF_Par);	
	}
	else
	{
		IIR_Butterworth_Coefficient_Init(LPF_100Hz_5Hz,&DATA_IIR_LPF_Par);	
		IIR_Butterworth_Coefficient_Init(HPF_Spectrum_Coeff,&DATA_IIR_HPF_Par);	
		IIR_Butterworth_Coefficient_Init(BSF_50Hz_Coeff,&DATA_IIR_BSF_Par);	
	}
//		alpha = 0.25;
//		_lpf_prev = 0;
}
/*
低通 → 高通 → 带阻（避免先高通放大低频噪声后陷波效果变差）
float out1, out2, out3;
IIR_Butterworth(raw_signal, &out1, &lpf);
IIR_Butterworth(out1, &out2, &hpf);
IIR_Butterworth(out2, &out3, &bsf);
若扫描速度更快（如 100 Hz 采样率扫描 20 Hz 信号），将低通截止频率提高到 20~30 Hz。
*/
void IIR_Butterworth_Coefficient_Init(const float temp[8],IIR_BUTTERWORTH_DEF* iir_butterworth_temp)
{
  iir_butterworth_temp->b0 = temp[0];
  iir_butterworth_temp->b1 = temp[1];
  iir_butterworth_temp->b2 = temp[2];
  iir_butterworth_temp->a0 = temp[3];
  iir_butterworth_temp->a1 = temp[4];
  iir_butterworth_temp->a2 = temp[5];
  iir_butterworth_temp->gain0 = temp[6];
  iir_butterworth_temp->gain1 = temp[7];
  iir_butterworth_temp->states0 = 0.0f;
  iir_butterworth_temp->states1 = 0.0f;
}
/*
核心算法
temp = gain0 * in - a1 * states0 - a2 * states1;
out = (b0 * temp + b1 * states0 + b2 * states1) * gain1;
states1 = states0;
states0 = temp;
*/
void IIR_Butterworth(float in,float* out,IIR_BUTTERWORTH_DEF* iir_butterworth_temp)
{
  float temp;
  temp =(iir_butterworth_temp->gain0 * in
         -iir_butterworth_temp->a1 * iir_butterworth_temp->states0)
    -iir_butterworth_temp->a2 * iir_butterworth_temp->states1;
  *out = ((iir_butterworth_temp->b0 * temp + iir_butterworth_temp->b1 * iir_butterworth_temp->states0) +
          iir_butterworth_temp->b2 * iir_butterworth_temp->states1)*iir_butterworth_temp->gain1;
  
  iir_butterworth_temp->states1 = iir_butterworth_temp->states0;
  iir_butterworth_temp->states0 = temp;
}


void IIR_Butterworth_LPF(float in,float* out)
{
  float temp;
	IIR_BUTTERWORTH_DEF* iir_butterworth_temp = &DATA_IIR_LPF_Par;
	
  temp =(iir_butterworth_temp->gain0 * in
         -iir_butterworth_temp->a1 * iir_butterworth_temp->states0)
    -iir_butterworth_temp->a2 * iir_butterworth_temp->states1;
  *out = ((iir_butterworth_temp->b0 * temp + iir_butterworth_temp->b1 * iir_butterworth_temp->states0) +
          iir_butterworth_temp->b2 * iir_butterworth_temp->states1)*iir_butterworth_temp->gain1;
  
  iir_butterworth_temp->states1 = iir_butterworth_temp->states0;
  iir_butterworth_temp->states0 = temp;
}











