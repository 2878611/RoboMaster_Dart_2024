#include "can_user.h"
#include "main.h"
#include "motor.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

HAL_StatusTypeDef flag111;

//3508标识符 0x200 0x1FF   反馈：0x200+ID
//2006标识符 0x200 0x1FF   反馈：0x200+ID
//6020标识符 0x1FF 0x2FF   反馈：0x204+ID
#define MOTOR_MAX_NUM 8
#define FEEDBACK_ID_BASE         0x201
#define FEEDBACK_ID_BASE_6020    0x205
#define CAN_CONTROL_ID_BASE      0x200
#define CAN_CONTROL_ID_EXTEND    0x1ff

CAN_RxHeaderTypeDef can1_rx_header;
uint8_t can1_rx_data[8];
CAN_RxHeaderTypeDef can2_rx_header;
uint8_t can2_rx_data[8];

		
static CAN_TxHeaderTypeDef  motor_tx_message;
static uint8_t              motor_can_send_data[8];

//motor data read
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1)
    {
      HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can1_rx_header, can1_rx_data);

        if ((can1_rx_header.StdId >= FEEDBACK_ID_BASE)                    // 201-207
            && (can1_rx_header.StdId < FEEDBACK_ID_BASE + MOTOR_MAX_NUM)) // 判断标识符，标识符为0x200+ID
        {
            uint8_t index = can1_rx_header.StdId - FEEDBACK_ID_BASE; // get motor index by can_id
            dart.motor_info[index].rotor_angle = ((can1_rx_data[0] << 8) | can1_rx_data[1]);
            dart.motor_info[index].rotor_speed = ((can1_rx_data[2] << 8) | can1_rx_data[3]);
            dart.motor_info[index].torque_current = ((can1_rx_data[4] << 8) | can1_rx_data[5]);
            dart.motor_info[index].temp = can1_rx_data[6];
        }if ((can1_rx_header.StdId >= FEEDBACK_ID_BASE_6020)                    // 205-211,注意把ID调成大于3,不然就会和读取3508的函数产生冲突
            && (can1_rx_header.StdId < FEEDBACK_ID_BASE_6020 + MOTOR_MAX_NUM)) // 判断标识符，标识符为0x204+ID
        {
            uint8_t index = can1_rx_header.StdId - FEEDBACK_ID_BASE_6020; // get motor index by can_id
            dart.motor_info[index].rotor_angle = ((can1_rx_data[0] << 8) | can1_rx_data[1]);
            dart.motor_info[index].rotor_speed = ((can1_rx_data[2] << 8) | can1_rx_data[3]);
            dart.motor_info[index].torque_current = ((can1_rx_data[4] << 8) | can1_rx_data[5]);
            dart.motor_info[index].temp = can1_rx_data[6];
        }
			}
    if (hcan->Instance == CAN2)
    {
       HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can2_rx_header, can2_rx_data);
        if ((can2_rx_header.StdId >= FEEDBACK_ID_BASE)                    // 201-207
            && (can2_rx_header.StdId < FEEDBACK_ID_BASE + MOTOR_MAX_NUM)) // 判断标识符，标识符为0x200+ID
        {
            uint8_t index = can2_rx_header.StdId - FEEDBACK_ID_BASE + 4; // get motor index by can_id
            dart.motor_info[index+4].rotor_angle = ((can2_rx_data[0] << 8) | can2_rx_data[1]);
            dart.motor_info[index+4].rotor_speed = ((can2_rx_data[2] << 8) | can2_rx_data[3]);
            dart.motor_info[index+4].torque_current = ((can2_rx_data[4] << 8) | can2_rx_data[5]);
            dart.motor_info[index+4].temp = can2_rx_data[6];
        }if ((can2_rx_header.StdId >= FEEDBACK_ID_BASE_6020)                    // 205-211,注意把ID调成大于3,不然就会和读取3508的函数产生冲突
            && (can2_rx_header.StdId < FEEDBACK_ID_BASE_6020 + MOTOR_MAX_NUM)) // 判断标识符，标识符为0x204+ID
        {
            uint8_t index = can2_rx_header.StdId - FEEDBACK_ID_BASE_6020; // get motor index by can_id
            dart.motor_info[index+4].rotor_angle = ((can2_rx_data[0] << 8) | can2_rx_data[1]);
            dart.motor_info[index+4].rotor_speed = ((can2_rx_data[2] << 8) | can2_rx_data[3]);
            dart.motor_info[index+4].torque_current = ((can2_rx_data[4] << 8) | can2_rx_data[5]);
            dart.motor_info[index+4].temp = can2_rx_data[6];
        }
			}
}


void can1_cmd_motor(uint8_t id_range,int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{

    uint32_t send_mail_box;
    motor_tx_message.StdId = (id_range == 0)?(0x200):(0x1ff);
    motor_tx_message.IDE = CAN_ID_STD;
    motor_tx_message.RTR = CAN_RTR_DATA;  
    motor_tx_message.DLC = 0x08;  
    motor_can_send_data[0] = ( motor1 >> 8);
    motor_can_send_data[1] =  motor1;
    motor_can_send_data[2] = ( motor2 >> 8);
    motor_can_send_data[3] =  motor2;
    motor_can_send_data[4] = ( motor3 >> 8);  
    motor_can_send_data[5] = motor3;
    motor_can_send_data[6] = (motor4 >> 8);
    motor_can_send_data[7] = motor4;
    HAL_CAN_AddTxMessage(&hcan1, &motor_tx_message, motor_can_send_data, &send_mail_box);
}

void can2_cmd_motor(uint8_t id_range,int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{

    uint32_t send_mail_box;
    motor_tx_message.StdId = (id_range == 0)?(0x200):(0x1ff);
    motor_tx_message.IDE = CAN_ID_STD;
    motor_tx_message.RTR = CAN_RTR_DATA;  
    motor_tx_message.DLC = 0x08;  
    motor_can_send_data[0] = ( motor1 >> 8);
    motor_can_send_data[1] =  motor1;
    motor_can_send_data[2] = ( motor2 >> 8);
    motor_can_send_data[3] =  motor2;
    motor_can_send_data[4] = ( motor3 >> 8);  
    motor_can_send_data[5] = motor3;
    motor_can_send_data[6] = (motor4 >> 8);
    motor_can_send_data[7] = motor4;
    HAL_CAN_AddTxMessage(&hcan2, &motor_tx_message, motor_can_send_data, &send_mail_box);
}




