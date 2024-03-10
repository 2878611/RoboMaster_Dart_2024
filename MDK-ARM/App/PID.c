#include "pid.h"
#include "main.h"

	
#define LIMIT_MIN_MAX(x,min,max) (x) = (((x)<=(min))?(min):(((x)>=(max))?(max):(x)))		//越界则赋边界值


void pid_init(pid_struct_t *pid,
              float PID[3],
              float i_max,
              float out_max)
{
  pid->kp      = PID[0];
  pid->ki      = PID[1];
  pid->kd      = PID[3];
  pid->i_max   = i_max;
  pid->out_max = out_max;
}

float pid_calc(pid_struct_t *pid, float ref, float fdb)//ref是目标值,fdb是电机解码的速度返回值
{
  pid->ref = ref;
  pid->fdb = fdb;
  pid->err[1] = pid->err[0];//err[1]是上一次计算出来的差值
  pid->err[0] = pid->ref - pid->fdb;//err[0]是这一次的预期速度和实际速度的差值,这两个值是可以是负数的
  
  pid->p_out  = pid->kp * pid->err[0];//40 3 0是标准值，把这个加到watch1里面
  pid->i_out += pid->ki * pid->err[0];
  pid->d_out  = pid->kd * (pid->err[0] - pid->err[1]);
  LIMIT_MIN_MAX(pid->i_out, -pid->i_max, pid->i_max);//防止越界
  
  pid->output = pid->p_out + pid->i_out + pid->d_out;
  LIMIT_MIN_MAX(pid->output, -pid->out_max, pid->out_max);//防止越界
  return pid->output;
}



float pos_pid_calc(pid_struct_t *pid, float ref, float fdb)//ref是目标值,fdb是电机解码的速度返回值
{
  float err;
  pid->ref = ref;
  pid->fdb = fdb;
  pid->err[1] = pid->err[0];//err[1]是上一次计算出来的差值
	
	err = pid->ref - pid->fdb;
   
	
	if(err > 8191/2) 
	{
		err -= 8191;
	}
	else if(err < -8191/2)
	{
		err += 8191;
	}
	
  pid->err[0] = err;//err[0]是这一次的预期速度和实际速度的差值,这两个值是可以是负数的
	
  pid->p_out  = pid->kp * pid->err[0];//40 3 0是标准值，把这个加到watch1里面
  pid->i_out += pid->ki * pid->err[0];
  pid->d_out  = pid->kd * (pid->err[0] - pid->err[1]);
  LIMIT_MIN_MAX(pid->i_out, -pid->i_max, pid->i_max);//防止越界
  
  pid->output = pid->p_out + pid->i_out + pid->d_out;
  LIMIT_MIN_MAX(pid->output, -pid->out_max, pid->out_max);//防止越界
  return pid->output;
}


