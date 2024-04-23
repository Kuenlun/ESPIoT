#include "wifi_class.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

namespace ESPIoT
{

    /* FreeRTOS event group to signal when we are connected*/
    static EventGroupHandle_t s_wifi_event_group{};
    /* The event group allows multiple bits for each event, but we only care about two events:
     * - we are connected to the AP with an IP
     * - we failed to connect after the maximum amount of retries */
    static constexpr uint32_t WIFI_CONNECTED_BIT{BIT0};
    static constexpr uint32_t WIFI_FAIL_BIT{BIT1};

    void WiFi::initialize_nvs()
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        ESP_LOGI(LOG_TAG, "NVS initialized");
    }

    void WiFi::event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
    {
        static uint8_t s_retry_num = 0;
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
            {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(LOG_TAG, "retry to connect to the AP");
            }
            else
            {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(LOG_TAG, "connect to the AP fail");
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(LOG_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }

    void WiFi::wifi_init_sta()
    {
        ESP_ERROR_CHECK(esp_netif_init());

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &event_handler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &event_handler,
                                                            NULL,
                                                            &instance_got_ip));

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_LOGI(LOG_TAG, "wifi_init_sta finished.");
    }

    WiFi::WiFi(const char *const ssid, const char *const pass)
    {
        // Initialize NVS
        initialize_nvs();

        s_wifi_event_group = xEventGroupCreate();
        wifi_init_sta();

        strcpy((char *)m_wifi_config.sta.ssid, ssid);
        strcpy((char *)m_wifi_config.sta.password, pass);

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &m_wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
         * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                               WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
         * happened. */
        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(LOG_TAG, "connected to ap SSID:%s password:%s", ssid, pass);
        }
        else if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGI(LOG_TAG, "Failed to connect to SSID:%s, password:%s", ssid, pass);
        }
        else
        {
            ESP_LOGE(LOG_TAG, "UNEXPECTED EVENT");
        }
    }

} // namespace ESPIoT
