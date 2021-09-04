//board: generic stm32f4 serials
//board part number: blackpill f103c8
//USB support: CDC(generic serial)
#define LEDPIN PC13
#define KEYPIN PA0

static char want = LOW;
static char curr = HIGH;
void blink(){
  if(HIGH == digitalRead(KEYPIN))return;
  want = !want;
}

void setup() {
  pinMode(LEDPIN, OUTPUT);
  pinMode(KEYPIN, INPUT_PULLUP);
  attachInterrupt(0, blink, FALLING);
}

void loop() {
  if(want != curr){
    digitalWrite(LEDPIN, want);
    curr = want;
  }
}
