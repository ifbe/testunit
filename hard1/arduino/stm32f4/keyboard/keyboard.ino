#include "Keyboard.h"
#define theled PC13
#define thekbd PA0

void blink(){
  if(HIGH == digitalRead(thekbd))return;

  digitalWrite(theled, LOW);
  Keyboard.println("You pressed the button");
  digitalWrite(theled, HIGH);
}

void setup() {
  Keyboard.begin();

  pinMode(theled, OUTPUT);
  digitalWrite(theled, HIGH);

  pinMode(thekbd, INPUT_PULLUP);
  attachInterrupt(0, blink, FALLING);
}
//
void loop() {
}
