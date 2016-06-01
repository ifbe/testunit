void setup() {
	Serial.begin(115200);
	Serial1.begin(115200);
}

void loop()
{
	int c[4];

	c[0]=analogRead(A0);
	c[1]=analogRead(A1);
	c[2]=analogRead(A2);
	c[3]=analogRead(A3);

	Serial1.print(c[0]);
	Serial1.print(' ');
	Serial1.print(c[1]);
	Serial1.print(' ');
	Serial1.print(c[2]);
	Serial1.print(' ');
	Serial1.print(c[3]);
	Serial1.println("");

	delay(10);
}
