#ifndef  CIRCULAR_BUFFER
#include <stdint.h>
#define  CIRCULAR_BUFFER
/* 环形缓冲区结构体 */
typedef struct {
    int32_t *buffer;        /* 缓冲区指针 */
    int size;          /* 缓冲区大小 */
    int head;          /* 缓冲区头部索引 */
    int tail;          /* 缓冲区尾部索引 */
    int count;         /* 当前元素个数 */
    float sum;     /* 当前总和（用于快速计算平均值） */
} CircularBuffer;




#define WAVE_BUFF_LEN   5
extern int32_t wave_Buffer[WAVE_BUFF_LEN];
extern CircularBuffer cb_wave;



#define TIME_BUFF_LEN   30
extern int32_t time_Buffer[TIME_BUFF_LEN];
extern CircularBuffer cb_time;




#define NORMAL_BUFF_LEN   30
extern int32_t normal_Buffer[NORMAL_BUFF_LEN];
extern CircularBuffer cb_normal;







/* 函数声明 */
void CircularBuffer_Init(CircularBuffer *buffer, int32_t *data_buffer, int size);
void CircularBuffer_Push(CircularBuffer *buffer, float value);
int CircularBuffer_GetAverage(CircularBuffer *buffer);
float CircularBuffer_GetAverageFloat(CircularBuffer *buffer);
void CircularBuffer_Clear(CircularBuffer *buffer);
int CircularBuffer_GetCount(CircularBuffer *buffer);
int CircularBuffer_IsFull(CircularBuffer *buffer);
int CircularBuffer_IsEmpty(CircularBuffer *buffer);



typedef struct
{
	float states0;
	float states1;
	float b0;
	float b1;
	float b2;
	float a0;
	float a1;
	float a2;
	float gain0;
	float gain1;
}IIR_BUTTERWORTH_DEF;

//巴特沃兹算法
void IIR_Butterworth_Coefficient_Init(const float temp[8],IIR_BUTTERWORTH_DEF* iir_butterworth_temp);
void IIR_Butterworth(float in,float* out,IIR_BUTTERWORTH_DEF* iir_butterworth_temp);
void IIR_Par_Init(unsigned char  scan_step);
void IIR_Butterworth_LPF(float in,float* out);


#endif



