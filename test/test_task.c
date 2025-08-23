// test_task.c
#include "unity.h"
#include "task.h"

static int mock_task_0_call_count = 0;
static int mock_task_10_call_count = 0;

void mock_task_0(void) {
    mock_task_0_call_count++;
}

void mock_task_10(void) {
    mock_task_10_call_count++;
}

void setUp(void) {
    mock_task_0_call_count = 0;
    mock_task_10_call_count = 0;
}

void tearDown(void) {}

void test_run_scheduler_executes_zero_interval_every_time(void) {
    task_t tasks[] = {
        {0, 0, mock_task_0}
    };

    for (uint32_t tick = 0; tick < 5; tick++) {
        run_scheduler(tasks, 1, tick);
    }

    TEST_ASSERT_EQUAL(5, mock_task_0_call_count);
}

void test_run_scheduler_executes_10ms_interval_correctly(void) {
    task_t tasks[] = {
        {10, 0, mock_task_10}
    };

    run_scheduler(tasks, 1, 9);
    TEST_ASSERT_EQUAL(0, mock_task_10_call_count);

    run_scheduler(tasks, 1, 10);
    TEST_ASSERT_EQUAL(1, mock_task_10_call_count);

    run_scheduler(tasks, 1, 20);
    TEST_ASSERT_EQUAL(2, mock_task_10_call_count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_run_scheduler_executes_zero_interval_every_time);
    RUN_TEST(test_run_scheduler_executes_10ms_interval_correctly);
    return UNITY_END();
}
