/*************************************************************************
    > File Name: glbs.h
    > Author: jimmy
    > Mail: 1074833353@qq.com
    > Created Time: 2021年10月30日 星期六 14时11分03秒
 ************************************************************************/
#ifndef __GLBS_H__
#define __GLBS_H__

#include <stdbool.h>
#include <stdint.h>

#define MIN_SAMPLE_NUM 3
#define MAX_SAMPLE_NUM 20

typedef enum gpn_mode_e {
    GPN_99 = 0,
    GPN_95,
    GPN_90,
    GPN_80,
} gpn_mode_t;

void glbs_init(gpn_mode_t mode);
bool glbs_process(const float *samples, uint8_t num, float *result);

#endif
