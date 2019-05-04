/**
  EPWM Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    epwm.c

  @Summary
    This is the generated driver implementation file for the EPWM driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides implementations for driver APIs for EPWM.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC12F1840
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
         MPLAB 	          :  MPLAB X 4.15
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include "epwm.h"

/**
  Section: Macro Declarations
*/

#define PWM_INITIALIZE_DUTY_VALUE    511

/**
  Section: EPWM Module APIs
*/

void EPWM_Initialize(void)
{
    // Set the EPWM to the options selected in the User Interface
	
	// CCP1M P1A: active high; P1B: active high; DC1B 3; P1M single; 
	CCP1CON = 0x3C;    
	
	// CCP1ASE operating; PSS1BD low; PSS1AC low; CCP1AS disabled; 
	ECCP1AS = 0x00;    
	
	// P1RSEN automatic_restart; P1DC 0; 
	PWM1CON = 0x80;    
	
	// STR1B P1B_to_port; STR1A P1A_to_CCP1M; STR1SYNC start_at_begin; 
	PSTR1CON = 0x01;    
	
	// CCPR1H 0; 
	CCPR1H = 0x00;    
	
	// CCPR1L 127; 
	CCPR1L = 0x7F;    

}

void EPWM_LoadDutyValue(uint16_t dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR1L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP1CON = ((uint8_t)(CCP1CON & 0xCF) | ((dutyValue & 0x0003)<<4));
}
/**
 End of File
*/

