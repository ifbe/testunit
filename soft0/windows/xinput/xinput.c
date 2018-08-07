#include<windows.h>
#include<xinput.h>
#include<stdio.h>
#include<stdlib.h>
static char* xname[16] = {
	"XINPUT_GAMEPAD_DPAD_UP",		//0x0001
	"XINPUT_GAMEPAD_DPAD_DOWN",		//0x0002
	"XINPUT_GAMEPAD_DPAD_LEFT",		//0x0004
	"XINPUT_GAMEPAD_DPAD_RIGHT",	//0x0008
	"XINPUT_GAMEPAD_START",			//0x0010
	"XINPUT_GAMEPAD_BACK",			//0x0020
	"XINPUT_GAMEPAD_LEFT_THUMB",	//0x0040
	"XINPUT_GAMEPAD_RIGHT_THUMB",	//0x0080
	"XINPUT_GAMEPAD_LEFT_SHOULDER",		//0x0100
	"XINPUT_GAMEPAD_RIGHT_SHOULDER",	//0x0200
	"????1",	//0x0400
	"????2",	//0x0800
	"XINPUT_GAMEPAD_A",		//0x1000
	"XINPUT_GAMEPAD_B",		//0x2000
	"XINPUT_GAMEPAD_X",		//0x4000
	"XINPUT_GAMEPAD_Y"		//0x8000
};




void print(int id, XINPUT_GAMEPAD g)
{
	int j;
	if(	(0 == g.wButtons) &&
		(8 >= g.bLeftTrigger) &&
		(8 >= g.bRightTrigger) &&
		(4096 >= g.sThumbLX) &&
		(4096 >= g.sThumbLY) &&
		(4096 >= g.sThumbRX) &&
		(4096 >= g.sThumbRY) )
	{
		return;
	}

	printf(
		"%x:\n"
		"	%x,%x\n"
		"	%d,%d\n"
		"	%d,%d\n",
		id,
		g.bLeftTrigger, g.bRightTrigger,
		g.sThumbLX, g.sThumbLY,
		g.sThumbRX, g.sThumbRY
	);
	for(j=0;j<16;j++)
	{
		if(g.wButtons & (1<<j))
		{
			printf("	%s\n", xname[j]);
		}
	}
}
void main()
{
	XINPUT_STATE s;
	printf("fuckyou\n");

	while(1)
	{
		if(ERROR_SUCCESS == XInputGetState(0, &s))
		{
			print(0, s.Gamepad);
		}
		if(ERROR_SUCCESS == XInputGetState(1, &s))
		{
			print(1, s.Gamepad);
		}
		if(ERROR_SUCCESS == XInputGetState(2, &s))
		{
			print(2, s.Gamepad);
		}
		if(ERROR_SUCCESS == XInputGetState(3, &s))
		{
			print(3, s.Gamepad);
		}
	}
}