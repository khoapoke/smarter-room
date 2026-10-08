#include <Arduino.h>
#include <Wire.h>
#include <DHTesp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

DHTesp dht;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

struct EnvironmentData {
    float temperature = NAN;
    float humidity = NAN;
    bool dark = false;
    bool motion = false;
    bool gasAlert = false;
};

EnvironmentData env;

bool fanOn = false;
bool lightOn = false;
bool alarmOn = false;

unsigned long lastSensorRead = 0;

// -------------------------
// HELPER FUNCTIONS
// -------------------------

void setRelay(uint8_t pin, bool on) {
    if (RELAY_ACTIVE_HIGH) {
        digitalWrite(pin, on ? HIGH : LOW);
    } else {
        digitalWrite(pin, on ? LOW : HIGH);
    }
}

void readSensors() {
    TempAndHumidity dhtData = dht.getTempAndHumidity();

    env.temperature = dhtData.temperature;
    env.humidity = dhtData.humidity;

    // Wokwi Photoresistor DO: HIGH = dark, LOW = bright.
    env.dark = (digitalRead(LDR_PIN) == HIGH);

    env.motion = (digitalRead(PIR_PIN) == HIGH);

    // Wokwi MQ2 DO: LOW = gas concentration exceeds threshold.
    env.gasAlert = (digitalRead(MQ2_PIN) == LOW);
}

void applyAutomation() {
    // -------- FAN --------
    // Start fan when temperature/humidity is too high or gas is detected.
    bool fanDemand = false;

    if (isnan(env.temperature) || isnan(env.humidity)) {
        // If DHT22 fails, don't make a decision from invalid values.
        fanDemand = env.gasAlert;
    } else {
        if (env.temperature >= FAN_TEMP_ON || env.humidity >= FAN_HUM_ON || env.gasAlert) {
            fanDemand = true;
        }

        // Hysteresis: keep the fan OFF only when the environment is safely below
        // the lower thresholds and there is no gas alarm.
        if (fanOn && env.temperature > FAN_TEMP_OFF) {
            fanDemand = true;
        }

        if (fanOn && env.humidity > FAN_HUM_OFF) {
            fanDemand = true;
        }
    }

    fanOn = fanDemand;

    // -------- LIGHT --------
    // Turn the room light on only when it is dark AND movement is detected.
    lightOn = env.dark && env.motion;

    // -------- ALARM --------
    bool overTemperature = !isnan(env.temperature) && env.temperature >= ALARM_TEMP;
    alarmOn = env.gasAlert || overTemperature;

    setRelay(FAN_RELAY_PIN, fanOn);
    setRelay(LIGHT_RELAY_PIN, lightOn);

    if (alarmOn) {
        tone(BUZZER_PIN, 2000);
    } else {
        noTone(BUZZER_PIN);
    }
}

void updateDisplay() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.print("ENVIRONMENT MONITOR");

    display.setCursor(0, 8);
    display.print("Temp: ");
    if (isnan(env.temperature)) {
        display.print("ERR");
    } else {
        display.print(env.temperature, 1);
        display.print(" C");
    }

    display.setCursor(0, 16);
    display.print("Hum : ");
    if (isnan(env.humidity)) {
        display.print("ERR");
    } else {
        display.print(env.humidity, 1);
        display.print(" %");
    }

    display.setCursor(0, 24);
    display.print("Room: ");
    display.print(env.dark ? "DARK" : "BRIGHT");

    display.setCursor(0, 32);
    display.print("Motion: ");
    display.print(env.motion ? "YES" : "NO");

    display.setCursor(0, 40);
    display.print("Gas: ");
    display.print(env.gasAlert ? "ALERT" : "SAFE");

    display.setCursor(0, 48);
    display.print("Fan:");
    display.print(fanOn ? "ON " : "OFF");
    display.print(" Light:");
    display.print(lightOn ? "ON" : "OFF");

    display.setCursor(0, 56);
    display.print("Alarm: ");
    display.print(alarmOn ? "ON" : "OFF");

    display.display();
}

void printStatus() {
    Serial.println("==============================");
    Serial.println("ENVIRONMENT MONITORING");

    Serial.print("Temperature: ");
    if (isnan(env.temperature)) Serial.println("ERROR");
    else {
        Serial.print(env.temperature, 1);
        Serial.println(" C");
    }

    Serial.print("Humidity   : ");
    if (isnan(env.humidity)) Serial.println("ERROR");
    else {
        Serial.print(env.humidity, 1);
        Serial.println(" %");
    }

    Serial.print("Room       : ");
    Serial.println(env.dark ? "DARK" : "BRIGHT");

    Serial.print("Motion     : ");
    Serial.println(env.motion ? "YES" : "NO");

    Serial.print("Gas        : ");
    Serial.println(env.gasAlert ? "ALERT" : "SAFE");

    Serial.print("Fan        : ");
    Serial.println(fanOn ? "ON" : "OFF");

    Serial.print("Light      : ");
    Serial.println(lightOn ? "ON" : "OFF");

    Serial.print("Alarm      : ");
    Serial.println(alarmOn ? "ON" : "OFF");
}

void setup() {
    Serial.begin(115200);

    // Sensor inputs
    pinMode(LDR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(MQ2_PIN, INPUT);

    // Actuator outputs
    pinMode(FAN_RELAY_PIN, OUTPUT);
    pinMode(LIGHT_RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    setRelay(FAN_RELAY_PIN, false);
    setRelay(LIGHT_RELAY_PIN, false);
    noTone(BUZZER_PIN);

    // DHT22
    dht.setup(DHT_PIN, DHTesp::DHT22);

    // OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("ERROR: OLED not found");
    } else {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Environment System");
        display.println("Starting...");
        display.display();
    }

    delay(1500);
}

void loop() {
    unsigned long now = millis();

    if (now - lastSensorRead >= SENSOR_INTERVAL_MS || lastSensorRead == 0) {
        lastSensorRead = now;

        readSensors();
        applyAutomation();
        updateDisplay();
        printStatus();
    }
}
