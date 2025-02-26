/*
Project1Part1
CECS447Project2MCU1-1.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/13/2025
File Description : This program will set up a pc1 <-> mcu1 connection that will allow us to turn on the led with different colors
alongside change the brightness through the terra term program.
*/

#include "PLL.h"
#include "UART.h"
#include "UART2.h"
#include "LEDSW.h"
#include "tm4c123gh6pm.h"

#define MAX_STR_LEN 20

// TODO: define bit values for all Colors 
#define DARK 		(0x00)
#define RED 		(0x02)
#define BLUE 		(0x04)
#define GREEN 	(0x08)
#define PURPLE 	(0x06)
#define YELLOW  (0x0A)
#define WHITE 	(0x0E)
#define CRAN 		(0x0C)


#define LED (*((volatile unsigned long*)0x40025038)) // address for green,blue,red LEDS
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))


// TODO: define all colors in the color wheel
const	uint8_t color_wheel[8] = {RED,BLUE,GREEN,PURPLE,WHITE,DARK,YELLOW,CRAN};
volatile int brightness = 100;
bool idle_state = false; 

// TODO: define bit addresses for the onboard three LEDs and two switches
#define LEDs 		(0x0C)
#define SW1			(0x10)
#define SW2			(0x01)
	
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
extern void DisableInterrupts(void);
void Mode1(void);
void Mode2(void);
void Mode3(void);
void Display_Menu(void);

void mode1Menu(void);
void LEDColorMenu(void);
void BrightnessMenu(void);
void setColorAndBrightness(void);
void waitDisplay(void);
void mode2Display(void);


volatile uint8_t currentColor;
volatile uint8_t colorIndex = 0; 
bool onScreen;

int main(void){
	DisableInterrupts();
  PLL_Init();
  UART0_Init(false,false);  // for PC<->MCU1
  UART2_Init(true,false);  // for MCU1<->MCU2
  PORTF_INIT();  // Initialize the onboard three LEDs and two push buttons
	
  while(1){
		LED = DARK;
		currentColor = DARK;
		// displays the main menu		
		Display_Menu(); 
		switch(UART_InChar()){
			case '1':
				Mode1();
				break;
			case '2':
				Mode2();
				break;
			case '3': 
				Mode3();
				break;
			default:
				break; 
		}
		OutCRLF();
  }
}

void Mode1(void){
	onScreen = true;
	while(onScreen){
		mode1Menu();
		switch(UART_InChar()){
			case '1':
				LEDColorMenu();
				break;
			case '2':
				BrightnessMenu();
				break;
			case '3':
					onScreen = false;
					currentColor = DARK;
					if(PWM_STATUS()){
						setColorAndBrightness();
					}
				break;
			default:
				break; 	
	}
	OutCRLF();
	
}
}

void Mode2(void){
	EnableInterrupts();
	onScreen = true; 
	mode2Display();
	UART2_OutChar('2');
	while(onScreen){
		WaitForInterrupt();
	switch(currentColor){
	case DARK:
		OutCRLF();
		UART_OutString((uint8_t *)"Dark LED is on");
		break;
	case RED:
		OutCRLF();
		UART_OutString((uint8_t *)"Red LED is on");
		break;
	case GREEN:
		OutCRLF();
		UART_OutString((uint8_t *)" Green LED is on");
		break;
	case BLUE:
		OutCRLF();
		UART_OutString((uint8_t *)" Blue LED is on");
		break;
	case CRAN:
		OutCRLF();
		UART_OutString((uint8_t *)" Cran LED is on");
		break;
	case PURPLE:
		OutCRLF();
		UART_OutString((uint8_t *)" Purple LED is on");
		break;
	case WHITE:
		OutCRLF();
		UART_OutString((uint8_t *)" White LED is on");
		break;
	case YELLOW:
		OutCRLF();
		UART_OutString((uint8_t *)" Yellow LED is on");
		break;
	default:
		break; 
	}

		
	}
	
}

void Mode3(void){
}

void Display_Menu(void){
	OutCRLF();
  UART_OutString((uint8_t *)" Welcome to CECS 447 Project 2 - UART");
	OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" MCU 1");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Main Menu");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" 1. PC <-> MCU1 LED CONTROL");
  OutCRLF();
	UART_OutString((uint8_t *)" 2. MCU1 <-> MCU2 COLOR WHEEL");
  OutCRLF();
	UART_OutString((uint8_t *)" 3. PC1 <-> MCU1 <-> MCU2 <-> PC2 CHAT ROOM");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Please choose a communication mode");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" (enter 1, 2, or 3): ");
  OutCRLF();
  UART_OutChar(' ');
  OutCRLF();
  UART_OutChar('-');
  UART_OutChar('-');
  UART_OutChar('>');
  OutCRLF();
}

void mode1Menu(void){
	OutCRLF();
  UART_OutString((uint8_t *)" Mode One Menu");
	OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Please select an option below ( enter 1, 2, or 3)");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" 1. CHOOSE LED COLOR");
  OutCRLF();
	UART_OutString((uint8_t *)" 2. CHANGE BRIGHTNESS OF CURRENT LED");
  OutCRLF();
	UART_OutString((uint8_t *)" 3. EXIT");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Please choose a communication mode");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" (enter 1, 2, or 3): ");
  OutCRLF();
  UART_OutChar(' ');
  OutCRLF();
  UART_OutChar('-');
  UART_OutChar('-');
  UART_OutChar('>');
  OutCRLF();
};

void mode2Display(void){
	OutCRLF();
  UART_OutString((uint8_t *)" Mode Two MCU1: press ^ to exit this mode at any time");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Please press sw2 to go through the colors in the following color wheel:");
  OutCRLF();
	UART_OutString((uint8_t *)" Dark, Red, Green, Blue, Yellow, Cran, Purple, White.");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" Once a color is selected, press sw1 to send the color to MCU2");
  OutCRLF();
	OutCRLF();
};

void waitDisplay(void){
	OutCRLF();
  UART_OutString((uint8_t *)" Mode Two MCU1: press ^ to exit this mode at any time");
  OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" current color: need to add");
  OutCRLF();
	UART_OutString((uint8_t *)" Waiting for color code from MCU2...");
  OutCRLF();
	OutCRLF();
}

void LEDColorMenu(void){
	bool onLEDScreen = true;
	while(onLEDScreen){
	OutCRLF();
  UART_OutString((uint8_t *)" Please Select a color from the following list:");
	OutCRLF();
	OutCRLF();
	UART_OutString((uint8_t *)" d(dark), r(red), g(green), b(blue), c(cran), p(purple), w(white), x(exit)");
  OutCRLF();
	OutCRLF();
	switch(UART_InChar()){
	case 'd':
		LED = DARK;
		currentColor = DARK;
		OutCRLF();
		UART_OutString((uint8_t *)"Dark LED is on");
		break;
	case 'r':
		LED = RED;
		currentColor = RED;
		OutCRLF();
		UART_OutString((uint8_t *)"Red LED is on");
		break;
	case 'g':
		LED = GREEN;
		currentColor = GREEN;
		OutCRLF();
		UART_OutString((uint8_t *)" Green LED is on");
		break;
	case 'b':
		LED = BLUE;
		currentColor = BLUE;
		OutCRLF();
		UART_OutString((uint8_t *)" Blue LED is on");
		break;
	case 'c':
		LED = CRAN;
		currentColor = CRAN;
		OutCRLF();
		UART_OutString((uint8_t *)" Cran LED is on");
		break;
	case 'p':
		LED = PURPLE;
		currentColor = PURPLE;
		OutCRLF();
		UART_OutString((uint8_t *)" Purple LED is on");
		break;
	case 'w':
		LED = WHITE;
		currentColor = WHITE;
		OutCRLF();
		UART_OutString((uint8_t *)" White LED is on");
		break;
	case 'x':
		onLEDScreen = false;
		break;
	default:
		break; 
	}
	PWM_INIT();
	setColorAndBrightness();
	}
}

void BrightnessMenu(){
	setColorAndBrightness();
	UART_OutString((uint8_t *)" Please enter a brightness level between(1-100):");
  OutCRLF();
	OutCRLF();
  brightness = UART_InUDec();
	setColorAndBrightness();
}

void setColorAndBrightness(void){
			switch(currentColor){
		case GREEN:
			Set_LED_Brightness(0,0,brightness);
			break;
		case RED:
			Set_LED_Brightness(brightness,0,0);
			break;
		case BLUE:
			Set_LED_Brightness(0,brightness,0);
			break;
		case PURPLE:
			Set_LED_Brightness(brightness,brightness,0);
			break;
		case WHITE:
			Set_LED_Brightness(brightness,brightness,brightness);
			break;
		case DARK:
			Set_LED_Brightness(0,0,0);
			break; 
		case YELLOW:
			Set_LED_Brightness(brightness,0,brightness);
			break;
		case CRAN:
			Set_LED_Brightness(0,brightness,brightness);
			break;
		default:
			break;
	}
}

void GPIOPortF_Handler(void) {
		for(int i = 0; i < 100000; i++){}; // Delay for debounce
		if(!idle_state){
			if (GPIO_PORTF_RIS_R & 0x01){  // SW2 Pressed
					colorIndex = (colorIndex + 1) % 8;
					currentColor = color_wheel[colorIndex];
					LED = currentColor;
					GPIO_PORTF_ICR_R = 0x01;  // Acknowledge flag
			}
			if (GPIO_PORTF_RIS_R & 0x10) {  // SW1 Pressed
					OutCRLF();
					UART2_OutChar(currentColor);
					UART_OutString((uint8_t *)" Color sent to MCU2");
					OutCRLF();
					GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag
					idle_state = true; 
			}
		}
	}

//void UART0_Handler(void){
//  if(UART0_RIS_R&UART_RIS_RXRIS){       // received one item
//		if ((UART2_FR_R&UART_FR_RXFE) == 0)
//		  LED = UART0_DR_R&RED;//		  led_on = UART1_DR_R&RED;
//    UART1_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
//  }
			
//}

void UART2_Handler(void){
  if(idle_state){       // received one item 
		if(UART2_RIS_R&UART_RIS_RXRIS){
			if ((UART2_FR_R&UART_FR_RXFE) == 0)
				currentColor = UART2_DR_R & 0xFF;
				LED = currentColor;
				UART2_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
				idle_state = false; 
		}
  }
}



	