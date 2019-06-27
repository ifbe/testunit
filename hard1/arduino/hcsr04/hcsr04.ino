
const int trigpin = 2;
const int echopin = 5;
long duration;
long distance;

void setup() {
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distance = duration/2*340/1000;

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println("mm");
}
