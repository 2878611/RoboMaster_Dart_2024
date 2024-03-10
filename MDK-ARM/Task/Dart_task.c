#include "stm32f4xx.h"                  // Device header
#include "Dart_task.h" 
#include "cmsis_os.h"
#include "gpio.h"
#include "remote_control.h"
#include "motor.h"
#include "can_user.h"
#include "switch.h"
#include <stdlib.h> 
#include <math.h>
//变量定义==============================================
//PID
fp32 motor_speed_pid[3]={1,0,0};
fp32 motor_pos_pid[3] = {1,0,1};
uint8_t dart_adjust_falg[3] = {0};

uint8_t flag[4] = {0};
uint8_t speed1;
uint8_t speed2;
//函数声明==============================================
static void dart_init(void);
static void mode_choose(void);
static void dart_adjust(void);
//static void dart_mode_reset(void);   //飞镖电机编码值置0
static void dart_control_mode(void); //飞镖遥控器控制
//static void dart_pos_mode(void);     //飞镖位置控制
static void calc_and_send(void);


//主函数==============================================
void Dart_task(void const * argument)
{ 
  dart_init();//PID计算初始化
  motor_init();//修改 前角度 last_angle
  for(;;)
  {
		dart_adjust();//编码器校准，通过限位开关读取绝对角度
		motor_calc();
	  //mode_choose();
		dart_control_mode();//遥控器控制飞镖指向
    calc_and_send();		//发送驱动信号
    osDelay(1);
		for(uint8_t i=0;i<4;i++)/****这个好像没有用****/
		{
			flag[i] = get_switch(i);
		}
  }
}

//初始化==============================================
void dart_init(void)
{
  for (uint8_t i = 4; i < 8; i++)
	{
        pid_init(&dart.motor_speed_pid[i], motor_speed_pid, 16000, 16000); //init pid parameter, kp=40, ki=3, kd=0, output limit = 16384			
	      //pid_init(&dart.motor_pos_pid[i], motor_pos_pid, 10000, 10000);
	}   
}

//模式选择=======================================+-*=======
//遥控器 左S s[1] 132   右S s[0] 132
// 左右ch[2] 上下ch[3]  左右ch[0] 上下ch[1]
void mode_choose(void)
{
  
//	if(rc_ctrl.rc.s[0] == 2)
//	{
//		 dart_adjust();
//	}
//	else
//	{
//		dart_adjust_falg[1] = 0;
//	}
	
	if(rc_ctrl.rc.s[0] == 3)
	{
		  dart_control_mode();
	}
  
}


//遥控器控制飞镖
void dart_control_mode(void)
{
	//YAW轴
	if(rc_ctrl.rc.ch[0]<-110 && abs(dart.motor_info[7].torque_current) < 4000 && get_switch(1) == 0)
		/****同时满足：右摇杆横向滑动一定距离；当前电流小于一定值；一号开关输出为低电平（即限位开关未接触）****/
	{
		dart.motor_info[7].target_speed = -rc_ctrl.rc.ch[0]*20;
	}
	else if(rc_ctrl.rc.ch[0] > 110 && abs(dart.motor_info[2].torque_current) < 4000 && get_switch(5) == 0)//get_switch(5) == 0
		/****这个2900是怎么确定的
				2900*100/8191=35.4圈
	****/
	{
		dart.motor_info[7].target_speed = -rc_ctrl.rc.ch[0]*20;
	}
	else
	{
		dart.motor_info[7].target_speed = 0;
	}
	
	//PITCH轴
	if(rc_ctrl.rc.ch[1] < -110 && abs(dart.motor_info[2].torque_current) < 4000 && get_switch(0) == 0)
	{
		dart.motor_info[6].target_speed = rc_ctrl.rc.ch[1]*20;
		dart.motor_info[5].target_speed = rc_ctrl.rc.ch[1]*20;
	}
	else if(rc_ctrl.rc.ch[1] > 110 && abs(dart.motor_info[2].torque_current) < 4000 && get_switch(4) == 0)
	{
		dart.motor_info[6].target_speed = rc_ctrl.rc.ch[1]*20;
		dart.motor_info[5].target_speed = rc_ctrl.rc.ch[1]*20;
	}
	else
	{
		dart.motor_info[6].target_speed = 0;
		dart.motor_info[5].target_speed = 0;
	}	
}

//编码器校准=========================================
void dart_adjust(void)
{
		//pitch轴编码器置0
	  if(get_switch(0) == 1)
	  {
		  dart.motor_info[6].absolute_angle = 0;
		  dart.motor_info[5].absolute_angle = 0;
	  }
//		if(get_switch(0)==0 && dart_adjust_falg[0] == 0)
//		{
//			dart.motor_info[6].target_speed = 200;
//			dart.motor_info[5].target_speed = 200; 
//		}
//		else
//		{
//			dart.motor_info[6].target_speed   = 0;
//			dart.motor_info[5].target_speed   = 0;
//			dart.motor_info[6].absolute_angle = 0;
//			dart.motor_info[5].absolute_angle = 0;
//			dart_adjust_falg[0] = 1;
//		}
		
		//yaw轴编码置0
		
	   if(get_switch(1) == 1)
	   {
				dart.motor_info[7].absolute_angle = 0;
	   }
//		if(get_switch(1)==0 && dart_adjust_falg[1] == 0)
//		{
//			dart.motor_info[7].target_speed = 200;
//		}
//		else
//		{
//			dart.motor_info[7].target_speed = 0;
//			dart.motor_info[7].absolute_angle = 0;
//			dart_adjust_falg[1] = 1;
//		}

}
//PID计算==============================================
void calc_and_send(void)
{
	for(uint16_t index = 0;	index<8;index++)
	{
			dart.motor_info[index].set_voltage = pid_calc(&dart.motor_speed_pid[index],
		                                                 dart.motor_info[index].target_speed,
		                                                 dart.motor_info[index].rotor_speed);
	}
	
	can2_cmd_motor(1,dart.motor_info[4].set_voltage,
	                 dart.motor_info[5].set_voltage,
	                 dart.motor_info[6].set_voltage,
	                 dart.motor_info[7].set_voltage);

}



//暂时没用
//void dart_pos_mode()
//{
//	  dart.motor_info[0].target_pos   = 8191/2 + 3*rc_ctrl.rc.ch[0];
//		dart.motor_info[0].target_speed = pos_pid_calc(&dart.motor_pos_pid[0],
//	                                          dart.motor_info[0].target_pos,
//	                                          dart.motor_info[0].absolute_angle);	   
//}
