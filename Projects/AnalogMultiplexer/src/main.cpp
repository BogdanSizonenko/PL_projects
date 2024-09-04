#include <Arduino.h>
#include <CD74HC4067.h>

CD74HC4067 mux(14, 27, 26, 25);

void setup() {
  Serial.begin(9600);
}

void loop() {
  mux.channel(1);
}
