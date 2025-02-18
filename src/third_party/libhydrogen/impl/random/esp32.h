// Important: RF *must* be activated on ESP board
// https://techtutorialsx.com/2017/12/22/esp32-arduino-random-number-generation/
#if (defined(CONFIG_IDF_TARGET_ESP32) || \
     defined(ESP8266) || \
     defined(CONFIG_IDF_TARGET_ESP32S2) || \
     defined(CONFIG_IDF_TARGET_ESP32S3) || \
     defined(CONFIG_IDF_TARGET_ESP32C2) || \
     defined(CONFIG_IDF_TARGET_ESP32C3) || \
     defined(CONFIG_IDF_TARGET_ESP32C5) || \
     defined(CONFIG_IDF_TARGET_ESP32C6) || \
     defined(CONFIG_IDF_TARGET_ESP32H2) || \
     defined(CONFIG_IDF_TARGET_ESP32P4))
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#endif

#if defined(ARDUINO)
#    include <Arduino.h>
#endif

static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    uint16_t         ebits = 0;

    hydro_hash_init(&st, ctx, NULL);

    while (ebits < 256) {
        uint32_t r = esp_random();

#ifdef ARDUINO
        delay(10);
#endif
#ifdef ESP32
        vTaskDelay(10);
#endif
        hydro_hash_update(&st, (const uint32_t *) &r, sizeof r);
        ebits += 32;
    }

    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}
