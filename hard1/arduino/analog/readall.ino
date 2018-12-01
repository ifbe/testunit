void setup() {
  Serial.begin(115200);
  analogReference(INTERNAL);
}

void loop() {
  Serial.print(analogRead(A0)*2.56/1024.0);
  Serial.print(',');
  Serial.print(analogRead(A1)*2.56/1024.0);
  Serial.print(',');
  Serial.print(analogRead(A2)*2.56/1024.0);
  Serial.print(',');
  Serial.print(analogRead(A3)*2.56/1024.0);
  Serial.print(',');

  //4
  Serial.print(analogRead(A6)*2.56/1024.0);
  Serial.print(',');

  //6
  Serial.print(analogRead(A7)*2.56/1024.0);
  Serial.print(',');

  //8
  Serial.print(analogRead(A8)*2.56/1024.0);
  Serial.print(',');

  //9
  Serial.print(analogRead(A9)*2.56/1024.0);
  Serial.print(',');

  //10
  Serial.print(analogRead(A10)*2.56/1024.0);
  Serial.print('\n');
}