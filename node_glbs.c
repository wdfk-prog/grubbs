/**
 * @file glbs.c
 * @author wdfk-prog
 * @brief Implementation of the Grubbs' Test for statistical outlier detection.
 * @version 1.0
 * @date 2021-10-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "glbs.h"

/**
 * @brief Internal data structure to hold a sample value and its validity status.
 */
typedef struct glbs_data_s {
    float value; /**< The floating point value of the sample. */
    bool  valid; /**< Flag indicating if the sample is considered valid (not an outlier). */
} glbs_data_t;

/**
 * @brief Grubbs' test critical values table G_p(n).
 *
 * The critical value depends on two parameters:
 * 1. The significance level alpha (related to the confidence probability P = 1 - alpha).
 * 2. The number of measurements n.
 *
 * Rows correspond to confidence levels (99%, 95%, 90%, 80%).
 * Columns correspond to the number of samples (n), from 3 to 20.
 * The index for n is `n-1`, but since our min samples is 3, the table index is `n-1`.
 * For n=3, use column index 2.
 */
static float gpn_data[][MAX_SAMPLE_NUM] = {
    /* n=3,   4,     5,     6,     7,     8,     9,     10,    11,    12,    13,    14,    15,    16,    17,    18,    19,    20  */
    /* P=99% (alpha=0.01) */
    {1.155, 1.155, 1.155, 1.492, 1.749, 1.944, 2.097, 2.220, 2.323, 2.410, 2.485, 2.550, 2.607, 2.659, 2.705, 2.747, 2.785, 2.821, 2.954, 2.884},
    /* P=95% (alpha=0.05) */
    {1.153, 1.153, 1.153, 1.463, 1.672, 1.822, 1.938, 2.032, 2.110, 2.176, 2.234, 2.285, 2.331, 2.371, 2.409, 2.443, 2.475, 2.501, 2.532, 2.557},
    /* P=90% (alpha=0.10) */
    {1.148, 1.148, 1.148, 1.425, 1.602, 1.729, 1.828, 1.909, 1.977, 2.036, 2.088, 2.134, 2.175, 2.213, 2.247, 2.279, 2.309, 2.335, 2.361, 2.385},
    /* P=80% (alpha=0.20) */
    {1.148, 1.148, 1.148, 1.156, 1.252, 1.329, 1.428, 1.509, 1.577, 1.636, 1.688, 1.734, 1.775, 1.813, 1.847, 1.879, 1.909, 1.935, 1.961, 1.985},
};

/**
 * @brief Static variable to store the currently configured confidence mode.
 */
static gpn_mode_t s_gpn_mode = GPN_80;

/**
 * @brief Initializes the Grubbs' test module with a specific confidence level.
 *
 * @param[in] mode The desired confidence level from gpn_mode_t.
 */
void glbs_init(gpn_mode_t mode)
{
    s_gpn_mode = mode;
}

/**
 * @brief Processes a set of samples to remove outliers using Grubbs' test.
 *
 * @param[in]  samples Pointer to the input array of sample data.
 * @param[in]  num     The number of samples in the input array.
 * @param[out] result  Pointer to a float where the calculated average of the valid
 *                     samples will be stored.
 *
 * @return bool Returns true on success, false on failure.
 */
bool glbs_process(const float *samples, uint8_t num, float *result)
{
    glbs_data_t glbs_data[MAX_SAMPLE_NUM] = {0};
    float       average                   = 0.0f;
    float       std_deviation             = 0.0f;
    float       sum                       = 0.0f;
    float       temp                      = 0.0f;
    float       gpi                       = 0.0f;
    uint8_t     left_num                  = 0;

    if (num < MIN_SAMPLE_NUM || num > MAX_SAMPLE_NUM) {
        return false;
    }

    // Initialize the working data array.
    memset(glbs_data, 0, sizeof(glbs_data));
    for (uint8_t i = 0; i < num; i++) {
        glbs_data[i].value = samples[i];
        glbs_data[i].valid = true;
    }

    // Sort the data in ascending order using a simple bubble sort.
    // An outlier, if it exists, will be either the minimum or maximum value.
    for (uint8_t i = num - 1; i > 0; i--) {
        for (uint8_t j = 0; j < i; j++) {
            if (glbs_data[j].value > glbs_data[j + 1].value) {
                temp                   = glbs_data[j].value;
                glbs_data[j].value     = glbs_data[j + 1].value;
                glbs_data[j + 1].value = temp;
            }
        }
    }

    // Iteratively find and remove outliers.
    while (1) {
        sum      = 0.0f;
        left_num = 0;
        for (uint8_t i = 0; i < num; i++) {
            if (glbs_data[i].valid) {
                sum += glbs_data[i].value;
                left_num++;
            }
        }

        // Stop if the number of remaining samples is too small.
        if (left_num < MIN_SAMPLE_NUM) {
            break;
        }

        // Step 1: Calculate the average (mean) of the current valid data set.
        average = sum / left_num;

        // Step 2: Calculate the standard deviation of the current valid data set.
        sum = 0.0f;
        for (uint8_t i = 0; i < num; i++) {
            if (glbs_data[i].valid) {
                sum += (glbs_data[i].value - average) * (glbs_data[i].value - average);
            }
        }
        std_deviation = sqrt(sum / (left_num - 1));

        // Step 3: Calculate the Grubbs' test statistic (Gi) for each point.
        // Gi = |value - average| / std_deviation
        uint8_t i = 0;
        for (i = 0; i < num; i++) {
            if (glbs_data[i].valid) {
                // Since the data is sorted, we only need to check the min and max,
                // but this implementation checks all points for simplicity.
                gpi = fabs(glbs_data[i].value - average) / std_deviation;

                // Step 4: Compare Gi with the critical value G_p(n) from the table.
                // If Gi > G_p(n), the data point is an outlier.
                if (gpi > gpn_data[s_gpn_mode][left_num - 1]) {
                    glbs_data[i].valid = false;
                    // Break and restart the loop with the smaller data set.
                    break;
                }
            }
        }

        // If the inner loop completed without finding any outliers, we are done.
        if (i == num) {
            break;
        }
    }

    // Calculate the final average from the remaining valid data points.
    sum      = 0.0f;
    left_num = 0;
    for (uint8_t i = 0; i < num; i++) {
        if (glbs_data[i].valid) {
            sum += glbs_data[i].value;
            left_num++;
        }
    }

    // Prevent division by zero if all points were discarded.
    if (left_num > 0) {
        *result = sum / left_num;
    } else {
        *result = 0.0f; // Or handle as an error case.
    }


    return true;
}