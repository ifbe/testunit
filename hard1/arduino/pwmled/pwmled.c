void setup() {
    Serial.begin(115200);
    pinMode(PA1, PWM);
    pinMode(PA2, PWM);
    pinMode(PA3, PWM);
}

void loop() {
    int a = random(65536);
    int b = random(65536);
    int c = random(65536);
    Serial.print(a);
    Serial.print(' ');
    Serial.print(b);
    Serial.print(' ');
    Serial.print(c);
    Serial.print('\n');

    pwmWrite(PA1, a);
    pwmWrite(PA2, b);
    pwmWrite(PA3, c);
    delay(1000);
}
