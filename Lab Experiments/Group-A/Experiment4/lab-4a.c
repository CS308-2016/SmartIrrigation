/*
* Team id: 15
* Author: Nishanth Rumandla, Dheeraj Reddy Kalmekolen

* Filename: lab-4a.c

* Functions:  main(), UARTIntHandler(), print_curr(), print_enter_temp(), setup()

* Global Variables: none

*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"

/*

 * Function Name: UARTIntHandler

 * Input: none

 * Output: none
 *
 * Description: depending on state, takes temperature as input from user
 *
 * Example Call: called automatically on interrupt

 */
void UARTIntHandler(void)
{
	
}
uint32_t ui32TempValueC;
uint32_t setTemp=25;
uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
/*

* Function Name: setup

* Input: none

* Output: none

* Description: sets up UART, ADC and GPIO

* Example Call: setup()

*/

void setup(){
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_3); //enable pin for LED PF2
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_UART0); //enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	
	ADCIntClear(ADC0_BASE, 1);
	ADCProcessorTrigger(ADC0_BASE, 1);
		
}

/*

* Function Name: main()

* Input: none

* Output: current temperature and depending on it, LED color change

* Description: prints out the current temperature

* Example Call: automatically called

*/

int main(void) {

		setup();
			
		while(!ADCIntStatus(ADC0_BASE, 1, false)){}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		UARTCharPut(UART0_BASE,'C');
		UARTCharPut(UART0_BASE,'u');
		UARTCharPut(UART0_BASE,'r');
		UARTCharPut(UART0_BASE,'r');
		UARTCharPut(UART0_BASE,'e');
		UARTCharPut(UART0_BASE,'n');
		UARTCharPut(UART0_BASE,'t');
		UARTCharPut(UART0_BASE,' ');
		UARTCharPut(UART0_BASE,'T');
		UARTCharPut(UART0_BASE,'e');
		UARTCharPut(UART0_BASE,'m');
		UARTCharPut(UART0_BASE,'p');
		UARTCharPut(UART0_BASE,'e');
		UARTCharPut(UART0_BASE,'r');
		UARTCharPut(UART0_BASE,'a');
		UARTCharPut(UART0_BASE,'t');
		UARTCharPut(UART0_BASE,'u');
		UARTCharPut(UART0_BASE,'r');
		UARTCharPut(UART0_BASE,'e');
		UARTCharPut(UART0_BASE,':');
		UARTCharPut(UART0_BASE,'0'+ui32TempValueC/10);
		UARTCharPut(UART0_BASE,'0'+ui32TempValueC%10);
		UARTCharPut(UART0_BASE,' ');
		UARTCharPut(UART0_BASE,169);
		UARTCharPut(UART0_BASE,'C');
		UARTCharPut(UART0_BASE,' ');
		
}
