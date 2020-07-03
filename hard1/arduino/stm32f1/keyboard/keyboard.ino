#include "Keyboard.h"
#define theled PB2
#define thekbd PA0

void blink(){
  if(LOW == digitalRead(thekbd))return;

  digitalWrite(theled, HIGH);
  Keyboard.println("You pressed the button");
  digitalWrite(theled, LOW);
}

void setup() {
  Keyboard.begin();

  pinMode(theled, OUTPUT);
  digitalWrite(theled, LOW);

  pinMode(thekbd, INPUT_PULLDOWN);
  attachInterrupt(0, blink, RISING);
}
//
void loop() {
}
