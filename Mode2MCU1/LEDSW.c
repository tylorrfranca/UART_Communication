/*
Project1Part1
LEDSW.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/10/2025
File Description : This file initilizes the LEDS, onboard switches, and pwm that is used to control the brightness of the LEDS
*/

#include "tm4c123gh6pm.h"
#include "LEDSW.h"
#include <stdint.h>
#include <stdbool.h>

// led & sw
#define PB4_MASK 0x10 // mask value for PB4
#define PF1_3_MASK 0x0E // mask value for PF1-3
#define PF0_4_SW 0x11 // mask value for PF4 Switch 1 and PF0 switch 2
#define PF_MASK 0x1F // mask value for PF0-4
#define PF_INT_MASK 0x40000000 // value to enable bit 30 in port f interrupt
#define PF_LONG_MASK 0x000FFFF0 //mask for port F values
#define PORTF0_LOCK 0x4C4F434B // value used to unlock portf PF0
#define LED (*((volatile unsigned long*)0x40025038)) // address for green,blue,red LEDS
#define LED_OFF 0x00 //turn off all LEDs
#define RED 0x02 // mask value for red onboard LED
#define BLUE 0x04  // mask value for blue onboard LED
#define GREEN 0x08 // mask value for green onboard LED
// pwm
#define PERIOD 10000				// Total PWM period

// Init for sw and LED
void PORTF_INIT(void){
SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF; // activate port F clock
    while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGC2_GPIOF)!=SYSCTL_RCGC2_GPIOF){} //wait for the clock to be ready
  GPIO_PORTF_LOCK_R = PORTF0_LOCK;  // unlock PortF PF0  
  GPIO_PORTF_CR_R |= PF_MASK;     // allow changes to PF4-0 :11111->0x1F     
  GPIO_PORTF_AMSEL_R &= ~PF_MASK;        // disable analog function
  GPIO_PORTF_PCTL_R &= ~PF_LONG_MASK;     // GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R &= ~PF0_4_SW;          // PF4 and PF0 input
	GPIO_PORTF_AFSEL_R &= ~PF0_4_SW;  //     disable alt funct on PF4,0			
  GPIO_PORTF_DIR_R |= PF1_3_MASK;      // PF1-3 output
  GPIO_PORTF_PUR_R |= PF0_4_SW;              // enable pullup resistors on PF4       
  GPIO_PORTF_DEN_R |= PF_MASK;              // enable digital pins PF1-PF4 
  GPIO_PORTF_IS_R &= ~PF0_4_SW;     // PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~PF0_4_SW;    // PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~PF0_4_SW;    // PF4 falling edge event
  GPIO_PORTF_ICR_R |= PF0_4_SW;     // clear flag4
  GPIO_PORTF_IM_R |= PF0_4_SW;      // arm interrupt on PF4 and PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00A00000; // PORTF Interrupt priority bits: 23-21, priority set to 5
	NVIC_EN0_R |= PF_INT_MASK;   // PORTF interrupt number is 30, enable bit 30 in NVIC.
}



void PWM_INIT(void){
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;           // Activate PWM1 module
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;         // Enable Port F clock
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGC2_GPIOF) == 0) {}; // Wait for GPIOF clock
//    
    GPIO_PORTF_AFSEL_R |= 0x0E;                      // Enable alternate function for PF1-3
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0;                // Clear PCTL bits for PF1-3
    GPIO_PORTF_PCTL_R |= 0x00005550;                 // Assign PF1-3 to M1PWM5, M1PWM6, M1PWM7
    GPIO_PORTF_AMSEL_R &= ~0x0E;                     // Disable analog functionality
    GPIO_PORTF_DEN_R |= 0x0E;                        // Enable digital function for PF1-3
    
    SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;            // Use PWM clock divider
    SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;            // Clear PWM divider field
    SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_2;             // Set PWM divider to 2 (PWM Clock = 25MHz)
    
    // Disable PWM generators before configuring
    PWM1_2_CTL_R &= ~PWM_1_CTL_ENABLE;  
    PWM1_3_CTL_R &= ~PWM_1_CTL_ENABLE;
    
    // Configure PWM for PF1 (Red) - M1PWM5 (GEN2, Output A)
    PWM1_2_GENB_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTLOAD_ZERO;
    PWM1_2_LOAD_R = PERIOD - 1;  
    PWM1_2_CMPB_R = PERIOD - 1;  // Default brightness

    // Configure PWM for PF2 (Blue) - M1PWM6 (GEN3, Output A)
    PWM1_3_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTLOAD_ZERO;
    PWM1_3_LOAD_R = PERIOD - 1;  
    PWM1_3_CMPA_R = PERIOD - 1;  // Default brightness
    
    // Configure PWM for PF3 (Green) - M1PWM7 (GEN3, Output B)
    PWM1_3_GENB_R = PWM_1_GENB_ACTCMPBD_ONE | PWM_1_GENB_ACTLOAD_ZERO;
    PWM1_3_CMPB_R = PERIOD - 1;  // Default brightness 

    // Enable PWM generators
    PWM1_2_CTL_R |= PWM_1_CTL_ENABLE;  
    PWM1_3_CTL_R |= PWM_1_CTL_ENABLE;  
    
    // Enable PWM outputs for PF1 (M1PWM5), PF2 (M1PWM6), PF3 (M1PWM7)
    PWM1_ENABLE_R |= 0xE0;  // 0xE0 = 0b11100000 (Enable bits 5, 6, 7)
}

void PWM_DISABLE(void){
 GPIO_PORTF_AFSEL_R &= ~0x0E;
}

bool PWM_STATUS(void){
	if(GPIO_PORTF_AFSEL_R == 0x0E){
		return true; 
	}
	else{
		return false;
	}
}

void PWM_ENABLE(void){
    PWM1_2_CTL_R |= PWM_1_CTL_ENABLE;  // Enable PWM1 Generator 2
  PWM1_3_CTL_R |= PWM_1_CTL_ENABLE;  // Enable PWM1 Generator 3
}


void Set_LED_Brightness(float brightness_red, float brightness_blue, float brightness_green) {
    if (brightness_red <= 0) brightness_red = 0;
    if (brightness_red >= 100) brightness_red = 99;
    
    if (brightness_blue <= 0) brightness_blue = 0;
    if (brightness_blue >= 100) brightness_blue = 99;

    if (brightness_green <= 0) brightness_green = 0;
    if (brightness_green >= 100) brightness_green = 99;

    uint16_t cmp_value_red = (uint16_t)(((uint32_t)brightness_red * (PERIOD - 1)) / 100);
    uint16_t cmp_value_blue = (uint16_t)(((uint32_t)brightness_blue * (PERIOD - 1)) / 100);
    uint16_t cmp_value_green = (uint16_t)(((uint32_t)brightness_green * (PERIOD - 1)) / 100);

    // Set correct PWM registers
    PWM1_2_CMPA_R = cmp_value_red;    // Red LED (PF1 - M1PWM5)
    PWM1_3_CMPA_R = cmp_value_blue;   // Blue LED (PF2 - M1PWM6)
    PWM1_3_CMPB_R = cmp_value_green;  // Green LED (PF3 - M1PWM7)
}

//void GPIOPortF_Handler(void) {
//    if (GPIO_PORTF_RIS_R & 0x10) {  // SW1 Pressed
//        for(int i = 0; i < 10000; i++); // Delay for debounce
//        GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag

//    }

//    if (GPIO_PORTF_RIS_R & 0x01) {  // SW2 Pressed
//        for(int i = 0; i < 10000; i++); // Delay for debounce
//        GPIO_PORTF_ICR_R = 0x01;  // Acknowledge flag
//				
//			}

//    }

