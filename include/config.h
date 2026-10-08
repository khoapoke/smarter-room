#pragma once

// =========================
// SENSOR PINS
// =========================
constexpr uint8_t DHT_PIN   = 15;
constexpr uint8_t LDR_PIN   = 34;   // Photoresistor DO
constexpr uint8_t PIR_PIN   = 27;   // PIR OUT
constexpr uint8_t MQ2_PIN   = 33;   // MQ2 DO

// =========================
// ACTUATOR PINS
// =========================
constexpr uint8_t FAN_RELAY_PIN   = 26;
constexpr uint8_t LIGHT_RELAY_PIN = 25;
constexpr uint8_t BUZZER_PIN      = 14;

// =========================
// OLED I2C
// =========================
constexpr uint8_t OLED_SDA = 21;
constexpr uint8_t OLED_SCL = 22;
constexpr uint8_t OLED_ADDRESS = 0x3C;

constexpr int OLED_WIDTH  = 128;
constexpr int OLED_HEIGHT = 64;

// =========================
// AUTOMATION THRESHOLDS
// =========================
constexpr float FAN_TEMP_ON     = 30.0f;
constexpr float FAN_TEMP_OFF    = 28.0f;
constexpr float FAN_HUM_ON      = 70.0f;
constexpr float FAN_HUM_OFF     = 65.0f;
constexpr float ALARM_TEMP      = 35.0f;

// Wokwi MQ2 digital output is LOW when gas exceeds its configured threshold.
constexpr bool RELAY_ACTIVE_HIGH = true;

// Sensor refresh interval. DHT22 should not be polled too quickly.
constexpr unsigned long SENSOR_INTERVAL_MS = 2000;
