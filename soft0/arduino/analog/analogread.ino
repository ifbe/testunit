void setup()
{
	Serial.begin(115200);
	Mouse.begin();
}

void loop()
{
	int x,y,z;
	int c[4];

	c[0]=512 - analogRead(A0);
	c[1]=analogRead(A1) - 512;
	c[2]=analogRead(A2) - 512;
	c[3]=analogRead(A3) - 512;

	Serial.print( c[0] );
	Serial.print(' ');

	Serial.print( c[1] );
	Serial.print(' ');

	Serial.print( c[2] );
	Serial.print(' ');

	Serial.print( c[3] );
	Serial.print(' ');

	Serial.println("");



	x=y=z=0;
	if( (c[0]<-50) | (c[0]>50) )
	{
		x = c[0] / 200;
	}
	if( (c[1]<-50) | (c[1]>50) )
	{
		y = c[1] / 200;
	}
	if(c[3]<-50)z = 1;
	if(c[3]>50)z = -1;
	if( (x!=0) | (y!=0) | (z!=0) )Mouse.move(x,y,z);
}
