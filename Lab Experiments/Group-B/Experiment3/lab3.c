/*
* Team id: 15 (Team Leader: Nishanth)
* Edited by: Mahindar, Nikhil

* Filename: lab3.c

* Functions: main()

* Global Variables: state

*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

#define divide 120.0
int state=0;

/*

* Function Name: main()

* Input: none

* Output: none

* Description: use the PWM on the TivaC Series device to control the brightness of LED present in the development board.

* Example Call: automatically called

*/
int main(void)
{

	volatile uint8_t max_value;
	volatile uint8_t min_value;
	volatile uint16_t state_angle;
	int delay = 1000000;

	volatile uint8_t x_i;
	volatile uint8_t y_i;
	volatile uint8_t green_i;
	volatile uint8_t blue_i;
	volatile uint8_t red_i;
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	state_angle = 0;
	max_value = 240;
	min_value = 10;
	x_i = max_value;
	y_i = min_value;

	//.The PWM module is clocked by the system clock through a divider,and that divider has a range of 2 to 64
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    //enable the PWM1 and the GPIOF module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// PortF pin1(PF1) must be configured as a PWMoutputpinformodule1,PWM generator2
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
    // PortF pin2(PF1) must be configured as a PWMoutputpinformodule1,PWM generator3
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
    // PortF pin3(PF1) must be configured as a PWMoutputpinformodule1, PWM generator3
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	//unlock the GPIO commit control register
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	//configures PF0 & 4 as inputs
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	//Configure module 1 PWM generator 2 as a down-counter and load the count value
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	 //Configure module 1 PWM generator 3 as a down-counter and load the count value
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	//PWM module 1, generator 2,3 needs to be enabled as an output andenabledtorun
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, x_i * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, y_i * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, y_i * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	while(1)
	{
		if(state==0){
			state_angle = (state_angle + 1)%360;
			if(state_angle<divide){
				x_i = min_value + state_angle*max_value/divide;
				y_i = min_value + max_value - x_i;
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, y_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, min_value * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, x_i * ui32Load / 1000);
			} else if (state_angle >= divide && state_angle<2*divide){
				x_i = min_value+(state_angle-divide)*max_value/divide;
				y_i = min_value+max_value - x_i;
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, min_value * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, x_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, y_i * ui32Load / 1000);
			} else if(state_angle >= 2*divide && state_angle<3*divide){
				x_i = min_value+(state_angle-2*divide)*max_value/divide;
				y_i = min_value+max_value - x_i;
				// set colour using the values in ui8AdjustR, ui8AdjustB, ui8AdjustG values
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, x_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, y_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, min_value * ui32Load / 1000);
			}
			if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)) delay = delay-1000;

			SysCtlDelay(delay);
		}
		while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
			if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&state<4){
				state++;

			}
			delay = delay+1000;
			SysCtlDelay(delay*10);
		}
		if(state==1){
			state=0;
			red_i = 128;
			blue_i = min_value;
			green_i = min_value;
			while(1){
				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
				{
	            	// if switch 1 alone is pressed
					red_i--;
					if (red_i < min_value)
					{
						red_i = min_value;
					}
				}
				while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
					if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&state<4){
						//if both switch1 and switch 2 are pressed
						state++;
					}

					red_i++;
					if (red_i > max_value)
					{
						red_i = max_value;
					}
					SysCtlDelay(delay*10);
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green_i * ui32Load / 1000);
				if(state>1)
					break;
				SysCtlDelay(delay);
			}
		}
		else if (state==2){
			state=0;
			blue_i = 128;
			red_i = min_value;
			green_i = min_value;
			while(1){
				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
				{
					// if switch 1 alone is pressed
					blue_i--;
					if (blue_i < min_value)
					{
						blue_i = min_value;
					}
				}
				while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
					if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&state<4){
						//if both switch1 and switch 2 are pressed
						state++;
					}

					blue_i++;
					if (blue_i > max_value)
					{
						blue_i = max_value;
					}
					SysCtlDelay(delay*10);
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green_i * ui32Load / 1000);
				if(state!=0)
					break;
				SysCtlDelay(delay);
			}
		}
		else if(state==3){
			state=0;
			green_i = 127;
			blue_i = min_value;
			red_i = min_value;
			while(1){
				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
				{
					green_i--;
					if (green_i < min_value)
					{
						green_i = min_value;
					}
				}
				while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
					if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&state<4){
						state++;
					}

					green_i++;
					if (green_i > max_value)
					{
						green_i = max_value;
					}
					SysCtlDelay(delay*10);
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue_i * ui32Load / 1000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green_i * ui32Load / 1000);
				if(state!=0)
					break;
				SysCtlDelay(delay);
			}
		}
	}

 }
