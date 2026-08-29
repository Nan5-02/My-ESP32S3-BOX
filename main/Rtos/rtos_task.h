#ifndef __RTOS_TASK_H__
#define __RTOS_TASK_H__

// 任务栈大小
#define WIFI_TASK_STACK_SIZE      (4096)
#define MAIN_TASK_STACK_SIZE      (4096)
// 任务优先级
#define WIFI_TASK_PRIO          (5)
#define MAIN_TASK_PRIO          (5)

void Rtos_Task_Create(void);

#endif /* __RTOS_TASK_H__ */
