#pragma once

#include <mqtt_client.h>

namespace ESPIoT
{

    class MQTT
    {
    public:
        // Tag used for the logging system
        static constexpr char LOG_TAG[]{"MQTT"};

    public:
        MQTT(const char *const uri);

        inline esp_mqtt_client_handle_t get_client() const { return m_client; };

    private:
        static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                                       int32_t event_id, void *event_data);

        static void log_error_if_nonzero(const char *message, int error_code);

    private:
        esp_mqtt_client_handle_t m_client{};
    };

} // namespace ESPIoT
