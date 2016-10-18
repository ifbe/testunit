void setup()
{
	Serial.begin(115200);
	Serial1.begin(115200);
}
void loop()
{
	char ch;

	while(1)
	{
		if(Serial1.available() > 0)
		{	
			ch = Serial1.read();
			Serial.print(ch);
		}
		if(Serial.available() > 0)
		{
			ch = Serial.read();
			Serial1.print(ch);
		}
	}
}

