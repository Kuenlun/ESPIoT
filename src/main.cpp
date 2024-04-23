#include <esp_log.h>

#include "secrets.h"
// #include "task_class.h"

#include "wifi_class.h"
#include "mqtt_class.h"

namespace ESPIoT
{

    // Tag used for the logging system
    static constexpr char LOG_TAG[]{"Main"};

    // Handle for the main task (used to notify it from other tasks)
    // static TaskHandle_t maint_task_handle{nullptr};

    extern "C" void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
        esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
        esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
        esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
        esp_log_level_set("transport", ESP_LOG_VERBOSE);
        esp_log_level_set("outbox", ESP_LOG_VERBOSE);
        esp_log_level_set("wifi", ESP_LOG_VERBOSE);

        ESPIoT::WiFi wifi(WIFI_SSID, WIFI_PASS);
        esp_log_level_set(ESPIoT::WiFi::LOG_TAG, ESP_LOG_VERBOSE);
        vTaskDelay(pdMS_TO_TICKS(10 * 1000));

        ESPIoT::MQTT mqtt("mqtt://broker.hivemq.com");
        esp_log_level_set(ESPIoT::MQTT::LOG_TAG, ESP_LOG_VERBOSE);

        int msg_id = esp_mqtt_client_subscribe_single(mqtt.get_client(), "/topic/ESPReceiver", 0);
        ESP_LOGI(LOG_TAG, "sent subscribe successful, msg_id=%d", msg_id);

        while (true)
        {
            msg_id = esp_mqtt_client_publish(mqtt.get_client(), "/topic/ESPSender", "Test Data", 0, 0, 0);
            ESP_LOGI(LOG_TAG, "sent publish successful, msg_id=%d", msg_id);

            vTaskDelay(pdMS_TO_TICKS(10 * 1000));
        }
    }

} // namespace ESPIoT
