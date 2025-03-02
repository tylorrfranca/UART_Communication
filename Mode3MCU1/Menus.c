///*
//Project1Part1
//UART.c
//Runs on LM4F120/TM4C123
//Starter File for CECS 447 Project 2 UART Communications
//Project Group Number: 1
//Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
//Date: 02/13/2025
//File Description : This program takes care of all of the UART connections at a baud rate of 57600 and connects the mcu to our terra term program.
//*/

//#include "PLL.h"
//#include "UART.h"
////#include "UART2.h"
//#include "LEDSW.h"
//#include "Menus.h"

//void Display_Menu(void){
//	OutCRLF();
//  UART_OutString((uint8_t *)" Welcome to CECS 447 Project 2 - UART");
//	OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" MCU 1");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" Main Menu");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" 1. PC <-> MCU1 LED CONTROL");
//  OutCRLF();
//	UART_OutString((uint8_t *)" 2. MCU1 <-> MCU2 COLOR WHEEL");
//  OutCRLF();
//	UART_OutString((uint8_t *)" 3. PC1 <-> MCU1 <-> MCU2 <-> PC2 CHAT ROOM");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" Please choose a communication mode");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" (enter 1, 2, or 3): ");
//  OutCRLF();
//  UART_OutChar(' ');
//  OutCRLF();
//  UART_OutChar('-');
//  UART_OutChar('-');
//  UART_OutChar('>');
//  OutCRLF();
//}

//void mode1Menu(void){
//	OutCRLF();
//  UART_OutString((uint8_t *)" Mode One Menu");
//	OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" Please select an option below ( enter 1, 2, or 3)");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" 1. CHOOSE LED COLOR");
//  OutCRLF();
//	UART_OutString((uint8_t *)" 2. CHANGE BRIGHTNESS OF CURRENT LED");
//  OutCRLF();
//	UART_OutString((uint8_t *)" 3. EXIT");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" Please choose a communication mode");
//  OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" (enter 1, 2, or 3): ");
//  OutCRLF();
//  UART_OutChar(' ');
//  OutCRLF();
//  UART_OutChar('-');
//  UART_OutChar('-');
//  UART_OutChar('>');
//  OutCRLF();
//};

//void LEDColorMenu(void){
//	bool onLEDScreen = true;
//	while(onLEDScreen){
//	OutCRLF();
//  UART_OutString((uint8_t *)" Please Select a color from the following list:");
//	OutCRLF();
//	OutCRLF();
//	UART_OutString((uint8_t *)" d(dark), r(red), g(green), b(blue), c(cran), p(purple), w(white), x(exit)");
//  OutCRLF();
//	OutCRLF();
//	switch(UART_InChar()){
//	case 'd':
//		LED = DARK;
//		currentColor = DARK;
//		OutCRLF();
//		UART_OutString((uint8_t *)"Dark LED is on");
//		break;
//	case 'r':
//		LED = RED;
//		currentColor = RED;
//		OutCRLF();
//		UART_OutString((uint8_t *)"Red LED is on");
//		break;
//	case 'g':
//		LED = GREEN;
//		currentColor = GREEN;
//		OutCRLF();
//		UART_OutString((uint8_t *)" Green LED is on");
//		break;
//	case 'b':
//		LED = BLUE;
//		currentColor = BLUE;
//		OutCRLF();
//		UART_OutString((uint8_t *)" Blue LED is on");
//		break;
//	case 'c':
//		LED = CRAN;
//		currentColor = CRAN;
//		OutCRLF();
//		UART_OutString((uint8_t *)" Cran LED is on");
//		break;
//	case 'p':
//		LED = PURPLE;
//		currentColor = PURPLE;
//		OutCRLF();
//		UART_OutString((uint8_t *)" Purple LED is on");
//		break;
//	case 'w':
//		LED = WHITE;
//		currentColor = WHITE;
//		OutCRLF();
//		UART_OutString((uint8_t *)" White LED is on");
//		break;
//	case 'x':
//		onLEDScreen = false;
//		break;
//	default:
//		break; 
//	}
//	PWM_INIT();
//	setColorAndBrightness();
//	}
//}

//void BrightnessMenu(){
//	setColorAndBrightness();
//	UART_OutString((uint8_t *)" Please enter a brightness level between(1-100):");
//  OutCRLF();
//	OutCRLF();
//  brightness = UART_InUDec();
//	setColorAndBrightness();
//}