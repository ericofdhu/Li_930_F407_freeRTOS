#include "spectral_peak_detector.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

/* 浮点比较容差 */
#define EPS 1e-8



int get_wavelength(int index, int start_wl,  int interval)
{
    return start_wl + index * interval;
}

int get_smoothed(int index, int start_wl,  int interval)
{
    return GuangPuScanLine[start_wl - MIN_WAVELEN + interval*index];
}

/* ------------------------------------------------------------------ */
/* 内部辅助：计算显著性 (Prominence)                                   */
/* 对于峰值：向左右搜索，直到遇到比当前峰值更高的点或边界。            */
/* Prominence = |CurrentHeight - Max(LeftMin, RightMin)|              */
/* ------------------------------------------------------------------ */
static double calculate_prominence(    
                        int16_t          start_wl,
                        int16_t          end_wl,
                        int16_t          interval,
                        int peak_index, int is_peak)
{
    double current_val = get_smoothed(peak_index, start_wl, interval);
    double left_extreme, right_extreme;

    int count = (end_wl - start_wl) / interval + 1;


    /* ---- 向左搜索 ---- */
    int has_left = 0;
    if (is_peak) {
        left_extreme = 0.0; /* 初始化为第一个有效点 */
        for (int i = peak_index - 1; i >= 0; i--) {
            if (get_smoothed(i, start_wl, interval) > current_val) break;       /* 遇到更高的峰 */
            if (!has_left) {
                left_extreme = get_smoothed(i, start_wl, interval);
                has_left = 1;
            } else if (get_smoothed(i, start_wl, interval) < left_extreme) {
                left_extreme = get_smoothed(i, start_wl, interval);
            }
        }
    } else {
        left_extreme = 0.0;
        for (int i = peak_index - 1; i >= 0; i--) {
            if (get_smoothed(i, start_wl, interval) < current_val) break;       /* 遇到更低的谷 */
            if (!has_left) {
                left_extreme = get_smoothed(i, start_wl, interval);
                has_left = 1;
            } else if (get_smoothed(i, start_wl, interval) > left_extreme) {
                left_extreme = get_smoothed(i, start_wl, interval);
            }
        }
    }

    /* ---- 向右搜索 ---- */
    int has_right = 0;
    if (is_peak) {
        right_extreme = 0.0;
        for (int i = peak_index + 1; i < count; i++) {
            if (get_smoothed(i, start_wl, interval) > current_val) break;
            if (!has_right) {
                right_extreme = get_smoothed(i, start_wl, interval);
                has_right = 1;
            } else if (get_smoothed(i, start_wl, interval) < right_extreme) {
                right_extreme = get_smoothed(i, start_wl, interval);
            }
        }
    } else {
        right_extreme = 0.0;
        for (int i = peak_index + 1; i < count; i++) {
            if (get_smoothed(i, start_wl, interval) < current_val) break;
            if (!has_right) {
                right_extreme = get_smoothed(i, start_wl, interval);
                has_right = 1;
            } else if (get_smoothed(i, start_wl, interval) > right_extreme) {
                right_extreme = get_smoothed(i, start_wl, interval);
            }
        }
    }

    /* 基准面：取两侧较高(低)的那个 */
    double base_level;
    if (is_peak) {
        /* 峰：取两侧最小值中的较大者 */
        if (has_left && has_right)
            base_level = (left_extreme > right_extreme) ? left_extreme : right_extreme;
        else if (has_left)
            base_level = left_extreme;
        else if (has_right)
            base_level = right_extreme;
        else
            base_level = current_val; /* 无邻域 */
    } else {
        /* 谷：取两侧最大值中的较小者 */
        if (has_left && has_right)
            base_level = (left_extreme < right_extreme) ? left_extreme : right_extreme;
        else if (has_left)
            base_level = left_extreme;
        else if (has_right)
            base_level = right_extreme;
        else
            base_level = current_val;
    }

    return fabs(current_val - base_level);
}

/* ------------------------------------------------------------------ */
/* 内部辅助：qsort 比较函数 — 按显著性降序排列                         */
/* ------------------------------------------------------------------ */
static int cmp_prominence_desc(const void *a, const void *b)
{
    const PeakInfo *pa = (const PeakInfo *)a;
    const PeakInfo *pb = (const PeakInfo *)b;
    if (pa->prominence > pb->prominence) return -1;
    if (pa->prominence < pb->prominence) return  1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* 内部辅助：最小距离过滤                                              */
/* 在 min_dist 范围内只保留显著性最高的那个                            */
/* ------------------------------------------------------------------ */
static int filter_by_min_distance(PeakInfo *peaks, int n, int min_dist)
{
    if (n <= 1) return n;

    /* 按显著性降序排序 */
    qsort(peaks, n, sizeof(PeakInfo), cmp_prominence_desc);

    /* removed 标记数组 */
    /* 使用变长数组(VLA)或栈分配；若 n 可能很大，请改为静态/动态分配 */
    /* 嵌入式环境中建议传入一个固定的 removed 缓冲区 */
    uint8_t removed[256]; /* 最多支持256个候选点 */
    if (n > 256) n = 256;  /* 安全截断 */
    memset(removed, 0, sizeof(removed));

    int kept_count = 0;

    for (int i = 0; i < n; i++) {
        if (removed[i]) continue;

        /* 保留当前峰 */
        peaks[kept_count++] = peaks[i];

        /* 移除附近 min_dist 范围内的其他较弱峰值 */
        for (int j = i + 1; j < n; j++) {
            if (removed[j]) continue;
            if (abs(peaks[j].index - peaks[i].index) < min_dist) {
                removed[j] = 1;
            }
        }
    }

    return kept_count;
}


PeakInfo gPeak[30];

/* ------------------------------------------------------------------ */
/* 主函数：峰谷检测                                                    */
/* ------------------------------------------------------------------ */
int detect_peaks_and_valleys(
    int16_t          start_wl,
    int16_t          end_wl,
    int16_t          interval,
    double         min_prominence,
    int            min_distance_pts,
    PeakInfo      *out_results,
    int            out_capacity)
{
    // if (wavelengths == NULL || intensities == NULL ||
    //     out_results == NULL || out_capacity <= 0)
    //    return -1;

    int count = (end_wl - start_wl) / interval + 1;

    if (count < 3)
        return -1;

    /* 直接使用 intensities 作为 smoothed（原C#代码中高斯平滑被注释掉） */
    // const double *smoothed = intensities;

    /* 2. 计算动态范围 */
    double min_val = get_smoothed(0, start_wl, interval);
    double max_val = get_smoothed(0, start_wl, interval);
    for (int i = 1; i < count; i++) {
        double val = get_smoothed(i, start_wl, interval);
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    double range = max_val - min_val;
    if (range == 0.0) return 0; /* 平坦信号 */

    /* 3. 寻找候选极值点 */
    /* 嵌入式环境使用栈上缓冲区；若数据量大请改为外部传入 */
    PeakInfo candidates[256];
    int cand_count = 0;

    for (int i = 1; i < count - 1; i++) {
        double prev = get_smoothed(i - 1, start_wl, interval);
        double curr = get_smoothed(i, start_wl, interval);
        double next = get_smoothed(i + 1, start_wl, interval);
        int handled = 0;

        /* 1) 检测平顶/平谷 (plateau) */
        if (fabs(curr - next) < EPS || fabs(curr - prev) < EPS) {
            int left = i;
            int right = i;
            /* 向左扩展 */
            while (left - 1 >= 0 && fabs(get_smoothed(left - 1, start_wl, interval) - curr) < EPS)
                left--;
            /* 向右扩展 */
            while (right + 1 < count && fabs(get_smoothed(right + 1, start_wl, interval) - curr) < EPS)
                right++;

            int has_left_n  = (left - 1 >= 0)        ? 1 : 0;
            int has_right_n = (right + 1 < count)    ? 1 : 0;

            double left_neighbor  = has_left_n  ? get_smoothed(left - 1, start_wl, interval)  : 0.0;
            double right_neighbor = has_right_n ? get_smoothed(right + 1, start_wl, interval) : 0.0;

            int is_plateau_peak   = has_left_n && has_right_n &&
                                     (left_neighbor  + EPS < curr) &&
                                     (right_neighbor + EPS < curr);
            int is_plateau_valley = has_left_n && has_right_n &&
                                     (left_neighbor  - EPS > curr) &&
                                     (right_neighbor - EPS > curr);

            if ((is_plateau_peak || is_plateau_valley) && cand_count < 256) {
                int center_index = (left + right) / 2;
                candidates[cand_count].index      = center_index;
                candidates[cand_count].wavelength = get_wavelength(center_index, start_wl, interval);
                candidates[cand_count].intensity  = get_smoothed(center_index, start_wl, interval);
                candidates[cand_count].is_peak    = is_plateau_peak;
                candidates[cand_count].prominence = 0.0;
                cand_count++;

                i = right;
                handled = 1;
            }
        }

        if (handled) continue;

        /* 2) 常规局部极值检测 */
        if (curr > prev && curr > next) {
            if (cand_count < 256) {
                candidates[cand_count].index      = i;
                candidates[cand_count].wavelength = get_wavelength(i, start_wl, interval);
                candidates[cand_count].intensity  = curr;
                candidates[cand_count].is_peak    = 1;
                candidates[cand_count].prominence = 0.0;
                cand_count++;
            }
        } else if (curr < prev && curr < next) {
            if (cand_count < 256) {
                candidates[cand_count].index      = i;
                candidates[cand_count].wavelength = get_wavelength(i, start_wl, interval);
                candidates[cand_count].intensity  = curr;
                candidates[cand_count].is_peak    = 0;
                candidates[cand_count].prominence = 0.0;
                cand_count++;
            }
        }
    }

    /* 4. 过滤与精炼 */
    PeakInfo filtered[256];
    int filt_count = 0;

    for (int i = 0; i < cand_count; i++) {
        double prom = calculate_prominence(start_wl, end_wl, interval,
                                           candidates[i].index,
                                           candidates[i].is_peak);

        if (prom < min_prominence)
            continue;

        candidates[i].prominence = prom;
        filtered[filt_count++] = candidates[i];
    }

    /* C. 应用最小距离过滤 */
    filt_count = filter_by_min_distance(filtered, filt_count, min_distance_pts);

    /* 按波长排序输出（简单插入排序，嵌入式环境友好） */
    for (int i = 1; i < filt_count; i++) {
        PeakInfo key = filtered[i];
        int j = i - 1;
        while (j >= 0 && filtered[j].wavelength > key.wavelength) {
            filtered[j + 1] = filtered[j];
            j--;
        }
        filtered[j + 1] = key;
    }

    /* 拷贝到输出缓冲区 */
    int out_count = (filt_count < out_capacity) ? filt_count : out_capacity;
    memcpy(out_results, filtered, (size_t)out_count * sizeof(PeakInfo));

    return out_count;
}
