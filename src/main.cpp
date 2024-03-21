#include <Arduino.h>
#include <array>
#include <bitset>


//-----------------------------------------//
//CHANGE IF LED CUBE HAS ANOTHER LEDS OF 8//
constexpr uint8_t LED_COUNT     = 8;
constexpr uint8_t PWD_PIN_COUNT = 4;
constexpr uint8_t GND_PIN_COUNT = 2;
//-----------------------------------------//

using pwd_pins_t  = std::array<uint8_t, PWD_PIN_COUNT>;
using gnd_pins_t  = std::array<uint8_t, GND_PIN_COUNT>;
using led_state_t = std::bitset<LED_COUNT>;

using it_pin_t = pwd_pins_t::iterator;



struct led_cube_t{
  led_state_t configuration{};
  pwd_pins_t  pins_pwd;
  gnd_pins_t  pins_gnd;

  led_cube_t(const pwd_pins_t &pwd, const gnd_pins_t &gnd);

  void set_pin_mode();
  void set_state(led_state_t state);

  void sync();
};


led_cube_t::led_cube_t(const pwd_pins_t &pwd, const gnd_pins_t &gnd): pins_pwd(pwd), pins_gnd(gnd){}

void led_cube_t::set_pin_mode(){
  for(auto &pin: pins_pwd){
    pinMode(pin, OUTPUT);
  }
  for(auto &pin: pins_gnd){
    pinMode(pin, OUTPUT);
  }

}

void led_cube_t::set_state(led_state_t state){
  if(configuration == state){
    return;
  }
  configuration = state;
}

void led_cube_t::sync(){
  for(auto &pin: pins_pwd){
    digitalWrite(pin, LOW);
  }
  for(auto &pin: pins_gnd){
    digitalWrite(pin, LOW);
  }

  size_t led_index = 0;
  for(auto pin_gnd: pins_pwd){
    bool flag = false;
    for(auto pin_pwd: pins_pwd){
      digitalWrite(pin_pwd, configuration[led_index]);
      ++led_index;
    }

    if(flag){
      digitalWrite(pin_gnd, LOW);
      flag = false;
    }
  }
 
}
constexpr size_t count_states = 10;
std::array<led_state_t, count_states> states{
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
size_t last_time{};

led_cube_t cube = led_cube_t({9, 10, 11, 12}, {5, 6});

void setup() {
  Serial.begin(9600);
  last_time = millis();
  cube.set_pin_mode();
}

void loop() {
  size_t current_time = millis();
  if((current_time - last_time) > 100){
    last_time = current_time;

    current_state = (current_state + 1) % count_states;
    cube.set_state(states[current_state]);
    cube.sync();
  }
}

