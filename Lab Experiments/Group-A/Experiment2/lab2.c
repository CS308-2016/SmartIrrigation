/*

*teamId: 15 (Team Leader: Nishanth Rumandla)

* Author List: Nishanth Rumandla, Dheeraj Reddy kalmekolen

* Filename: lab2.c

* Functions: setup(), ledPinConfig(), switchPinConfig(), main(), detectKeyPress1(), detectKeyPress2()

* Global Variables: ui8LED, test, test2, sw1, sw2, sw2Status, LOCK_F, CR_F

*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
uint8_t sw2Status=0;
uint8_t sw1=0; // IDLE
uint8_t sw2=0; // IDLE
uint8_t test=1;
uint8_t test2=1;
uint8_t ui8LED = 4;
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
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
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
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

/*

* Function Name: detectKeyPress1()

* Input: none

* Output: returns flag

* Description: flag is whether the key is long pressed

* Example Call: detectKeyPress1();

*/

unsigned char detectKeyPress()
{
	unsigned char flag=0;
	if(sw1==0 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
		sw1=1;
	}
	else if(sw1==1 && !(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)){
		sw1=0;
	}
	else if(sw1==2 && !(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)){
			sw1=0;
	}
	else if(sw1==1 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
		sw1=2;
		//return 1;
	}

	if(sw1==2){
		flag=1;
	}
	return flag;
}
/*
* Function Name: detectKeyPress2()
* Input: none
* Output: returns flag
* Description: flag is whether the key is long pressed
* Example Call: detectKeyPress2();
*/

unsigned char detectKeyPress2()
{
	if(sw2==0 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
		sw2=1;
	}
	else if(sw2==1 && !GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
		sw2=0;
	}
	else if(sw2==1 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
		sw2=2;
	}
	else if(sw2==2 && !GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
		sw2=0;
	}
	if(sw2==2){
		return 1;
	}
	return 0;
}
/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: changes LED on SW1 press and increments sw2Status on SW2 press
*
* Description: handles interrupts on
*
* Example Call: called automatically on interrupt

*/

void Timer0IntHandler(void)
{
	// Clear the timer interrupt


	if(detectKeyPress()){
		if(test==1){
			//if switch was not pressed, turn it on now
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
		}
	}
	else{
		if(test==0){
			test=1;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
		}
	}
	if(detectKeyPress2()){
		//if switch was not pressed, make it on now
		if(test2==1){
			test2=0;
			sw2Status+=1;
		}
	}
	else{
		if(test2==0){
			test2=1;
		}
	}

}
/*

* Function Name: main()

* Input: none

* Output: none

* Description: Keeps on looping indefinitely

* Example Call: automatically called

*/
int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	uint32_t ui32Period;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 100) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
	while(1)
	{

	}
}
