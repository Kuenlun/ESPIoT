#pragma once

#include <esp_wifi_types.h>

namespace ESPIoT
{

    class WiFi
    {
    public:
        // Tag used for the logging system
        static constexpr char LOG_TAG[]{"WiFi"};

    public:
        WiFi(const char *const ssid, const char *const pass);

    private:
        static void event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data);

    private:
        void initialize_nvs();

        void wifi_init_sta();

    private:
        static constexpr uint8_t EXAMPLE_ESP_MAXIMUM_RETRY{5};
    };

} // namespace ESPIoT
