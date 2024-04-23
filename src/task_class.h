#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace ESPIoT
{

    class TaskClass
    {
    public:
        // Tag used for the logging system
        static constexpr char LOG_TAG[]{"TaskClass"};

        inline TaskHandle_t get_task_handle() const { return m_task_handle; };

        const char *get_task_name() const;

        inline void start_task() const { vTaskResume(get_task_handle()); };

    protected:
        TaskClass(const char *const task_name,
                  const UBaseType_t task_priority,
                  const configSTACK_DEPTH_TYPE stack_depth);

        virtual void task_loop() = 0;

    private:
        TaskHandle_t m_task_handle;
    };

} // namespace ESPIoT
