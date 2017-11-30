#include <stdio.h>
#include <windows.h>
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif




int main()
{
	int j,x,y;
    DWORD cNumRead, fdwMode, fdwSaveOldMode;
	HANDLE hStdin, hStdout;
	CONSOLE_SCREEN_BUFFER_INFO bInfo;

    INPUT_RECORD irInBuf[128];
	KEY_EVENT_RECORD keyrec;
	MOUSE_EVENT_RECORD mouserec;
	WINDOW_BUFFER_SIZE_RECORD wbsrec;

    // Get the standard input handle
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if(hStdin == INVALID_HANDLE_VALUE)
	{
        printf("hStdin\n");
		return 0;
	}

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hStdout == INVALID_HANDLE_VALUE)
	{
        printf("hStdout\n");
		return 0;
	}

    // Save the current input mode, to be restored on exit
    if(!GetConsoleMode(hStdin, &fdwSaveOldMode) )
	{
        printf("GetConsoleMode\n");
		return 0;
	}

    // Enable the window and mouse input events
    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    if(!SetConsoleMode(hStdin, fdwMode) )
	{
        printf("SetConsoleMode\n");
		return 0;
	}

    // Loop to read and handle the next 100 input events
    while(1)
    {
        // Wait for the events
        if(!ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead) ) // number of records read
		{
            printf("ReadConsoleInput\n");
			return 0;
		}

        // Dispatch the events to the appropriate handler
        for(j = 0; j < cNumRead; j++)
        {
            switch(irInBuf[j].EventType)
            {
                case KEY_EVENT:
				{
					keyrec = irInBuf[j].Event.KeyEvent;
					if(keyrec.bKeyDown)
					{
						if(keyrec.uChar.AsciiChar == 0)
						{
							printf("KEY_EVENT(kbd):%x\n", keyrec.wVirtualKeyCode);
						}
						else
						{
							printf("KEY_EVENT(char):%x\n", keyrec.uChar.UnicodeChar);
						}
					}
                    break;
				}
                case MOUSE_EVENT:
				{
					mouserec = irInBuf[j].Event.MouseEvent;
					switch(mouserec.dwEventFlags)
					{
						case 0:
						{
							if(mouserec.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
							{
								printf("left button press \n");
							}
							else if(mouserec.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
							{
								printf("right button press \n");
							}
							else
							{
								printf("button press\n");
							}
							break;
						}
						case DOUBLE_CLICK:
						{
							printf("double click\n");
							break;
						}
						case MOUSE_HWHEELED:
						{
							printf("horizontal mouse wheel\n");
							break;
						}
						case MOUSE_MOVED:
						{
							GetConsoleScreenBufferInfo(hStdout, &bInfo);
							y = bInfo.srWindow.Top;
							printf("%x,%x\n",
								mouserec.dwMousePosition.X,
								mouserec.dwMousePosition.Y-y
							);
							break;
						}
						case MOUSE_WHEELED:
						{
							printf("vertical mouse wheel\n");
							break;
						}
						default:
						{
							printf("unknown\n");
							break;
						}
					}
                    break;
				}
                case WINDOW_BUFFER_SIZE_EVENT:
				{
					wbsrec = irInBuf[j].Event.WindowBufferSizeEvent;
					//printf("Resize:%x,%x\n", wbsrec.dwSize.X, wbsrec.dwSize.Y);

					GetConsoleScreenBufferInfo(hStdout, &bInfo);
					x = bInfo.srWindow.Right - bInfo.srWindow.Left + 1;
					y = bInfo.srWindow.Bottom - bInfo.srWindow.Top + 1;
					printf("Resize:%x,%x\n", x, y);
                    break;
				}
                case MENU_EVENT:
				{
					printf("MENU_EVENT\n");
					exit(-1);
                    break;
				}
                case FOCUS_EVENT:
				{
					printf("FOCUS_EVENT\n");
                    break;
				}
                default:
				{
                    printf("Unknown:%x", irInBuf[j].EventType);
                    break;
				}
            }
        }
    }

    //Restore input mode on exit
    SetConsoleMode(hStdin, fdwSaveOldMode);
    return 0;
}