#include "stm32f4xx.h"                  // Device header
#include "Friction_task.h" 
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
fp32 motor_speed_pid_2006[3]={10,0,0};
fp32 motor_pos_pid_2006[3] = {1,0,1};
//标志位
uint8_t Friction_adjust_flag[4] = {0};
uint8_t Last_Stable = 0 ;
//斜坡速度
float ramp[2] = {0};
//函数声明==============================================
static void Friction_init();
static void Friction_adjust();
static void mode_choose(); 
static void calc_and_send();
static void magazine_task();
static void speed_ramp();
//主函数==============================================
void Friction_task(void const * argument)
{
	Friction_init();
  for(;;)
  {
		Friction_adjust();//编码值校准
		magazine_task();  //弹仓切换
		mode_choose();    //模式选择
		speed_ramp();     //摩擦轮斜坡启动
		calc_and_send();  //pid计算和发送
    osDelay(1);
  }

}

//初始化===============================================
void Friction_init()
{
	  for (uint8_t i = 0; i < 4; i++)
	{
        pid_init(&dart.motor_speed_pid[i], motor_speed_pid_2006, 16000, 16000); //init pid parameter, kp=40, ki=3, kd=0, output limit = 16384			
	      pid_init(&dart.motor_pos_pid[i], motor_pos_pid_2006, 10000, 10000);
	}   
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET); /****这是使能5V电源供电****/
 
}


//编码值校准============================================
void Friction_adjust()
{	
	if(get_switch(2)==1)
	{
			dart.motor_info[2].absolute_angle = 0;
	}
}

//模式选择==============================================
//遥控器 左S s[1] 132   右S s[0] 132
// 左右ch[2] 上下ch[3]  左右ch[0] 上下ch[1]
void mode_choose()
{

  //3508摩擦轮控制
	if(rc_ctrl.rc.s[1] == 1)    //最高挡
	{
	   if(rc_ctrl.rc.s[0] == 1)
	   {
		   dart.motor_info[0].target_speed = -3600;
		   dart.motor_info[1].target_speed =  3600;
	   }
	   else if(rc_ctrl.rc.s[0] == 3)
	   {
	     dart.motor_info[0].target_speed = -3590;
		   dart.motor_info[1].target_speed =  3590;	
	   }
	   else if(rc_ctrl.rc.s[0] == 2)
	   { 
		   dart.motor_info[0].target_speed = -3580;
		   dart.motor_info[1].target_speed =  3580;		
   	 }
  }
	else if(rc_ctrl.rc.s[1] == 3) //中间挡
	{
	   if(rc_ctrl.rc.s[0] == 1)
	   {
		   dart.motor_info[0].target_speed = -3100;
		   dart.motor_info[1].target_speed =  3100;
	   }
	   else if(rc_ctrl.rc.s[0] == 3)
	   {
	     dart.motor_info[0].target_speed = -3000;
		   dart.motor_info[1].target_speed =  3000;	
	   }
	   else if(rc_ctrl.rc.s[0] == 2)
	   { 
		   dart.motor_info[0].target_speed = -2900;
		   dart.motor_info[1].target_speed =  2900;		
   	 }
  }
	else  //停转
	{
		   dart.motor_info[0].target_speed = 0;
		   dart.motor_info[1].target_speed = 0;		
	}
	
	//2006推杆控制->速度控制
		if(rc_ctrl.rc.ch[3] > 110 && dart.motor_info[2].torque_current > -4000 && get_switch(3) == 0)
		{
			if((3800 <dart.motor_info[2].absolute_angle && dart.motor_info[2].absolute_angle < 4100)||dart.motor_info[2].absolute_angle > 6900)
			{ 
				dart.motor_info[2].target_speed = -5000;
			}
			else
			{
				dart.motor_info[2].target_speed = -30000;
			}
		}
		else if(rc_ctrl.rc.ch[3] < -110 && dart.motor_info[2].torque_current < 4500 && get_switch(2) == 0)
		{
			dart.motor_info[2].target_speed =  30000;
		}
		else
		{
			dart.motor_info[2].target_speed = 0;
		}
}

//弹仓切换-》遥控器左边拨杆左右拨动切换弹舱
void magazine_task()
{
  if(rc_ctrl.rc.ch[2] < -550 && abs(dart.motor_info[3].torque_current)<5000)
	{
		dart.motor_info[3].target_speed =  1000;
	}
	else if (rc_ctrl.rc.ch[2] > 550 && abs(dart.motor_info[3].torque_current)<5000)
	{
		dart.motor_info[3].target_speed = -1000;
	}
	else
	{
		dart.motor_info[3].target_speed = 0;
	}

}

////弹仓切换-》每次-往上拨即可切换弹仓
//void magazine_task()
//{
//	//右弹仓
//	if(rc_ctrl.rc.s[0] == 1 && Friction_adjust_flag[3]==0)
//	{
//			if(Last_Stable == 1)
//			{
//						if(abs(dart.motor_info[3].torque_current)<5000 )
//	          {
//		          dart.motor_info[3].target_speed = 1000;
//	          }
//	          else
//	         {
//		          dart.motor_info[3].target_speed   = 0;
//		          Friction_adjust_flag[3] = 1;
//			        Last_Stable = 0;
//	         }
//			}
//			else if(Last_Stable == 0)
//			{
//						if(abs(dart.motor_info[3].torque_current)<5000 )
//	          {
//		          dart.motor_info[3].target_speed = -1000;
//	          }
//	          else
//	         {
//		          dart.motor_info[3].target_speed   = 0;
//		          Friction_adjust_flag[3] = 1;
//			        Last_Stable = 1;
//	         }
//			}

//	}
//	//标志位清0
//	if(rc_ctrl.rc.s[0] == 3)
//	{
//		Friction_adjust_flag[3] = 0;
//	}

//}
//PID计算==============================================
void calc_and_send()
{
	for(uint16_t index = 0;	index<2;index++)
	{
			dart.motor_info[index].set_voltage = pid_calc(&dart.motor_speed_pid[index],
		                                                 ramp[index],
		                                                 dart.motor_info[index].rotor_speed);
	}
	for(uint16_t index = 2;	index<4;index++)
	{
			dart.motor_info[index].set_voltage = pid_calc(&dart.motor_speed_pid[index],
		                                                 dart.motor_info[index].target_speed,
		                                                 dart.motor_info[index].rotor_speed);
	}
	can1_cmd_motor(0,dart.motor_info[0].set_voltage,
	                 dart.motor_info[1].set_voltage,
	                 dart.motor_info[2].set_voltage,
	                 dart.motor_info[3].set_voltage);

}
//斜坡函数
static void speed_ramp()
{
	int8_t start = 1;
	int8_t slow = 1;

	if(rc_ctrl.rc.s[1] == 1||rc_ctrl.rc.s[1] == 3)
	{
		 ramp[1] = ramp[1] + start;
	}
	else
	{
		 ramp[1] = ramp[1] - slow;
	}
	
	if(rc_ctrl.rc.s[1] == 1||rc_ctrl.rc.s[1] == 3)
	{
		 ramp[0] = ramp[0] - start;
	}
	else
	{
		 ramp[0] = ramp[0] + slow;
	}
	
	if(ramp[1]>dart.motor_info[1].target_speed)
	{
		 ramp[1] = dart.motor_info[1].target_speed;
	}
	else if(ramp[1] < 0)
	{
		 ramp[1] = 0;
	}
	
	if(ramp[0]< dart.motor_info[0].target_speed)
	{
		 ramp[0] = dart.motor_info[0].target_speed;
	}
	else if(ramp[0] > 0)
	{
		 ramp[0] = 0;
	}

}