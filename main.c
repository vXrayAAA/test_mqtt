#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "mqtt_client.h"

// Definição das credenciais do Wi-Fi e do broker MQTT
#define WIFI_SSID "sua_rede_wifi"
#define WIFI_PASS "sua_senha_wifi"
#define MQTT_URI "mqtt://endereco_ip_do_broker_mqtt:1883"
#define MQTT_USER "seu_usuario_mqtt"
#define MQTT_PASS "sua_senha_mqtt"
#define MQTT_TOPIC "seu_topico_mqtt"

static const char *TAG = "MQTT_EXAMPLE";

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // Caso de evento de conexão com o broker MQTT
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Conectado ao broker MQTT!");
            // Subscrevendo ao tópico MQTT
            msg_id = esp_mqtt_client_subscribe(client, MQTT_TOPIC, 0);
            ESP_LOGI(TAG, "Subscrevendo ao tópico %s, msg_id = %d", MQTT_TOPIC, msg_id);
            break;
        // Caso de evento de desconexão do broker MQTT
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado do broker MQTT!");
            break;
        // Caso de evento de mensagem recebida no tópico MQTT
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Mensagem recebida no tópico %.*s: %.*s", event->topic_len, event->topic, event->data_len, event->data);
            break;
        default:
            ESP_LOGI(TAG, "Evento MQTT não tratado: %d", event->event_id);
            break;
    }
    return ESP_OK;
}

void wifi_init_sta()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t sta_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
    esp_wifi_start();
}

void mqtt_app_start()
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
        .username = MQTT_USER,
        .password = MQTT_PASS,
        .event_handle = mqtt_event_handler_cb,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void app_main()
{
    ESP_LOGI(TAG, "Iniciando aplicação...");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "Inicializando o Wi-Fi...");
    wifi_init_sta();
    ESP_LOGI(TAG, "Inicializando o MQTT...");
    mqtt_app_start();
}
