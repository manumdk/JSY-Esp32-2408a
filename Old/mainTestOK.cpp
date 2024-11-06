#include <Arduino.h>
#include <ArduinoJson.h>
#include <MycilaJSY.h>

Mycila::JSY jsy;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  // read JSY on pins 17 (JSY RX / Serial TX) and 16 (JSY TX / Serial RX)
  jsy.begin(Serial2, 15, 13);
 if (jsy.isEnabled()) {
    Mycila::JSY::BaudRate max = jsy.getMaxAvailableBaudRate();
    if (jsy.setBaudRate(max)) {
      Serial.println("JSY baud rate updated");
    } else {
      Serial.println("JSY baud rate update failed");
    }
  } else {
    Serial.println("JSY is disabled");
  }
}

void loop() {
  uint32_t now = millis();
  if (jsy.read()) {
    Serial.print("JSY read in ");
    Serial.print(millis() - now);
    Serial.print(" ms\n");

    JsonDocument doc;
    jsy.toJson(doc.to<JsonObject>());
    serializeJsonPretty(doc, Serial);
    Serial.println();
  }
  delay(1000);
}