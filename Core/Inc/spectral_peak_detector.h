#ifndef SPECTRAL_PEAK_DETECTOR_H
#define SPECTRAL_PEAK_DETECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 峰谷信息结构体 */
typedef struct {
    int      index;       /* 数据索引 */
    float   wavelength;  /* 波长 (X) */
    float   intensity;   /* 强度 (Y) */
    float   prominence;  /* 显著性/突出度 */
    int      is_peak;     /* 1为峰, 0为谷 */
} PeakInfo;




/*
 * 执行峰谷检测
 *
 * @param wavelengths        波长数组 (必须单调递增)
 * @param intensities        强度数组
 * @param count              数据点数 (>=3)
 * @param min_prominence     最小显著性绝对阈值
 * @param min_distance_pts   最小峰间距 (点数)
 * @param out_results        输出缓冲区 (调用者预分配)
 * @param out_capacity       输出缓冲区最大容量
 *
 * @return 实际检测到的峰谷数量; <0 表示参数错误
 */
int detect_peaks_and_valleys(
    signed   short          start_wl,
    signed   short          end_wl,
    signed   short          interval,
    double         min_prominence,
    int            min_distance_pts,
    PeakInfo      *out_results,
    int            out_capacity);

#ifdef __cplusplus
}
#endif

#endif /* SPECTRAL_PEAK_DETECTOR_H */
