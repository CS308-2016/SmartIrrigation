/*
* Team id: 15(Team Leader: Nishanth)
* Edited by: Nikhil, Mahindar

* Filename: lab-2.c

* Functions: setup(), ledPinConfig(), switchPinConfig(), main(), detectKeyPress1(), detectKeyPress2()

* Global Variables: ui8LED, s1_on, s2_on, flag1, flag2, state1, state2, sw2Status

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
------ Global Variable Declaration
*/
uint8_t sw2Status=0;
//uint32_t ui32Period;
uint8_t ui8LED=4;
uint8_t flag1=0,flag2=0;
uint8_t state1=0,state2=0;
/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals

* Example Call: setup();

*/

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
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
uint8_t detectKeyPress1()
{
	if(state1==0){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
			state1=1;
		}
	}
	else if(state1==1){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
			state1=2;
			flag1=1;
		}
		else{
			state1=0;
			flag1=0;
		}
	}
	else{
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
			//flag1=0;
		}
		else{
			state1=0;
			flag1=0;
		}
	}
	return flag1;
}
/*
* Function Name: detectKeyPress2()
* Input: none
* Output: returns flag
* Description: flag is whether the key is long pressed
* Example Call: detectKeyPress2();
*/
uint8_t detectKeyPress2()
{
	if(state2==0){
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
				state2=1;
			}
		}
		else if(state2==1){
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
				state2=2;
				flag2=1;
			}
			else{
				state2=0;
				flag2=0;
			}
		}
		else{
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
				//flag2=0;
			}
			else{
				state2=0;
				flag2=0;
			}
		}
	return flag2;
}

uint8_t s1_on = 0, s2_on = 0;
/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: changes LED on SW1 press and increments sw2Status on SW2 press
*
* Description: handles interrupts on Timer0
*
* Example Call: called automatically on interrupt

*/
void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if(detectKeyPress1()){
		if(s1_on==0){
			//if switch was not pressed, make it on now
			s1_on=1;
			if (ui8LED == 2)
			{
				ui8LED = 8;
			}
			else
			{
				ui8LED = ui8LED/2;
			}
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		}
	}
	else{
		if(s1_on==1){
			s1_on=0;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
		}
	}
	if(detectKeyPress2()){
		//if switch was not pressed, make it on now
		if(s2_on==0){
			s2_on=1;
			sw2Status++;
		}
	}
	else{
		if(s2_on==1){
			s2_on=0;
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

	uint32_t ui32Period;
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	switchPinConfig();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{
	}
}
