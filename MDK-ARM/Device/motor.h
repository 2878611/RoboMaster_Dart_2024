#ifndef MOTOR_H
#define MOTOR_H
#include "struct_typedef.h"
#include "PID.h"

//rm motor data
//单个电机数据结构体
typedef struct
{
	  //ID号
    uint16_t can_id;		
	    
	  //电流相关
    int16_t  set_voltage;		//发送信息
	
	  //速度相关
    int16_t  rotor_speed;		//现在的转速
	  int16_t  target_speed;  //目标速度
	 
	  //角度相关
    float rotor_angle;	  //现在的角度
		float last_angle;		  //上次的角度
	  float angle_difl;     //角度差，用于累加（也可以用于长度计算）
	  float absolute_angle; //绝对角度
	
	  //位置相关
	  int16_t target_pos;
	

    int16_t  torque_current;		//实际转矩电流
    uint8_t  temp;		//电机温度

} dart_motor_t;

//dart total data
//飞镖整体控制结构体
typedef struct 
{
    dart_motor_t motor_info[8];     
    pid_struct_t motor_speed_pid[8];    
    pid_struct_t motor_pos_pid[8]; 


}dart_control;

extern dart_control  dart;
void motor_calc(void);
void motor_init(void);
	
#endif
