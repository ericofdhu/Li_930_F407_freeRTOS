#include <stdint.h>
#include <math.h>

/**
 * @brief 一阶RC滤波器结构体
 */
typedef struct {
    float alpha;      // 滤波系数 (0.0 ~ 1.0)
    float prev_out;   // 上一次的输出值
    uint8_t initialized; // 是否已初始化
} RCFilter;


#define TIME_SCAN_SAMPLE_FREQ 10.0f  // 采样频率 1000Hz
#define TIME_SCAN_CUTO_FREQ 0.2f    // 截止频率 50Hz

#define WAVE_SCAN_SAMPLE_FREQ 100.0f  // 采样频率 1000Hz
#define WAVE_SCAN_CUTO_FREQ 50.0f    // 截止频率 50Hz


extern RCFilter wave_scan_filter;
extern RCFilter time_scan_filter;
extern RCFilter normal_filter;



int8_t rc_filter_init(RCFilter *filter, float cutoff_freq, float sample_freq);
float rc_filter_process(RCFilter *filter, float input);

