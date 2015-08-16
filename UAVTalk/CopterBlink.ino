#include "UAVTalk.h"
#include <Adafruit_NeoPixel.h>
#include "SimpleTimer.h"  //see http://playground.arduino.cc/Code/SimpleTimer#GetTheCode for installation instructions

#define PIN 4
#define PIXEL 16

Adafruit_NeoPixel ring = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
enum state {
  stopped,
  checking_leds,
  running_leds
};
// the timer object
SimpleTimer timer;
int timerId;

state main_state = stopped;

const uint32_t RED = ring.Color(255, 0, 0);
const uint32_t GREEN = ring.Color(0, 255, 0);
const uint32_t BLUE = ring.Color(0, 0, 255);
const uint32_t MAGENTA = ring.Color(255, 0, 255);
const uint32_t YELLOW = ring.Color(255, 255, 0);
const uint32_t DARK_YELLOW = ring.Color(155, 155, 0);
const uint32_t PINK = ring.Color(219,112,147);
const uint32_t SKY_BLUE = ring.Color(135,206,255);
const uint32_t MAIZE = ring.Color(128,158,10);
const uint32_t LAVENDER = ring.Color(88,2,163);
const uint32_t SEA_FOAM = ring.Color(32,178,170);
const uint32_t SPleds = ring.Color(102,205,0);
const uint32_t DARK_ORANGE = ring.Color(237,180,6);
const uint32_t ORANGE = ring.Color(237,120,6);
const uint32_t WHITE = ring.Color(255,255,255);
const uint32_t BLACK = ring.Color(0,0,0);

void setup() {
  Serial.begin(57600);
  ring.begin();
  ring.show(); // Initialize all pixels to 'off'
  ring.setBrightness(100);
  main_state = checking_leds;
  timerId = timer.setInterval(500, toggleLedColor);
}

void loop() {
   switch (main_state) {
    case checking_leds:
      checkLeds();
      break;
    case running_leds:
      uavtalk_read();
      setLedOutputs();
      ring.show();
      break;
   }
}

enum color_state {
  leds_off,
  leds_red,
  leds_green,
  leds_blue,
  leds_done
};
color_state init_color_state = leds_off;

// runs every 0.5s
void toggleLedColor() {
  switch (init_color_state) {
    case leds_off:
      setAllLeds(RED);
      init_color_state = leds_red;
      break;
    case leds_red:
      setAllLeds(GREEN);
      init_color_state = leds_green;
      break;
    case leds_green:
      setAllLeds(BLUE);
      init_color_state = leds_blue;
      break;
    case leds_blue:
      setAllLeds(BLACK);
      timer.deleteTimer(timerId);
      init_color_state = leds_done;
      main_state = running_leds;  
      break;
  }
  ring.show();
}

void setAllLeds(unsigned long new_color) {
  for (int i=0; i < PIXEL; i++){
    ring.setPixelColor(i, new_color);
  } 
}

void checkLeds() {
  timer.run();
}

void setLedOutputs() {
   // armed
   if (osd_armed == 1) { // seems, that messages are sent only once on state-change "event"
                         // and that these onetime-messages get lost sometimes (same behavior seen at flight mode "osd_mode")
                         // Flight Telemetry Update Period can be set in GCS (Default for armend: 5000ms)
                         // UPDATE: Changed to 500ms. But this led is not working reliable... grrr
     ring.setPixelColor(0, DARK_YELLOW); // arming = DARK_YELLOW
   } else if (osd_armed == 2) {
     ring.setPixelColor(0, RED); // armed = RED
   } else {
     ring.setPixelColor(0, BLACK);
   }

   // yaw
   uint32_t value = (osd_yaw + 180) * 25 / 36; // 0 - 255
   if (value > 255) {
     value = 255;
   }
   uint32_t color =  value;
   ring.setPixelColor(1, color);
   
   // throttle (0-100)
   value = osd_throttle;   // works well, if Flight Telemetry Update Period is set to 300ms in GCS
   value = value * 2.55;
   color = value << 16;    // red
   ring.setPixelColor(2, color);

   // flight mode
   if (osd_mode == 1) {
     ring.setPixelColor(3, GREEN); // green
   } else if (osd_mode == 2) {
     ring.setPixelColor(3, YELLOW); // yellow
   } else if (osd_mode == 3) {
     ring.setPixelColor(3, RED); // red
   } else {
     ring.setPixelColor(3, LAVENDER); // 
   }  
}

