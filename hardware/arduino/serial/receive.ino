void setup()
{
	Serial.begin(115200);
	Serial1.begin(9600);
}
void loop()
{
	char incomingByte;

	while (Serial1.available() > 0)
	{
		// read the incoming byte:
		incomingByte = Serial1.read();

		// say what you got:
		Serial.print(incomingByte);
	}
}

