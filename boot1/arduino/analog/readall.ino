void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print(analogRead(A0));
  Serial.print(',');
  Serial.print(analogRead(A1));
  Serial.print(',');
  Serial.print(analogRead(A2));
  Serial.print(',');
  Serial.print(analogRead(A3));
  Serial.print(',');

  //4
  Serial.print(analogRead(A6));
  Serial.print(',');

  //6
  Serial.print(analogRead(A7));
  Serial.print(',');

  //8
  Serial.print(analogRead(A8));
  Serial.print(',');

  //9
  Serial.print(analogRead(A9));
  Serial.print(',');

  //10
  Serial.print(analogRead(A10));
  Serial.print('\n');
}