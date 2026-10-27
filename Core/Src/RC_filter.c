#include "Rc_filter.h"


#ifndef NULL
#define NULL ((void *)0)
#endif

#define M_PI ((double)3.14159265358979323846)

RCFilter wave_scan_filter;
RCFilter time_scan_filter;
RCFilter normal_filter;

/**
 * @brief 初始化一阶RC滤波器
 * @param filter 滤波器结构体指针
 * @param cutoff_freq 截止频率（Hz）
 * @param sample_freq 采样频率（Hz）
 * @return 0: 成功, -1: 失败
 */
int8_t rc_filter_init(RCFilter *filter, float cutoff_freq, float sample_freq) {
    if (filter == NULL || cutoff_freq <= 0.0f || sample_freq <= 0.0f) {
        return -1;
    }
    
    // 计算时间常数 RC = 1 / (2 * π * fc)
    float rc = 1.0f / (2.0f * (float)M_PI * cutoff_freq);
    
    // 计算滤波系数 α = Δt / (RC + Δt)
    float dt = 1.0f / sample_freq;
    filter->alpha = dt / (rc + dt);
    
    // 初始化状态
    filter->prev_out = 0.0f;
    filter->initialized = 0;
    
    return 0;
}

/**
 * @brief 直接设置滤波系数（简化初始化）
 * @param filter 滤波器结构体指针
 * @param alpha 滤波系数（0.0 ~ 1.0，值越大响应越快，滤波效果越差）
 * @return 0: 成功, -1: 失败
 */
int8_t rc_filter_init_alpha(RCFilter *filter, float alpha) {
    if (filter == NULL || alpha <= 0.0f || alpha > 1.0f) {
        return -1;
    }
    
    filter->alpha = alpha;
    filter->prev_out = 0.0f;
    filter->initialized = 0;
    
    return 0;
}

/**
 * @brief 对单个采样值进行滤波
 * @param filter 滤波器结构体指针
 * @param input 当前采样值
 * @return 滤波后的输出值
 */
float rc_filter_process(RCFilter *filter, float input) {
    if (filter == NULL) {
        return input;
    }
    
    float output;
    
    // 首次调用，直接使用输入值作为初始状态
    if (!filter->initialized) {
        output = input;
        filter->initialized = 1;
    } else {
        // 一阶RC滤波公式：y[n] = α * x[n] + (1 - α) * y[n-1]
        output = filter->alpha * input + (1.0f - filter->alpha) * filter->prev_out;
    }
    
    // 更新上一次输出值
    filter->prev_out = output;
    
    return output;
}

/**
 * @brief 重置滤波器状态
 * @param filter 滤波器结构体指针
 */
void rc_filter_reset(RCFilter *filter) {
    if (filter != NULL) {
        filter->prev_out = 0.0f;
        filter->initialized = 0;
    }
}

/**
 * @brief 获取当前滤波系数
 * @param filter 滤波器结构体指针
 * @return 滤波系数 α
 */
float rc_filter_get_alpha(RCFilter *filter) {
    return (filter != NULL) ? filter->alpha : 0.0f;
}



