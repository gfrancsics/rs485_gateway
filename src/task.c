#include "task.h"

void run_scheduler(task_t* p_task, uint8_t number_of_tasks, uint32_t tick)
{
    for (uint8_t task_index = 0; task_index < number_of_tasks; task_index++)
    {
        if (p_task[task_index].interval == 0)
        {
            p_task[task_index].func();
        }
        else if ((tick - p_task[task_index].last_tick) >= p_task[task_index].interval)
        {
            p_task[task_index].func();
            p_task[task_index].last_tick = tick;
        }
    }
}