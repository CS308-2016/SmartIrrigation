/*

*teamId: 15 (Team Leader: Nishanth Rumandla)

* Author List: Nishanth Rumandla, Dheeraj Reddy kalmekolen

* Description: This code will familiarize you with using GPIO on TMS4C123GXL microcontroller.

* Filename: lab-1.c

* Functions: setup(), ledPinConfig(), switchPinConfig(), main()

* Global Variables: sw2Status, LOCK_F, CR_F

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
------ Global Variable Declaration
*/
uint8_t sw2Status=0;

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

* Function Name: ledPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

* Example Call: ledPinConfig();

*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1, Pin-2, Pin-3 of PORT F set as output.
}

/*

* Function Name: switchPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

* Example Call: switchPinConfig();

*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4, Pin-0 of PORT F as Input.
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

/*

* Function Name: main()

* Input: none

* Output: LED color change on SW1 press, sw2Status incremented on SW2 press

* Description: Indefinite looping where it checks whether the switches are pressed

* Example Call: automatically called

*/
int main(void)
{
	uint8_t ui8LED = 4;
	uint8_t test=1;
	uint8_t test2=1;

	setup();
	ledPinConfig();
	switchPinConfig();

	while(1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4 )==0x00){
			//GPIOPinRead = 0 if switch pressed
			if(test==1){
				//if switch was not pressed, make it on now
				if (ui8LED == 2)
				{
					ui8LED = 8;
				}
				else if (ui8LED == 8)
				{
					ui8LED = 4;
				}
				else{
					ui8LED=2;
				}
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
				test=0;
				//SysCtlDelay(1000000);
			}
		}
		else{
			if(test==0){
				test=1;
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
			}
		}
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
			//GPIOPinRead = 0 if switch pressed
			if(test2==1){
				//if switch was not pressed, make it on now
				test2=0;
				sw2Status+=1;
				//SysCtlDelay(10000);
			}
		}
		else{
			if(test2==0){
				test2=1;
			}
		}
		SysCtlDelay(10000);
	}

}
