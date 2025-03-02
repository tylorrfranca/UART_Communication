/*
Project2MCU2Mode2
UARTmcu2.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/28/2025
File Description : This program will set up a pc2 <-> mcu2 connection that will allow us to connect our pc through uart0. We also connect mcu2 to mcu1 through uart3
that will allow us to change the LED colors thorugh the onboard switches and will update the colors on our pc display.
*/

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART.h"      // UART0 for PC2 communication at 57,600 bps
#include "UART3_INIT.h"     // UART3 for inter–MCU communication at 38,400 bps
#include "LEDSW.h"     // PortF for onboard LEDs/switches
#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// Color definitions (LED bit masks)
// ---------------------------------------------------------------------
#define DARK     0x00
#define RED      0x02
#define BLUE     0x04
#define GREEN    0x08
#define PURPLE   0x06
#define YELLOW   0x0A
#define CRAN     0x0C
#define WHITE    0x0E

#define SW1       0x10
#define SW2       0x01

uint8_t current_color;
const	uint8_t color_wheel[8] = {DARK, RED, GREEN, BLUE, YELLOW, CRAN, PURPLE, WHITE};
volatile uint8_t colorIndex = 0;
    const uint8_t *colorNames[] = {
        (uint8_t *)"Dark", (uint8_t *)"Red",   (uint8_t *)"Green", (uint8_t *)"Blue",
        (uint8_t *)"Yellow",(uint8_t *)"Cran", (uint8_t *)"Purple",(uint8_t *)"White"
    };


bool Mode2Flag = false;
bool Mode3Flag = false;
bool color_sent = false;
bool color_recieved = false; 
bool firstRound = true; 


extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
extern void DisableInterrupts(void);
void Mode2(void);
void Mode3(void);
void System_Init(void);
void Mode2SendDisplay(void);
void Mode2ReceiveDisplay(void);
void Mode2InitialDisplay(void);
void StartingDisplay(void);

int main(void) {
		System_Init();
    OutCRLF();		
		while(1){
			StartingDisplay();
			switch(UART3_InChar()){
				case '2':
					Mode2();
					break;
				default:
					break;
			}					
	}
}


void Mode2(void){
	EnableInterrupts();
	Mode2Flag = true;
	Mode2InitialDisplay();
	while(Mode2Flag){
		if(!firstRound){
			Mode2ReceiveDisplay();
		}
		firstRound = false; 
		color_recieved = false;
		while(!color_recieved && Mode2Flag){
			WaitForInterrupt();
		}
		for(int i = 0; i < 8; i++){
			if(LED == color_wheel[i]){
				colorIndex = i;
			}
		}
		if (Mode2Flag == false){
			LED = DARK;
			return;
		}
		
		color_sent = false;
		Mode2SendDisplay();
		while(!color_sent && Mode2Flag){
			WaitForInterrupt();
		}
		if (Mode2Flag == false){
			LED = DARK;
			return;
		}
	}
}


void Mode3(void){
}

void Beginning_Prompt(void){
}


void UART3_Handler(void){
// simple debouncing code: generate 20ms to 30ms delay
for (uint32_t time=0;time<200000;time++) {}
if (Mode2Flag){
		if(UART3_RIS_R&UART_RIS_RXRIS){       // received one item
        if ((UART3_FR_R&UART_FR_RXFE) == 0)
					if ((UART3_DR_R&0xFF) == 0x5E){
						Mode2Flag = false;
						OutCRLF();
					}
					LED = UART3_DR_R&0xFF;
					color_recieved = true;
					UART3_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
			}
	}
}
void GPIOPortF_Handler(void){	
// simple debouncing code: generate 20ms to 30ms delay
for (uint32_t time=0;time<200000;time++) {}
if (!color_sent && Mode2Flag){	
		if(GPIO_PORTF_RIS_R & SW2){
			GPIO_PORTF_ICR_R = SW2;
			colorIndex = (colorIndex + 1) % 8;
			LED = color_wheel[colorIndex];
			OutCRLF();
			UART_OutString((uint8_t*)"Current Color: ");
			UART_OutString((uint8_t*) colorNames[colorIndex]);
			}
		
		if(GPIO_PORTF_RIS_R & SW1){ 		
			GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag
			UART3_OutChar(LED);
			color_sent = true;
		}
	}
}

void System_Init(void){
		DisableInterrupts();
    PLL_Init();               // 50 MHz
    UART_Init(false, false);  // UART0 -> PC2 at 57,600 bps
    UART3_INIT(true, false); // UART3 -> MCU1 at 38,400 bps
    PORTF_INIT();             // Onboard LEDs and switches
}



void StartingDisplay (void){
	  UART_OutString((uint8_t *)"Welcome to CECS 447 Project 2 - UART");
    OutCRLF();
    UART_OutString((uint8_t *)"MCU2");
    OutCRLF();
    UART_OutString((uint8_t *)"Waiting for command from MCU1 ...");
    OutCRLF();		
}

void Mode2SendDisplay (void){
	OutCRLF();
	UART_OutString((uint8_t*)"Mode 2 MCU2");
	OutCRLF();
	UART_OutString((uint8_t*)"In color wheel state");
	OutCRLF();
	UART_OutString((uint8_t*)"Please press sw2 to go through the colors");
	OutCRLF();
	UART_OutString((uint8_t*)"in the following color wheel: Dark, Red,");
	OutCRLF();
	UART_OutString((uint8_t*)"Green, Blue, Yellow, Cran, Purple, White.");
	OutCRLF();
	UART_OutString((uint8_t*)"Once a color is selected, press sw1 to send the color to MCU1");
	OutCRLF();
	UART_OutString((uint8_t*)"Current Color: ");
	UART_OutString((uint8_t*) colorNames[colorIndex]);
}

void Mode2ReceiveDisplay(void){
	OutCRLF();
  UART_OutString((uint8_t *)"Mode 2 MCU2");
	OutCRLF();
	UART_OutString((uint8_t*)"Current Color: ");
	UART_OutString((uint8_t*) colorNames[colorIndex]);
	OutCRLF();	
  UART_OutString((uint8_t *)"Waiting for color code from MCU1 ...");
	OutCRLF();
}

void Mode2InitialDisplay(void){
	OutCRLF();
  UART_OutString((uint8_t *)"Mode 2 MCU2");
	OutCRLF();
  UART_OutString((uint8_t *)"Waiting for color code from MCU1 ...");
	OutCRLF();
}