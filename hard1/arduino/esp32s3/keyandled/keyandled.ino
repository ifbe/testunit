#ifdef RGB_BUILTIN

#define KEYPIN 0
int g_old = 0;
int g_val = 1;

void setup() {
  Serial.begin(115200);

  setled();

  pinMode(KEYPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEYPIN), keyisr, FALLING);
}

// the loop function runs over and over again forever
void loop() {
  setled();
}

void setled()
{
  if(g_old == g_val)return;
  g_old = g_val;

  Serial.print("setled:");
  Serial.println(g_val);

  int b = !!(g_val&1);
  int g = !!(g_val&2);
  int r = !!(g_val&4);
  b *= RGB_BRIGHTNESS/16;
  g *= RGB_BRIGHTNESS/16;
  r *= RGB_BRIGHTNESS/16;
  rgbLedWrite(RGB_BUILTIN, r, g, b);
}

void getgpio()
{
  int val = digitalRead(KEYPIN);
  Serial.print("gpio=");
  Serial.print(KEYPIN);
  Serial.print(", val=");
  Serial.println(val);
}

void keyisr()
{
  g_val = (g_val+1) & 7;
}
#endif
