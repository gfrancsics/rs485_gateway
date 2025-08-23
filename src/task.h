// task.h
#pragma once
#include <stdint.h>

typedef struct {
    uint32_t interval;
    uint32_t last_tick;
    void (*func)(void);
} task_t;

void run_scheduler(task_t* p_task, uint8_t number_of_tasks, uint32_t tick);
