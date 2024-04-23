#include "task_class.h"

#include <cstring>

#include <esp_log.h>

namespace ESPIoT
{

    const char *TaskClass::get_task_name() const
    {
        return pcTaskGetName(m_task_handle);
    };

    TaskClass::TaskClass(const char *const task_name,
                         const UBaseType_t task_priority,
                         const configSTACK_DEPTH_TYPE stack_depth)
    {
        // Check the task name limit length
        if (strnlen(task_name, configMAX_TASK_NAME_LEN) >= configMAX_TASK_NAME_LEN)
            ESP_LOGW(LOG_TAG, "The task name '%s' will be truncated to %u characters",
                     task_name, configMAX_TASK_NAME_LEN - 1);
        // Create the FreeRTOS task function as a lambda
        const auto task = [](void *const param)
        {
            vTaskSuspend(nullptr);
            reinterpret_cast<TaskClass *>(param)->task_loop();
        };
        // Create the FreeRTOS task
        const BaseType_t ret{xTaskCreatePinnedToCore(task,
                                                     task_name,
                                                     stack_depth,
                                                     this,
                                                     task_priority,
                                                     &m_task_handle,
                                                     0)};
        assert(ret == pdPASS);
    }

} // namespace ESPIoT
