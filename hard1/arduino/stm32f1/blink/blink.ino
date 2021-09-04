//board: generic stm32f1 serials
//board part number: generic f103c8
//USB support: CDC(generic serial)
#define LEDPIN PB2
#define KEYPIN PA0

static char want = LOW;
static char curr = HIGH;
void blink(){
  if(LOW == digitalRead(KEYPIN))return;
  Serial.println("shit");
  want = !want;
}

void setup() {
  Serial.begin(115200);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);

  pinMode(KEYPIN, INPUT_PULLDOWN);
  attachInterrupt(0, blink, RISING);
}

void loop() {

  if(want != curr){
    digitalWrite(LEDPIN, want);
    curr = want;
  }
}
