#include <Arduino.h>
#include <ArduinoSTL.h>

constexpr uint8_t PWD_PIN_COUNT = 4;
constexpr uint8_t GND_PIN_COUNT = 2;

struct led_cube_t{
  uint8_t  configuration{};
  uint8_t pins_pwd[PWD_PIN_COUNT];
  uint8_t pins_gnd[GND_PIN_COUNT];
  bool need_sync{true};

  led_cube_t(uint8_t *pwd, uint8_t *gnd);

  void set_pin_mode();
  void set_state(uint8_t state);
  void sync();
};


led_cube_t::led_cube_t(uint8_t *pwd, uint8_t *gnd){
  for(int i = 0; i < PWD_PIN_COUNT; ++i){
    pins_pwd[i] = pwd[i];
  }
  for(int i = 0; i < GND_PIN_COUNT; ++i){
    pins_gnd[i] = gnd[i];
  }
}

void led_cube_t::set_pin_mode(){
  for(int i = 0; i < PWD_PIN_COUNT; ++i){
    pinMode(pins_pwd[i], OUTPUT);
  }
  for(int i = 0; i < GND_PIN_COUNT; ++i){
    pinMode(pins_gnd[i], OUTPUT);
  }
}

void led_cube_t::set_state(uint8_t state){
  if(configuration == state){
    return;
  }
  configuration = state;
  need_sync = true;
}

void led_cube_t::sync(){
  if(!need_sync){
    return;
  }

  for(size_t i = 0; i < PWD_PIN_COUNT; ++i){
    digitalWrite(pins_pwd[i], LOW);
  }
  for(size_t i = 0; i < GND_PIN_COUNT; ++i){
    digitalWrite(pins_gnd[i], HIGH);
  }

  bool flag;
  for(size_t layer = 0; layer < GND_PIN_COUNT; ++layer){
    flag = false;
    for(size_t i = 0; i < PWD_PIN_COUNT; ++i){
      size_t led   = layer * PWD_PIN_COUNT + i;
      size_t mask  = (1 << led);
      size_t state = (configuration & mask) != 0;

      if(state){
        flag = true;
        digitalWrite(pins_pwd[i], HIGH);
      }
    }
    
    if(flag){
      digitalWrite(pins_gnd[layer], LOW);
    }
  }
  need_sync = false;
}

constexpr uint8_t states[] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00000001,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00010000,
};

size_t current_state{};
constexpr size_t state_count = sizeof(states) / sizeof(uint8_t);

size_t last_time{};
size_t operation_duration{};
uint8_t pin_pwd[] = {9, 10, 11, 12};
uint8_t pin_gnd[] = {5, 6};

led_cube_t cube = led_cube_t(pin_pwd, pin_gnd);

void setup() {
  Serial.begin(9600);
  last_time = millis();
  cube.set_pin_mode();
}

void loop() {
  size_t current_time = millis();
  if((current_time - last_time) > (100 - operation_duration)){
    last_time = current_time;
    Serial.println(last_time);
    current_state = (current_state + 1);/// % state_count;
    cube.set_state(current_state);
    cube.sync();
    operation_duration = millis() - last_time;
  }
}

