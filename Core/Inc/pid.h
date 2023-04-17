#ifndef PID_H
#define PID_H
#include <stdint.h>
#include <math.h>
#define MAX32 (0xffffffff)
#define MAX31 ((int)0x80000000)
#define MAX15 ((short)0x7fff)
#define MIN15 ((short)0x8000)


typedef enum{
    para_is_float,
    para_is_int
}para_type;

typedef enum{
    non_saturate,
    is_saturate
}pid_state;

/**
 * @brief the p_gain、i_gain、d_gain need to be initial
 * @param input the input error (cmd-sensing)
 */
typedef union {
    /*pid 小數運算*/
    struct {         
        float p_gain;
        float i_gain;
        float d_gain;
        float err_sum;
        float last_err;
        float saturation;
        float error;
        float output;
    }dec;
    struct {
        /*pid 整數運算*/
        uint32_t p_gain;   //Q16.16
        uint32_t i_gain;   //Q16.16
        uint32_t d_gain;   //Q16.16
        uint32_t err_sum;
        uint32_t last_err;
        int32_t saturation;
        int32_t error;
        int32_t output;
    }integer;
}pid_para;

/**
 * @param type the parameter that can select (float/int) how to calculate
 */
typedef struct 
{
    pid_para para;
    para_type type;
    pid_state state;
}pid_t;

/**
 * @brief the function use to do pid control 
 * @param pid the parameter use in the function 
 * @retval 0 means success execute -1 means fail execute check the type
 */
int pid_control(pid_t* pid);
/**
 * @brief the function use to init pid param 
 * @param pid the parameter use in the function 
 * @retval 0 means success execute -1 means fail execute check the type
 */
int pid_inital (pid_t* pid , float p_gain ,float i_gain ,float d_gain,
                para_type type ,float saturate, float error);

#endif