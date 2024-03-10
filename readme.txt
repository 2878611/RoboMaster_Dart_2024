请勿随意使用cubeMX重新生成代码，会造成函数重定义
Dart_2024\Dart_2024.axf: Error: L6200E: Symbol USART3_IRQHandler multiply defined (by remote_control.o and stm32f4xx_it.o).
Dart_2024\Dart_2024.axf: Error: L6200E: Symbol Dart_task multiply defined (by dart_task.o and freertos.o).
Dart_2024\Dart_2024.axf: Error: L6200E: Symbol Friction_task multiply defined (by friction_task.o and freertos.o).
分别涉及以下函数
void USART3_IRQHandler(void) ——将stm32f4xx_it.c文件中改为弱定义__weak void USART3_IRQHandler(void)

void Dart_task(void const * argument)——删除freertos.c中的定义
void Friction_task(void const * argument)——删除freertos.c中的定义

