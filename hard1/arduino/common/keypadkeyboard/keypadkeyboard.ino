/* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/
#include <Mouse.h>
#include <Keyboard.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
{'m','n','o','p'},    //ll,ld,lu,lr
{'i','j','k','l'},    //lk,ls,lb,lt
{'e','f','g','h'},    //rd,ru,rr,rl
{'a','b','c','d'}    //rs,rt,rb,rk
};
byte rowPins[ROWS] = {2, 3, 5, 7};	//row
byte colPins[COLS] = {10, 16, 14, 15};	//column
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int lx = 0;
int ly = 0;
int rx = 0;
int ry = 0;
int t[4];

unsigned long loopCount;
unsigned long startTime;
String msg;


void setup() {
    //Keyboard.begin();
    Serial.begin(115200);
    loopCount = 0;
    startTime = millis();
    msg = "";
    Mouse.begin();
    Keyboard.begin();
}


void loop() {
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        //Serial.print("Average loops per second = ");
        //Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }
    
    t[0] = analogRead(A6);
    t[1] = 1023-analogRead(A7);
    t[2] = 1023-analogRead(A0);
    t[3] = analogRead(A1);
    if((lx!=t[0])|(ly!=t[1])|(rx!=t[2])|(ry!=t[3]))
    {
        if(t[0] < 256)Mouse.move(-1, 0);
        if(t[0] > 768)Mouse.move(1, 0);
        if(t[1] < 256)Mouse.move(0, 1);
        if(t[1] > 768)Mouse.move(0, -1);
        Serial.print(lx);
        Serial.print(',');
        Serial.print(ly);
        Serial.print(',');
        Serial.print(rx);
        Serial.print(',');
        Serial.print(ry);
        Serial.print('\n');
    }
    lx = t[0];
    ly = t[1];
    rx = t[2];
    ry = t[3];

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    msg = " PRESSED.";
                    Keyboard.press(kpd.key[i].kchar);
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    msg = " RELEASED.";
                    Keyboard.release(kpd.key[i].kchar);
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print(kpd.key[i].kchar);
                Serial.println(msg);

            }
        }
    }
}  // End loop
