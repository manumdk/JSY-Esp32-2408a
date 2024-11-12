//#include <ArduinoJson.h>

// Pin: RX (ESP32) -> TX (JSY)
#define JSY_TX_PIN 15

// Pin: TX (ESP32) -> RX (JSY)
#define JSY_RX_PIN 13

// Pin: ZC (ESP32)
#define ZC_PIN 38
#define PIN_SSR gpio_num_t::GPIO_NUM_26

// Minimum delay to reach the voltage required for a gate current of 30mA.
// delay_us = asin((gate_resistor * gate_current) / grid_volt_max) / pi * period_us
// delay_us = asin((330 * 0.03) / 325) / pi * 10000 = 97us
#define PHASE_DELAY_MIN_US 100

// #include <MycilaDimmer.h>                   /// the corrected librairy  in RBDDimmer-master-corrected.rar , the original has a bug
// Mycila::Dimmer dimmer_hard(); // initialase port for dimmer for ESP8266, ESP32, Arduino due boards

#include <RBDdimmer.h> /// the corrected librairy  in RBDDimmer-master-corrected.rar , the original has a bug

dimmerLamp dimmer_hard(PIN_SSR, ZC_PIN); // initialase port for dimmer for ESP8266, ESP32, Arduino due boards
int dimmer_security = 100;                    // coupe le dimmer toute les X minutes en cas de probleme externe.
int dimmer_security_count = 0;

void Dimmer_setup() // Configuration du Dimmer
{
  // configuration dimmer
  dimmer_hard.begin(NORMAL_MODE, ON); // dimmer initialisation: name.begin(MODE, STATE)
  dimmer_hard.setPower(0);
  dimmer_hard.setState(OFF);
  Serial.println("Dimmer started...");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  pinMode(PIN_SSR, OUTPUT);

  Dimmer_setup();
}

uint32_t lastTime = 0;
size_t i = 0;
float dutyCycles[5] = {0, 0.25, 0.50, 0.75, 1};
void loop()
{
  if (millis() - lastTime > 2000)
  {

    Serial.printf("Duty cycle: %f, \n", dutyCycles[i]);

    dimmer_hard.setPower(i * 100);
    if (i < 4)
    {
      i++;
    }
    else
    {
      i = 0;
    }
  }

  lastTime = millis();
}
