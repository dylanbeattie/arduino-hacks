#define DEBUG 1

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "limits.h"

USBHIDKeyboard Keyboard;

#define PINCOUNT 10
unsigned long debounceDelayInMilliseconds = 10;

int keys[] = {
  /* 0 */ 0,
  /* 1 */ KEY_PAGE_DOWN,
  /* 2 */ KEY_F18,
  /* 3 */ KEY_F19,
  /* 4 */ KEY_F20,
  /* 5 */ KEY_F21,
  /* 6 */ KEY_PAGE_UP,
  /* 7 */ KEY_F22,
  /* 8 */ KEY_F23,
  /* 9 */ KEY_F24
};

unsigned long lastDebounceTimes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int actualStates[] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };
int formerStates[] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#endif
  for (int pin = 0; pin < PINCOUNT; pin++) pinMode(pin, INPUT_PULLUP);
  Keyboard.begin();
  USB.begin();
  Keyboard.releaseAll();
}

unsigned long now;
void loop() {
  for (int pin = 0; pin < PINCOUNT; pin++) {
    unsigned long now = millis();
    int reading = digitalRead(pin);
    if (reading != formerStates[pin]) lastDebounceTimes[pin] = now;
    unsigned long gap = now - lastDebounceTimes[pin];
    if (gap > debounceDelayInMilliseconds) {
      if (reading != actualStates[pin]) {
        actualStates[pin] = reading;
#ifdef DEBUG
        Serial.print(pin);
        Serial.print(": ");
        Serial.print(now);
        Serial.print(" - ");
        Serial.print(lastDebounceTimes[pin]);
        Serial.print(" = ");
        Serial.print(gap);
        Serial.print(" (");
        Serial.print(reading);
        Serial.println(")");
#endif
        if (reading == LOW) {
          Keyboard.write(keys[pin]);
        } else {
          Keyboard.release(keys[pin]);
        }
      }
    }
    formerStates[pin] = reading;
  }
}
#endif /* ARDUINO_USB_MODE */
