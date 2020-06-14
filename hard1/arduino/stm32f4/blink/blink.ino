static char val = LOW;
void blink(){
  val = !val;
}

void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PA0, INPUT_PULLUP);
  attachInterrupt(0, blink, FALLING);
}

void loop() {
  digitalWrite(PC13, val);
}
