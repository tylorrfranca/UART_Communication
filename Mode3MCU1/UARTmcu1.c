/*
Project 2 Part 1 & 2
UARTmcu1.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/28/2025
File Description : This program facilitates communication between a PC and MCU1 using UART0, as well as 
MCU1-to-MCU2 communication via UART2. The system supports LED control, brightness adjustment, 
and an interactive color selection mode.
*/

#include "PLL.h"
#include "UART.h"
#include "UART2.h"
#include "LEDSW.h"
#include "tm4c123gh6pm.h"

#define MAX_STR_LEN 20

// TODO: define bit values for all Colors 
#define DARK 		0x00
#define RED 		0x02
#define BLUE 		0x04
#define GREEN 	0x08
#define PURPLE 	0x06
#define YELLOW  0x0A
#define WHITE 	0x0E
#define CRAN 		0x0C


#define LED (*((volatile unsigned long*)0x40025038)) // address for green,blue,red LEDS
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))


// TODO: define all colors in the color wheel
const	uint8_t color_wheel[8] = {DARK,RED,GREEN,BLUE,YELLOW,CRAN,PURPLE,WHITE};
const uint8_t *colorNames[] = {
        (uint8_t*)"Dark", (uint8_t*)"Red",   (uint8_t*)"Green", (uint8_t*)"Blue",
        (uint8_t*)"Yellow",(uint8_t*)"Cran", (uint8_t*)"Purple",(uint8_t*)"White"
    };

volatile int brightness = 100;
bool color_received = false; 
bool color_sent = false; 
bool message_sent = false; 
bool message_received = false; 
bool Mode2Flag = false; 
bool Mode3Flag = false; 
uint8_t string[255];
uint8_t string2[255];

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
void mode2IdleDisplay(void);
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
  UART0_Init(true,false);  // for PC<->MCU1
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
	LED = DARK;
	
}
}

void Mode2(void){
	PWM_DISABLE();
	PORTF_INIT();
	UART2_OutChar('2');
	EnableInterrupts();
	Mode2Flag = true; 
	while(Mode2Flag){
		color_sent = false; 
		mode2Display();
		while(!color_sent && Mode2Flag){
			WaitForInterrupt();
		}
		
		color_received = false;
		mode2IdleDisplay();
		while(!color_received && Mode2Flag){
			WaitForInterrupt();
		}
		for(int i = 0; i < 8; i++){
			if(LED == color_wheel[i]){
				colorIndex = i; 
			}
		}
	}
	
}

void Mode3(void){
	UART2_OutChar('3');
	EnableInterrupts();
	Mode3Flag = true; 
	while(Mode3Flag){
//		message_sent = false; 
//		//mode3Display();
//		while(!message_sent && Mode3Flag){
			UART_OutString((uint8_t *)" Please Write Message: ");
			OutCRLF();
			UART_InString(string, 254); 
			OutCRLF();
			UART2_OutString(string);
//		}
//		message_received = false;
//		//mode3IdleDisplay();
//		while(!message_received && Mode2Flag){
//			WaitForInterrupt();
//		}	
	}
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
	UART_OutString((uint8_t *)" Current Color: ");
	UART_OutString((uint8_t *) colorNames[colorIndex]);
	OutCRLF();
};

void mode2IdleDisplay(void){
	OutCRLF();
  UART_OutString((uint8_t *)" Mode Two MCU1: press ^ to exit this mode at any time");
  OutCRLF();
	UART_OutString((uint8_t *)" Current Color: ");
	UART_OutString((uint8_t *) colorNames[colorIndex]);
	OutCRLF();
	UART_OutString((uint8_t *)" Waiting for color code from MCU2...");
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
	case 'y':
		LED = YELLOW;
		currentColor = YELLOW;
		OutCRLF();
		UART_OutString((uint8_t *)" Yellow LED is on");
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
	default:
		break; 
	}
	PWM_INIT();
	setColorAndBrightness();
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
		for(int i = 0; i < 200000; i++){}; // Delay for debounce
		if(!color_sent && Mode2Flag){
			if (GPIO_PORTF_RIS_R & 0x01){  // SW2 Pressed
					GPIO_PORTF_ICR_R = 0x01;  // Acknowledge flag
					colorIndex = (colorIndex + 1) % 8;
					LED = color_wheel[colorIndex];
				  OutCRLF();
					UART_OutString((uint8_t *)" Current Color: ");
					UART_OutString((uint8_t *) colorNames[colorIndex]);
					OutCRLF();
			}
			if (GPIO_PORTF_RIS_R & 0x10) {  // SW1 Pressed
					GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag 
					OutCRLF();
					UART2_OutChar(LED);
					OutCRLF(); 
					color_sent = true;
			}
		}
	}

void UART0_Handler(void){
  if(UART0_RIS_R&UART_RIS_RXRIS){       // received one item
		if ((UART0_FR_R&UART_FR_RXFE) == 0)
		   if((UART0_DR_R & 0xFF) == '^'){
					UART2_OutChar('^');
					Mode2Flag = false;
					
			 }
    UART0_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
  }
			
}

void UART2_Handler(void){
	for(int i = 0; i < 200000; i++){}; // Delay for debounce
  if(!color_received && Mode2Flag){       // received one item 
		if(UART2_RIS_R&UART_RIS_RXRIS){
			if ((UART2_FR_R&UART_FR_RXFE) == 0)
				LED = UART2_DR_R & 0xFF;
				UART2_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
				color_received = true;
		}
  }
//	if(!message_received && Mode3Flag){       // received one item 
//		if(UART2_RIS_R&UART_RIS_RXRIS){
//			if ((UART2_FR_R&UART_FR_RXFE) == 0)
//				string2 = UART2_DR_R & 0xFF;
//				UART2_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
//				color_received = true;
//		}
//  }
}



	