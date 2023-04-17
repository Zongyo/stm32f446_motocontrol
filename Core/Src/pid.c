#include "../Inc/pid.h"

int pid_inital (pid_t* pid , float p_gain ,float i_gain ,float d_gain,
                para_type type,float saturate, float error){
    if (pid->type == para_is_float){
        pid->para.dec.p_gain = p_gain;
        pid->para.dec.i_gain = i_gain;
        pid->para.dec.d_gain = d_gain;
        pid->para.dec.err_sum = 0;
        pid->para.dec.last_err = error;
        if (saturate == 0)
            pid->para.integer.saturation = INFINITY;
        else
            pid->para.dec.saturation = saturate;
        return 0;
    }
    else if(pid->type == para_is_int){
        pid->para.integer.p_gain=(uint32_t) p_gain * (1<<16);
        pid->para.integer.i_gain=(uint32_t) i_gain * (1<<16);
        pid->para.integer.d_gain=(uint32_t) d_gain * (1<<16);
        pid->para.integer.err_sum = 0;
        pid->para.integer.last_err = (int32_t)error;
        if (saturate == 0)
            pid->para.integer.saturation = 0x20000000;
        else
            pid->para.integer.saturation = (int32_t)saturate;
        return 0;
    }
    else{
        return -1;
    }
}

int pid_control( pid_t* pid){
    switch (pid->type){
        case para_is_float:{
            float sum = 0;
            //避免誤差積分數值太大
            if ( pid->state == is_saturate && (pid->para.integer.err_sum ^ pid->para.integer.error) < 0){
                pid->para.dec.err_sum += pid->para.dec.error;
            }
            else if(pid->state == non_saturate){
                 pid->para.dec.err_sum += pid->para.dec.error;
            }

            sum = pid->para.dec.error * pid->para.dec.p_gain;
            sum = sum + pid->para.dec.err_sum * pid->para.dec.i_gain;
            //有需要微分器再執行
            if (pid->para.dec.d_gain != 0)
            {
                sum = sum + (pid->para.dec.error - pid->para.dec.last_err) * pid->para.dec.d_gain;
                pid->para.dec.last_err = (pid->para.dec.last_err + pid->para.dec.error)/2 ; //0.5 + 0.25+ 0.125+ ...
            }

            //輸出飽和上限
            if (sum > pid->para.dec.saturation){          //輸出正飽和
                sum = pid->para.dec.saturation;
                pid->state = is_saturate;
            }
            else if(sum < -pid->para.dec.saturation){   //輸出負飽和
                sum = -pid->para.dec.saturation;
                pid->state = is_saturate;
            }
            else{
                pid->state=non_saturate;
            }
            
            pid->para.dec.output = sum;
            return 0;
            break;
        }
        //trade off:做floating的乘法可能會比做int64的乘法快，要做優化可能需要降低一點安全性
        case para_is_int:{
            int64_t sum = 0;

            if ( pid->state == is_saturate && (pid->para.integer.err_sum ^ pid->para.integer.error) < 0){
                pid->para.integer.err_sum += pid->para.integer.error;
            }
            else if(pid->state == non_saturate){
                 pid->para.integer.err_sum += pid->para.integer.error;
            }

            sum = (pid->para.integer.error * pid->para.integer.p_gain);
            sum = sum + (pid->para.integer.err_sum * pid->para.integer.i_gain );
            if (pid->para.integer.d_gain != 0)
            {
                sum = sum + ((pid->para.integer.error - pid->para.integer.last_err) * pid->para.integer.d_gain );
                pid->para.integer.last_err = (pid->para.integer.last_err >> 1) + (pid->para.integer.error >> 1);
            }
            sum = sum >> 16;
            //檢查是否輸出飽和
            if (sum>pid->para.integer.saturation){
                sum = pid->para.integer.saturation;
                pid->state = is_saturate;
            }
            else if(sum<-pid->para.integer.saturation){
                sum = -pid->para.integer.saturation;
                pid->state = is_saturate;
            }
            else{
                pid->state = non_saturate;
            }
            
            pid->para.integer.output = sum;
            return 0;
            break;
        }
        default :{
            return -1;
            break;
        }
    }
}