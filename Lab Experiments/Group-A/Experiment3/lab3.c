/*
* Team id: 15
* Edited by: Nishanth Rumandla, Dheeraj Reddy Kalmekolen

* Filename: lab3.c

* Functions: main()

* Global Variables: ui8AdjustR, ui8AdjustB, ui8AdjustG, ui8AdjustMax, ui8AdjustMin, angle, n ,k, k2, inc

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

/*

* Global Variables

*/

volatile uint8_t ui8AdjustR;
volatile uint8_t ui8AdjustB;
volatile uint8_t ui8AdjustG;
volatile uint8_t ui8AdjustMax;
volatile uint8_t ui8AdjustMin;
volatile uint16_t angle;
int n,k,k2;
int inc;

/*

* Function Name: main()

* Input: none

* Output: none

* Description: use the PWM on the TivaC Series device to control the brightness of LED present in the development board. 

* Example Call: automatically called

*/
int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    angle = 0;
    ui8AdjustMax = 244;
    ui8AdjustMin = 10;
    ui8AdjustR = ui8AdjustMax;
    ui8AdjustB = 0;
    ui8AdjustG = 0;
    inc=5;

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
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    n=0;
    while(1)
    {
        while(n==0){

            angle = angle + inc;
            if(angle>=360){
                angle = 0;
            }
            if(angle<120){// angle 0 to 120 : blue component remains same
                ui8AdjustG = ui8AdjustMin + angle*ui8AdjustMax/120.0;
                ui8AdjustR = ui8AdjustMin + ui8AdjustMax - ui8AdjustG;
                ui8AdjustB = ui8AdjustMin;
            } else if (angle<240){// angle 120 to 240 : red component remains same
                ui8AdjustB = ui8AdjustMin+(angle-120)*ui8AdjustMax/120.0;
                ui8AdjustG = ui8AdjustMin+ui8AdjustMax - ui8AdjustB;
                ui8AdjustR = ui8AdjustMin;
            } else if(angle<360){// angle 240 to 360 : green component remains same
                ui8AdjustR = ui8AdjustMin+(angle-240)*ui8AdjustMax/120.0;
                ui8AdjustB = ui8AdjustMin+ui8AdjustMax - ui8AdjustR;
                ui8AdjustG = ui8AdjustMin;
            }
            // set colour using the values in ui8AdjustR, ui8AdjustB, ui8AdjustG values
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

            SysCtlDelay(1000000);
            if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&(!(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00))){
            	// if switch 1 alone is pressed
                inc--;
                if(inc<1) inc = 1;
                if(inc>10) inc = 10;
                SysCtlDelay(10000000);
            }
            k=0, k2=0;
            while((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)){
                if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
                    //if both switch1 and switch 2 are pressed
                    k++;
                    SysCtlDelay(10000000);
                }
                k2=1;

            }
            if((k==0)&&(k2==1)){
                inc++;
                if(inc<1) inc = 1;
                if(inc>10) inc = 10;
                SysCtlDelay(10000000);
            }
            else if (k2==1){
                n=k;
                break;
            }
            k2++;//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);

        }
       
        if(n==1){
            ui8AdjustR = 127;
            ui8AdjustB = 10;
            ui8AdjustG = 10;
            while(n==1){
                if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&!(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00))
                {
                // if switch 1 alone is pressed
                    ui8AdjustR -= 8;
                    if (ui8AdjustR < 10)
                   {
                        ui8AdjustR = 10;
                    }
                    SysCtlDelay(10000000);
                }
                k=0,k2=0;
                while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
                    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
                    //if both switch1 and switch 2 are pressed
                        k++;SysCtlDelay(10000000);
                    }
                    k2=1;
                }

                if(k==0 && k2==1)
                {
                    ui8AdjustR+=8;
                    if (ui8AdjustR > 245)
                    {
                        ui8AdjustR = 245;
                    }
                }
                else if (k2==1&&k!=0){
                    n=k;
                }
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

                SysCtlDelay(100000);
            }
        }
        else if (n==2){
            ui8AdjustB = 127;
            ui8AdjustR = 10;
            ui8AdjustG = 10;
            while(n==2){
                if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&!(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00))
                {
                // if switch 1 alone is pressed
                    ui8AdjustB-=8;
                    if (ui8AdjustB < 10)
                    {
                        ui8AdjustB = 10;
                    }
                    SysCtlDelay(10000000);
                }
                k=0,k2=0;
                while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
                    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
                    //if both switch1 and switch 2 are pressed
                        k++;
                        SysCtlDelay(10000000);
                    }
                    k2=1;

                }

                if(k==0 && k2==1)
                {
                    ui8AdjustB+=8;
                    if (ui8AdjustB > 245)
                    {
                        ui8AdjustB = 245;
                    }
                }
                else if (k2==1&&k!=0){
                    n=k;
                }
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

                SysCtlDelay(100000);
            }
        }
        else if(n>2){
            ui8AdjustG = 127;
            ui8AdjustB = 10;
            ui8AdjustR = 10;
            while(n>2){
                if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)&&!(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00))
                {
                // if switch 1 alone is pressed
                    ui8AdjustG-=8;
                    if (ui8AdjustG < 10)
                    {
                        ui8AdjustG = 10;
                    }
                    SysCtlDelay(10000000);
                }

                k=0,k2=0;
                while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
                    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
                    //if both switch1 and switch 2 are pressed
                        k++;SysCtlDelay(10000000);
                    }
                    k2=1;

                }


                if(k==0 && k2==1)
                {
                    ui8AdjustG+=8;
                    if (ui8AdjustG > 245)
                    {
                        ui8AdjustG = 245;
                    }
                }
                else if (k2==1&&k!=0){
                    n=k;
                }
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

                SysCtlDelay(100000);
            }
        }

    }

}
