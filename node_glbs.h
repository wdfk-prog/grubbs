/**
 * @file glbs.h
 * @author wdfk-prog
 * @brief Public interface for the Grubbs' Test algorithm to detect and remove outliers.
 * @version 1.0
 * @date 2021-10-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __GLBS_H__
#define __GLBS_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief The minimum number of samples required for the Grubbs' test.
 */
#define MIN_SAMPLE_NUM 3

/**
 * @brief The maximum number of samples supported by the Grubbs' test implementation.
 */
#define MAX_SAMPLE_NUM 20

/**
 * @brief Defines the confidence level for the Grubbs' test.
 *
 * The confidence level (P) is related to the significance level (alpha) by P = 1 - alpha.
 * A higher confidence level makes the test stricter, meaning it is less likely to
 * identify a data point as an outlier.
 */
typedef enum gpn_mode_e {
    GPN_99 = 0, /*!< 99% confidence level (alpha = 0.01) */
    GPN_95,     /*!< 95% confidence level (alpha = 0.05) */
    GPN_90,     /*!< 90% confidence level (alpha = 0.10) */
    GPN_80,     /*!< 80% confidence level (alpha = 0.20) */
} gpn_mode_t;

/**
 * @brief Initializes the Grubbs' test module with a specific confidence level.
 *
 * This function must be called before using glbs_process().
 *
 * @param[in] mode The desired confidence level from gpn_mode_t.
 */
void glbs_init(gpn_mode_t mode);

/**
 * @brief Processes a set of samples to remove outliers using Grubbs' test.
 *
 * This function takes an array of floating-point samples, iteratively removes
 * any identified outliers based on the configured confidence level, and calculates
 * the average of the remaining valid data points.
 *
 * @param[in]  samples Pointer to the input array of sample data.
 * @param[in]  num     The number of samples in the input array. Must be between
 *                     MIN_SAMPLE_NUM and MAX_SAMPLE_NUM.
 * @param[out] result  Pointer to a float where the calculated average of the valid
 *                     samples will be stored.
 *
 * @return bool Returns true on successful processing, false if the input parameters
 *              are invalid (e.g., sample count is out of range).
 */
bool glbs_process(const float *samples, uint8_t num, float *result);

#endif /* __GLBS_H__ */