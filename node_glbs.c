#include <stdio.h>
#include <string.h>
#include <math.h>
#include "node_glbs.h"

typedef struct glbs_data_s {
    float value;
    bool valid;
} glbs_data_t;
//Grubbs 表, 临界值GP(n)与两个参数有关：检出水平α (与置信概率P有关)和测量次数n (与自由度f有关)。
/*  
    定检出水平α：如果要求严格，检出水平α可以定得小一些，例如定α＝0.01，
    那么置信概率P＝1－α＝0.99；如果要求不严格，α可以定得大一些，例如定
    α＝0.10，即P＝0.90；通常定α＝0.05，P＝0.95 
*   s_gpn_mode 就是 P
*/
static float gpn_data[][MAX_SAMPLE_NUM] = {
    {1.155, 1.155, 1.155, 1.492, 1.749, 1.944, 2.097, 2.220, 2.323, 2.410, 2.485, 2.550, 2.607, 2.659, 2.705, 2.747, 2.785, 2.821, 2.954, 2.884},
    {1.153, 1.153, 1.153, 1.463, 1.672, 1.822, 1.938, 2.032, 2.110, 2.176, 2.234, 2.285, 2.331, 2.371, 2.409, 2.443, 2.475, 2.501, 2.532, 2.557},
    {1.148, 1.148, 1.148, 1.425, 1.602, 1.729, 1.828, 1.909, 1.977, 2.036, 2.088, 2.134, 2.175, 2.213, 2.247, 2.279, 2.309, 2.335, 2.361, 2.385},
    {1.148, 1.148, 1.148, 1.156, 1.252, 1.329, 1.428, 1.509, 1.577, 1.636, 1.688, 1.734, 1.775, 1.813, 1.847, 1.879, 1.909, 1.935, 1.961, 1.985},
};
static gpn_mode_t s_gpn_mode = GPN_80;

void glbs_init(gpn_mode_t mode)
{
    s_gpn_mode = mode;
}

bool glbs_process(const float *samples, uint8_t num, float *result)
{
    glbs_data_t glbs_data[MAX_SAMPLE_NUM] = {0};
    float average = 0;
    float std_deviation = 0;
    float sum = 0;
    float temp = 0;
    float gpi = 0;
    uint8_t left_num = 0;

    if(num < MIN_SAMPLE_NUM || num > MAX_SAMPLE_NUM) {
        return false;
    }

    memset(glbs_data, 0, sizeof(glbs_data));
    for(uint8_t i = 0; i < num; i++) {
        glbs_data[i].value = samples[i];
        glbs_data[i].valid = true;
    }

//    printf("origin data: ");
//    for(uint8_t i = 0; i < num; i++) {
//        printf("%.3f ", glbs_data[i].value);
//    }
//    printf("\r\n");

    //bubble sort
    //将上述测量数据按从小到大的顺序排列，可以肯定，可疑值不是最小值就是最大值。 
    for(uint8_t i = num - 1; i > 0; i--) {
        for(uint8_t j = 0; j < i; j++) {
            if(glbs_data[j].value > glbs_data[j + 1].value) {
                temp = glbs_data[j].value;
                glbs_data[j].value = glbs_data[j + 1].value;
                glbs_data[j + 1].value = temp;
            }
        }
    }

//    printf("sort data: ");
//    for(uint8_t i = 0; i < num; i++) {
//        printf("%.3f ", glbs_data[i].value);
//    }
//    printf("\r\n");

    while(1) {
        sum = 0;
        left_num = 0;
        for(uint8_t i = 0; i < sizeof(glbs_data)/sizeof(glbs_data[0]); i++) {
            if(glbs_data[i].valid) {
                sum += glbs_data[i].value;
                left_num++;
            }
        }

        if(left_num < MIN_SAMPLE_NUM) {
            break;
        }
        //计算平均值x 计算时，必须将所有数据全部包含在内
        average = sum/left_num;

        sum = 0;
        for(uint8_t i = 0; i < sizeof(glbs_data)/sizeof(glbs_data[0]); i++) {
            if(glbs_data[i].valid) {
                sum += (glbs_data[i].value - average)*(glbs_data[i].value - average);
            }
        }
        sum = sum/(left_num-1);
        //计算标准差s 计算时，必须将所有数据全部包含在内
        std_deviation = sqrt(sum);
    
        //printf("left num = %d, average = %.3f, std_deviation = %.3f\r\n", left_num, average, std_deviation);
        uint8_t i = 0;
        for(i = 0; i < sizeof(glbs_data)/sizeof(glbs_data[0]); i++) {
            if(glbs_data[i].valid) {
                //计算偏离值：平均值与最小值之差 最大值与平均值之差
                //确定一个可疑值 是最小值还是最大值
                //计算Gi值：Gi＝(可疑值 － 平均值 )/ 标准差
                if(glbs_data[i].value > average) {
                    gpi = (glbs_data[i].value - average)/std_deviation;
                } else {
                    gpi = (average - glbs_data[i].value)/std_deviation;
                }
                /*下面要把计算值Gi与格拉布斯表给出的临界值GP(n)比较，如果计
                算的Gi值大于表中的临界值GP(n)，则能判断该测量数据是异常值，可以
                剔除。但是要提醒，临界值GP(n)与两个参数有关：检出水平α (与置信概
                率P有关)和测量次数n (与自由度f有关)。*/
                if(gpi > gpn_data[s_gpn_mode][left_num-1]) {
                    //printf("delete glbs_data[%d] = %.3f, gpi = %.3f gpn[%d] = %.3f\r\n", i, glbs_data[i].value, gpi, left_num, gpn_data[s_gpn_mode][left_num-1]);
                    glbs_data[i].valid = false;
                    break;
                }
            }
        }

        if(i == sizeof(glbs_data)/sizeof(glbs_data[0])) {
            break;
        }
    }

//    printf("final data: ");
//    for(uint8_t i = 0; i < sizeof(glbs_data)/sizeof(glbs_data[0]); i++) {
//        if(glbs_data[i].valid) {
//            printf("%.3f ", glbs_data[i].value);
//        }
//    }
//    printf("\r\n");

    sum = 0;
    left_num = 0;
    for(uint8_t i = 0; i < sizeof(glbs_data)/sizeof(glbs_data[0]); i++) {
        if(glbs_data[i].valid) {
            sum += glbs_data[i].value;
            left_num++;
        }
    }

    *result = sum/left_num;

    return true;
}

//int main(void)
//{
//    //float samples[] = {8.2, 5.4, 14.0, 7.3, 4.7, 9.0, 6.5, 10.1, 7.7, 6.0};
//    float samples[] = {8.2, 5.4, 5.0, 5.2, 15.1, 5.3, 5.5, 6.0};
//    float result = 0;
//
//    glbs_process(samples, sizeof(samples)/sizeof(samples[0]), &result);
//
//    printf("result = %.3f\r\n", result);
//
//    return 0;
//}
