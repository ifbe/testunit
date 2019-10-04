/*
 LEDC Software Fade

 This example shows how to software fade LED
 using the ledcWrite function.

 Code adapted from original Arduino Fade example:
 https://www.arduino.cc/en/Tutorial/Fade

 This example code is in the public domain.
 */

// use 5000 Hz as a LEDC base frequency [2,1220]
#define LEDC_BASE_FREQ     31280

// use first channel of 16 channels (started from zero)
#define CH0   0
#define CH1   1

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define PWM_L   13
#define PWM_R   2

#define DIR_L   14
#define DIR_R   4

#define EN_L    12
#define EN_R    0

int dir = 1;

void setup() {
  pinMode(EN_L, OUTPUT);
  pinMode(EN_R, OUTPUT);

  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);

  ledcSetup(CH0, LEDC_BASE_FREQ, 10);
  ledcAttachPin(PWM_L, CH0);
  ledcWrite(CH0, 1000);

  ledcSetup(CH1, LEDC_BASE_FREQ, 10);
  ledcAttachPin(PWM_R, CH1);
  ledcWrite(CH1, 1000);
}

void loop() {
  digitalWrite(EN_L, 1);
  digitalWrite(EN_R, 1);

  digitalWrite(DIR_L, dir);
  digitalWrite(DIR_R, !dir);

  delay(500);
  dir = !dir;
}
