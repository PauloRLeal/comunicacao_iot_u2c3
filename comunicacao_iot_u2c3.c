#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define WIFI_SSID "Paulo"
#define WIFI_PASSWORD "paulo1243"
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80
#define API_KEY "8KPDDIZMG9L6F3W5"
#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define SENSOR_PIN 28
#define DHT_PIN 16

// Globais
int btn_a_state, btn_b_state;
float x_position, y_position;
float sensor_data = 0.0; // umidade do DHT22
float temperatura = 0.0; // temperatura do DHT22
char wind_rose_direction[20] = "Centro";

// ---------- Funções Auxiliares ----------
void read_buttons()
{
    btn_a_state = !gpio_get(BTN_A_PIN);
    btn_b_state = !gpio_get(BTN_B_PIN);
}

void calculate_wind_direction(float x, float y)
{
    const float threshold = 0.6;
    const float margin = 0.2;

    if (fabs(x) <= margin && fabs(y) <= margin)
    {
        strcpy(wind_rose_direction, "Centro");
    }
    else if (y >= threshold && fabs(x) <= margin)
    {
        strcpy(wind_rose_direction, "Norte");
    }
    else if (y <= -threshold && fabs(x) <= margin)
    {
        strcpy(wind_rose_direction, "Sul");
    }
    else if (x >= threshold && fabs(y) <= margin)
    {
        strcpy(wind_rose_direction, "Leste");
    }
    else if (x <= -threshold && fabs(y) <= margin)
    {
        strcpy(wind_rose_direction, "Oeste");
    }
    else if (y >= threshold && x >= threshold)
    {
        strcpy(wind_rose_direction, "Nordeste");
    }
    else if (y >= threshold && x <= -threshold)
    {
        strcpy(wind_rose_direction, "Noroeste");
    }
    else if (y <= -threshold && x >= threshold)
    {
        strcpy(wind_rose_direction, "Sudeste");
    }
    else if (y <= -threshold && x <= -threshold)
    {
        strcpy(wind_rose_direction, "Sudoeste");
    }
    else
    {
        strcpy(wind_rose_direction, "Centro");
    }
}

int direction_to_number(const char *dir)
{
    if (strcmp(dir, "Centro") == 0)
        return 0;
    if (strcmp(dir, "Norte") == 0)
        return 1;
    if (strcmp(dir, "Sul") == 0)
        return 2;
    if (strcmp(dir, "Leste") == 0)
        return 3;
    if (strcmp(dir, "Oeste") == 0)
        return 4;
    if (strcmp(dir, "Nordeste") == 0)
        return 5;
    if (strcmp(dir, "Noroeste") == 0)
        return 6;
    if (strcmp(dir, "Sudeste") == 0)
        return 7;
    if (strcmp(dir, "Sudoeste") == 0)
        return 8;
    return -1;
}

bool read_dht22_sensor(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t data[5] = {0};
    int i, j;

    // Inicia comunicação com pulso baixo de 18ms
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(18);
    gpio_put(DHT_PIN, 1);
    sleep_us(40);
    gpio_set_dir(DHT_PIN, GPIO_IN);

    // Aguarda resposta do sensor (nível baixo e depois alto)
    uint32_t timeout = 10000;
    while (gpio_get(DHT_PIN) == 0 && timeout--)
        ;
    if (timeout == 0)
        return false;

    timeout = 10000;
    while (gpio_get(DHT_PIN) == 1 && timeout--)
        ;
    if (timeout == 0)
        return false;

    // Leitura dos 40 bits (5 bytes)
    for (i = 0; i < 5; i++)
    {
        for (j = 7; j >= 0; j--)
        {
            // Espera pelo início do bit
            timeout = 10000;
            while (gpio_get(DHT_PIN) == 0 && timeout--)
                ;
            if (timeout == 0)
                return false;

            sleep_us(30); // Espera 30us para ler o bit

            if (gpio_get(DHT_PIN))
            {
                data[i] |= (1 << j);
            }

            // Aguarda fim do pulso
            timeout = 10000;
            while (gpio_get(DHT_PIN) == 1 && timeout--)
                ;
            if (timeout == 0)
                return false;
        }
    }

    // Verifica o checksum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return false;
    }

    // Monta umidade e temperatura com parte decimal
    uint16_t raw_humidity = (data[0] << 8) | data[1];
    uint16_t raw_temperature = (data[2] << 8) | data[3];

    // Verifica se a temperatura é negativa
    if (raw_temperature & 0x8000)
    {
        raw_temperature &= 0x7FFF;
        *temperature = -((float)raw_temperature) / 10.0f;
    }
    else
    {
        *temperature = ((float)raw_temperature) / 10.0f;
    }

    *humidity = ((float)raw_humidity) / 10.0f;

    return true;
}

float converterValor(float input)
{
    float min_input = 0;
    float max_input = 4096;
    float range = 200;

    // Verifica se o input está dentro dos limites
    if (input < min_input)
        input = min_input;
    if (input > max_input)
        input = max_input;

    if (input >= 1800 && input <= 2200)
        return 0.0;

    // Calcula o valor normalizado
    float output = -1.0 + 2.0 * (input - min_input) / (max_input - min_input);

    // Arredonda para os limites do range de coleta
    float steps = (max_input - min_input) / range;
    output = ((float)(output * steps + 0.5)) / (float)steps;

    if (output > 1)
        output = 1;
    else if (output < 1)
        output = -1;

    return output;
}

void read_analog()
{
    adc_select_input(1);
    x_position = converterValor(adc_read());
    adc_select_input(0);
    y_position = converterValor(adc_read());
    printf("X: %.2f, Y: %.2f", x_position, y_position);
    calculate_wind_direction(x_position, y_position);
    uint8_t temp = 0, hum = 0;
    if (read_dht22_sensor(&temp, &hum))
    {
        temperatura = (float)temp;
        sensor_data = (float)hum;
    }
    else
    {
        printf("Erro ao ler sensor DHT22\n");
        temperatura = 0.0;
        sensor_data = 0.0;
    }
}

// ---------- HTTP Request ----------
static ip_addr_t server_ip;
static volatile bool dns_done = false;

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg)
{
    if (ipaddr)
        server_ip = *ipaddr;
    dns_done = true;
}

bool resolve_dns()
{
    dns_done = false;
    err_t err = dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);
    if (err == ERR_OK)
        return true;
    absolute_time_t timeout = make_timeout_time_ms(10000);
    while (!dns_done && absolute_time_diff_us(get_absolute_time(), timeout) > 0)
    {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    return dns_done;
}

bool send_data()
{
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb)
        return false;
    if (tcp_connect(pcb, &server_ip, THINGSPEAK_PORT, NULL) != ERR_OK)
    {
        tcp_close(pcb);
        return false;
    }

    char request[512];
    int wind_dir_num = direction_to_number(wind_rose_direction);

    snprintf(request, sizeof(request),
             "GET /update?api_key=%s"
             "&field1=%d&field2=%d&field3=%.2f&field4=%.2f&field5=%d&field6=%.2f&field7=%.2f HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             API_KEY,
             btn_a_state,
             btn_b_state,
             x_position,
             y_position,
             wind_dir_num,
             sensor_data,
             temperatura,
             THINGSPEAK_HOST);

    printf("Pacote HTTP enviado:\n%s\n", request);

    err_t err = tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        tcp_close(pcb);
        return false;
    }

    tcp_output(pcb);
    sleep_ms(1000);
    tcp_close(pcb);
    return true;
}

// ---------- MAIN ----------
int main()
{
    stdio_init_all();
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    adc_gpio_init(SENSOR_PIN);
    gpio_init(DHT_PIN);
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 1);
    if (cyw43_arch_init())
        return 1;
    cyw43_arch_enable_sta_mode();

    printf("Conectando-se ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000))
    {
        printf("Erro ao conectar no Wi-Fi\n");
        return 1;
    }
    printf("Wi-Fi conectado!\n");

    while (true)
    {
        read_buttons();
        read_analog();

        if (resolve_dns())
        {
            bool ok = send_data();
            printf(ok ? "Dados enviados!\n" : "Erro ao enviar dados\n");
        }
        else
        {
            printf("Erro ao resolver DNS\n");
        }

        sleep_ms(15000); // Intervalo mínimo exigido pela ThingSpeak
    }

    return 0;
}
