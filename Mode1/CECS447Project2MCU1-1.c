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
//#include "UART2.h"
#include "LEDSW.h"

#define MAX_STR_LEN 20

// TODO: define bit values for all Colors 
#define RED 		(0x02)
#define BLUE 		(0x04)
#define GREEN 	(0x08)
#define PURPLE 	(RED|BLUE)
#define WHITE 	(RED|BLUE|GREEN)
#define DARK 		(0x00)
#define CRAN 		(RED|GREEN)

#define LED (*((volatile unsigned long*)0x40025038)) // address for green,blue,red LEDS

// TODO: define all colors in the color wheel
const	uint8_t color_wheel[] = {RED,BLUE,GREEN,PURPLE,WHITE,DARK,CRAN};
volatile int brightness = 100;

// TODO: define bit addresses for the onboard three LEDs and two switches
#define LEDs 		(0x0C)
#define SW1			()
#define SW2			()
	
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
uint8_t currentColor;
bool onScreen;

int main(void){
	DisableInterrupts();
  PLL_Init();
  UART_Init(false,false);  // for PC<->MCU1
//	UART2_Init();  // for MCU1<->MCU2
  PORTF_INIT();  // Initialize the onboard three LEDs and two push buttons
	EnableInterrupts();
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
				PWM_INIT();
				BrightnessMenu();
				break;
			case '3':
					onScreen = false;
					currentColor = DARK;
					setColorAndBrightness();
				break;
			default:
				break; 	
	}
	OutCRLF();
	
}
}

void Mode2(void){
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
		case DARK:
			Set_LED_Brightness(0,0,0);
			break; 
		case CRAN:
			Set_LED_Brightness(brightness,0,brightness);
	}
}