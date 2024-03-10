#include "motor.h"
#include "struct_typedef.h"
#include "can_user.h"
//3508标识符 0x200 0x1FF   反馈：0x200+ID
//2006标识符 0x200 0x1FF   反馈：0x200+ID
//6020标识符 0x1FF 0x2FF   反馈：0x204+ID

/*
can1 6020*3 
can2 2006*2 3508*2
*/

dart_control  dart;

void motor_2006_push_calc(uint16_t i);
void motor_3508_calc(uint16_t i);
void motor_2006_calc(uint16_t i);
void motor_pitch_calc(uint16_t i);
void motor_yaw_calc(uint16_t i);

//电机初始化函数
void motor_init(void)
{
	for(uint8_t index = 0;index<7;index++)
	{
	   dart.motor_info[index].last_angle = dart.motor_info[index].rotor_angle;
	}
}

//电机计算函数
void motor_calc(void)
{
	//推杆
	motor_2006_push_calc(2);
	//pitch
	motor_pitch_calc(5);
	motor_pitch_calc(6);
	//yaw
	motor_yaw_calc(7);
	//弹仓
	motor_2006_calc(3);
}


//2006向上推杆
void motor_2006_push_calc(uint16_t i)
{

	dart.motor_info[i].angle_difl = dart.motor_info[i].rotor_angle - dart.motor_info[i].last_angle;  //3508转过的角度差
	dart.motor_info[i].last_angle = dart.motor_info[i].rotor_angle;       
	
	if(dart.motor_info[i].angle_difl>8191/2)
	{
		dart.motor_info[i].angle_difl -= 8191;
	}
	else if(dart.motor_info[i].angle_difl <- 8191/2)
	{
		dart.motor_info[i].angle_difl +=8191;
	}
	
	dart.motor_info[i].angle_difl/=5000;               //减速比 
	
	dart.motor_info[i].absolute_angle -= dart.motor_info[i].angle_difl;  //角度差累加,得到绝对角度
	
	if(dart.motor_info[i].absolute_angle>8191)
	{
		dart.motor_info[i].absolute_angle -= 8191;
	}
	else if(dart.motor_info[i].absolute_angle<0)
	{
		dart.motor_info[i].absolute_angle+=8191;
	}
	
}

//3508电机处理函数，将减速比消除
void motor_3508_calc(uint16_t i)
{
	dart.motor_info[i].angle_difl = dart.motor_info[i].rotor_angle - dart.motor_info[i].last_angle;  //3508转过的角度差
	dart.motor_info[i].last_angle = dart.motor_info[i].rotor_angle;       
	
	if(dart.motor_info[i].angle_difl>8191/2)
	{
		dart.motor_info[i].angle_difl -= 8191;
	}
	else if(dart.motor_info[i].angle_difl <- 8191/2)
	{
		dart.motor_info[i].angle_difl +=8191;
	}
	
	dart.motor_info[i].angle_difl/=(3591/187);               //减速比 
	
	dart.motor_info[i].absolute_angle += dart.motor_info[i].angle_difl;  //角度差累加,得到绝对角度
	
	if(dart.motor_info[i].absolute_angle>8191)
	{
		dart.motor_info[i].absolute_angle -= 8191;
	}
	else if(dart.motor_info[i].absolute_angle<0)
	{
		dart.motor_info[i].absolute_angle+=8191;
	}													
}

//2006电机处理函数，将减速比消除
void motor_2006_calc(uint16_t i)
{
	dart.motor_info[i].angle_difl = dart.motor_info[i].rotor_angle - dart.motor_info[i].last_angle;  //3508转过的角度差
	dart.motor_info[i].last_angle = dart.motor_info[i].rotor_angle;       
	
	if(dart.motor_info[i].angle_difl>8191/2)
	{
		dart.motor_info[i].angle_difl -= 8191;
	}
	else if(dart.motor_info[i].angle_difl <- 8191/2)
	{
		dart.motor_info[i].angle_difl +=8191;
	}
	
	dart.motor_info[i].angle_difl/=360;               //减速比 
	
	dart.motor_info[i].absolute_angle += dart.motor_info[i].angle_difl;  //角度差累加,得到绝对角度
	
	if(dart.motor_info[i].absolute_angle>8191)
	{
		dart.motor_info[i].absolute_angle -= 8191;
	}
	else if(dart.motor_info[i].absolute_angle<0)
	{
		dart.motor_info[i].absolute_angle+=8191;
	}													
}

//6020电机处理函数
void motor_pitch_calc(uint16_t i)
{
	dart.motor_info[i].angle_difl = dart.motor_info[i].rotor_angle - dart.motor_info[i].last_angle;  //3508转过的角度差
	dart.motor_info[i].last_angle = dart.motor_info[i].rotor_angle;       
	
	if(dart.motor_info[i].angle_difl>8191/2)
	{
		dart.motor_info[i].angle_difl -= 8191;
	}
	else if(dart.motor_info[i].angle_difl <- 8191/2)
	{
		dart.motor_info[i].angle_difl +=8191;
	}
	
	dart.motor_info[i].angle_difl/=100;               //减速比 
	
	dart.motor_info[i].absolute_angle += dart.motor_info[i].angle_difl;  //角度差累加,得到绝对角度
	
	if(dart.motor_info[i].absolute_angle>8191)
	{
		dart.motor_info[i].absolute_angle -= 8191;
	}
	else if(dart.motor_info[i].absolute_angle<0)
	{
		dart.motor_info[i].absolute_angle+=8191;
	}													
}

//6020电机处理函数
void motor_yaw_calc(uint16_t i)
{
	dart.motor_info[i].angle_difl = dart.motor_info[i].rotor_angle - dart.motor_info[i].last_angle;  //3508转过的角度差
	dart.motor_info[i].last_angle = dart.motor_info[i].rotor_angle;       
	
	if(dart.motor_info[i].angle_difl>8191/2)//角度差大于半圈/****需要限制电机转速，转速过快会失效，不过飞镖应该没有问题****/
	{
		dart.motor_info[i].angle_difl -= 8191;
	}
	else if(dart.motor_info[i].angle_difl <- 8191/2)
	{
		dart.motor_info[i].angle_difl +=8191;
	}
	
	dart.motor_info[i].angle_difl/=100;               //减速比 
	
	dart.motor_info[i].absolute_angle -= dart.motor_info[i].angle_difl;  //角度差累加,得到绝对角度
	
	if(dart.motor_info[i].absolute_angle>8191)
	{
		dart.motor_info[i].absolute_angle -= 8191;
	}
	else if(dart.motor_info[i].absolute_angle<0)
	{
		dart.motor_info[i].absolute_angle+=8191;
	}													
}


