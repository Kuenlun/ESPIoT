#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace ESPIoT
{

    extern "C" void app_main(void)
    {
        while (true)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

} // namespace ESPIoT
