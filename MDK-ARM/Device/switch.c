#include "struct_typedef.h"
#include "gpio.h"

uint16_t get_switch(uint16_t switch_id)
{
	  if(switch_id == 0) //pitch限位
		{
			return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6);
		}
	  if(switch_id == 1) //yaw限位
		{
			return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7);
		}
		if(switch_id == 2) //推杆限位
		{
			return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8);
		}
		if(switch_id == 3)
		{
			return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9);
		}
		if(switch_id == 4)//pitch限位右
		{
			return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12);
		}
		if(switch_id == 5)//yaw限位右
		{
			return HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2);
		}
		
}
