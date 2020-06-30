#include "Keyboard.h"

void blink(){
  digitalWrite(PC13, LOW);
  Keyboard.println("You pressed the button");
  digitalWrite(PC13, HIGH);
}

void setup() {
  Keyboard.begin();

  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH);

  pinMode(PA0, INPUT_PULLUP);
  attachInterrupt(0, blink, FALLING);
}

void loop() {
}
