#include <ArduinoJson.h>
#include <MycilaJSY.h>
#include <HardwareSerial.h>
#include <MycilaPulseAnalyzer.h>

#include <esp32-hal-gpio.h>
#include <hal/gpio_ll.h>
#include <soc/gpio_struct.h>

#include <esp32-hal.h>

#include <Preferences.h>



  #define PIN_OUTPUT gpio_num_t::GPIO_NUM_26


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

static void ARDUINO_ISR_ATTR wait1us() {
  uint64_t m = (uint64_t)esp_timer_get_time();
  uint64_t e = m + 1;
  if (m > e)
    while ((uint64_t)esp_timer_get_time() > e)
      NOP();
  while ((uint64_t)esp_timer_get_time() < e)
    NOP();
}

// outputs a 1 us pulse when an edge is detected
static volatile uint32_t edgeCount = 0;
static void ARDUINO_ISR_ATTR onEdge(Mycila::PulseAnalyzer::Event e, void* arg) {
  if (e == Mycila::PulseAnalyzer::Event::SIGNAL_RISING) {
    edgeCount = edgeCount + 1;
    gpio_ll_set_level(&GPIO, PIN_OUTPUT, HIGH);
    wait1us();
    gpio_ll_set_level(&GPIO, PIN_OUTPUT, LOW);
  }
  if (e == Mycila::PulseAnalyzer::Event::SIGNAL_FALLING) {
    edgeCount = edgeCount + 1;
    gpio_ll_set_level(&GPIO, PIN_OUTPUT, HIGH);
    wait1us();
    gpio_ll_set_level(&GPIO, PIN_OUTPUT, LOW);
  }
}

// outputs a 1 us pulse when the ZC event is sent
static volatile uint32_t zeroCrossCount = 0;
static void ARDUINO_ISR_ATTR onZeroCross(void* arg) {
  zeroCrossCount = zeroCrossCount + 1;
  gpio_ll_set_level(&GPIO, PIN_OUTPUT, HIGH);
  wait1us();
  gpio_ll_set_level(&GPIO, PIN_OUTPUT, LOW);
}

static void flash_operations(void* arg) {
  while (true) {
    Preferences preferences;
#if 1 // test with flash / nvm operations
    preferences.begin("crashme", false);
    preferences.putULong64("crashme", 0);
#endif
    delay(5);
  }
}

Mycila::PulseAnalyzer pulseAnalyzer;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  // jsy.setCallback([](const Mycila::JSY::EventType eventType) {
  //   if (eventType == Mycila::JSY::EventType::EVT_CHANGE) {
  //     Serial.printf(" - %" PRIu32 " EVT_CHANGE: %f V, %f A, %f W\n", (uint32_t)millis(), jsy.data.channel2().voltage, jsy.data.channel2().current, jsy.data.channel2().activePower);
  //   }
  // });

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
    pulseAnalyzer.onEdge(onEdge);
  pulseAnalyzer.onZeroCross(onZeroCross);
  pulseAnalyzer.begin(38);

  // Simulate some flash operations at the same time.
  xTaskCreate(flash_operations, "flash_op", 4096, NULL, uxTaskPriorityGet(NULL), NULL);

  
  Serial.println("################################ Fin Setup ######################################");
}


bool state = LOW;
float Tension ;
void loop() {
  state = !state;
  //digitalWrite(RELAY_PIN, state);
  Tension = jsy.data.channel2().voltage;
  Serial.printf(" - %" PRIu32 " EVT_CHANGE: %f V, %f A, %f W\n", (uint32_t)millis(), jsy.data.channel2().voltage, jsy.data.channel2().current, jsy.data.channel2().activePower);
  delay(2000);
}
