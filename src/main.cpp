#include <Arduino.h>
#include <ArduinoJson.h>
#include <MycilaJSY.h>

Mycila::JSY jsy;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  // read JSY on pins 17 (JSY RX / Serial TX) and 16 (JSY TX / Serial RX)
  jsy.begin(Serial2, 15, 13, true);
}

Mycila::JSYBaudRate target = Mycila::JSYBaudRate::BAUD_38400;

void loop() {
  if (!jsy.isEnabled()) {
    delay(1000);
    return;
  }

  delay(100);

  JsonDocument doc;
  jsy.toJson(doc.to<JsonObject>());
  serializeJson(doc, Serial);
  Serial.println();

  if (jsy.getBaudRate() != target) {
    jsy.setBaudRate(target);
  }
}