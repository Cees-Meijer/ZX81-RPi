// ZX80 Raspberry Keyboard Scanner
// Based on the Python program by @mrpjevans mrpjevans.com 2017
// MIT License (https://opensource.org/licenses/MIT) see LICENSE
// Scan a hardware keyboard connected to the GPIO pins of a RPi.

#include <stdio.h>
#include <stdlib.h>
#include "keyscanner.h"
#include <wiringPi.h>

// KB1 Connector
char dataLines[] = {24,23,22,21,14 }; // Wiringpi
//char dataLines[] = {19,13,6,5,11 }; //GPIO

// KB2 Connector
char addressLines[] = {8,9,7,0 ,2 ,3 ,12,13};//wiringpi
//char addressLines[] = {2,3,4,17,27,22,10,9}; //GPIO
#define NR_ADDRESLINES 8
#define NR_DATALINES 5

// The ZX 80 Keyboard Matrix (Mapped to hardware keyboard )
char keys[8][5] = {
	{'B','N','M','.',' '},
	{'H','J','K','L',ENTER},  // ENTER,
	{'V','C','X','Z',SHIFT}, //0x12 Leftshift
	{'Y','U','I','O','P'},
	{'G','F','D','S','A'},
	{'6','7','8','9','0'},
	{'T','R','E','W','Q'},       
	{'5','4','3','2','1'} 
};

// Track keypresses so we can support multiple keys
unsigned char keyTrack[8][5] = {
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0}
};

// Keyboard mode and reset button
int buttonPressed = -1;
int buttonGPIO = 12;
int buttonTime = 0;
int isFree = 0;
char keyPressed =0;

void init_keyboard()
{
wiringPiSetup();
//wiringPiSetupGpio();

//# Set all address lines low
for (int addressLine=0;addressLine < NR_ADDRESLINES;addressLine++)
{
	pinMode(addressLines[addressLine], OUTPUT);
	digitalWrite(addressLines[addressLine], 0);
}

//# Set all data lines for input
for (int dataLine=0; dataLine<NR_DATALINES;dataLine++)
{
	pinMode(dataLines[dataLine],INPUT);
	pullUpDnControl(dataLines[dataLine], PUD_DOWN);
}

//# Setup Button
//pullUpDnControl(buttonGPIO, 2);
}

void set_addresLine(int addressLine)
{
	printf("Set:A%d\r\n",addressLines[addressLine]);
	digitalWrite(addressLines[addressLine], 1);
}

int kb_scan(unsigned int *key)
{
 int type = 0;
 int _esc =0;
 		// Individually set each address line high
		for(int addressLine=0;addressLine<NR_ADDRESLINES;addressLine++)
		{
			digitalWrite(addressLines[addressLine], HIGH);
			delay(1);
			//# Scan data lines
			for(int dataLine=0;dataLine < NR_DATALINES;dataLine++)
			{
				//# Get state and details for this button
				isFree = digitalRead(dataLines[dataLine]);
				
				keyPressed = keys[addressLine][dataLine];
                if(isFree ==1)
                {
					if(keyPressed==SHIFT || keyPressed=='1' || keyPressed==' '){_esc++;}
				}
				// If pressed for the first time
				if(isFree == 1 && keyTrack[addressLine][dataLine] == 0)
                {
					//# Press the key and make a note
					printf("A:%d,D:%d [%c]\r\n", addressLine,dataLine,keyPressed);
					keyTrack[addressLine][dataLine] = 1;
					*key = (unsigned int)keyPressed;
					type = K_PRESS;
				}

				// If not pressed now but was pressed on last check
				else if(isFree == 0 && keyTrack[addressLine][dataLine] == 1)
				{
					//# Release the key and make a note
					//printf("Releasing %c", keyPressed);
					keyTrack[addressLine][dataLine] = 0;
					*key = (unsigned int)keyPressed;
					
					type=K_RELEASE;
				}

			delay(1);
            }
			digitalWrite(addressLines[addressLine], LOW);

	    }
        
	    if(_esc>=2){*key =ESC;}				// Press Shift, 1 and SPACE : ESC (=Soft Reset)
 return type;
}
