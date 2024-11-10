#include <ArduinoJson.h>
#include <MycilaJSY.h>

#ifndef SOC_UART_HP_NUM
#define SOC_UART_HP_NUM SOC_UART_NUM
#endif
#if SOC_UART_HP_NUM < 3
#define Serial2 Serial1
#define RX2 RX1
#define TX2 TX1
#endif

// Pin: RX (ESP32) -> TX (JSY)
#define JSY_TX_PIN 15

// Pin: TX (ESP32) -> RX (JSY)
#define JSY_RX_PIN 13

// Pin: Relay  (ESP32)
#define RELAY_PIN 26
// #define RELAY_PIN 32

Mycila::JSY jsy;
JsonDocument doc;
const Mycila::JSY::BaudRate rates[] = {
    Mycila::JSY::BaudRate::BAUD_4800,
    Mycila::JSY::BaudRate::BAUD_9600,
    Mycila::JSY::BaudRate::BAUD_19200,
    Mycila::JSY::BaudRate::BAUD_38400,
};

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  jsy.setCallback([](const Mycila::JSY::EventType eventType) {
    if (eventType == Mycila::JSY::EventType::EVT_CHANGE) {
      Serial.printf(" - %" PRIu32 " EVT_CHANGE: %f V, %f A, %f W\n", (uint32_t)millis(), jsy.data.channel2().voltage, jsy.data.channel2().current, jsy.data.channel2().activePower);
    }
  });

  jsy.begin(Serial2, JSY_TX_PIN, JSY_RX_PIN);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);


  jsy.begin(Serial2, JSY_TX_PIN, JSY_RX_PIN);
  if (jsy.getBaudRate() != Mycila::JSY::BaudRate::BAUD_38400) {
    jsy.setBaudRate(Mycila::JSY::BaudRate::BAUD_38400);
  }
  jsy.end();

  // read JSY on pins 17 (JSY RX / Serial TX) and 16 (JSY TX / Serial RX)
  jsy.begin(Serial2, JSY_TX_PIN, JSY_RX_PIN, true, 0, 38400);

  pinMode(RELAY_PIN, OUTPUT);
  Serial.println("################################ Fin Setup ######################################");
}


bool state = LOW;

void loop() {
  state = !state;
  digitalWrite(RELAY_PIN, state);
  delay(5000);
}
