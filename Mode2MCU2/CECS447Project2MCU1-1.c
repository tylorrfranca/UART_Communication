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
const	uint8_t color_wheel[8] = {RED,BLUE,GREEN,PURPLE,WHITE,DARK,YELLOW,CRAN};
volatile uint8_t colorIndex = 0;
//    const uint8_t *colorNames[] = {
//        (uint8_t *)"Dark", (uint8_t *)"Red",   (uint8_t *)"Green", (uint8_t *)"Blue",
//        (uint8_t *)"Yellow",(uint8_t *)"Cran", (uint8_t *)"Purple",(uint8_t *)"White"
//    };

void UART2_Handler(void);

int idle_state = 1; // start off on idle state and change if received data from mcu1

extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
extern void DisableInterrupts(void);

//void Mode2(void) {
//    OutCRLF();
//    UART_OutString((uint8_t *)"Mode 2 MCU2: Waiting for color code from MCU1 ...");
//    OutCRLF();

//    // Wait for a color code from MCU1 via UART3
//    while(UART3_DataAvailable()) {
//			 UART_OutString((uint8_t *)"Dark, Red, Green, Blue, Yellow, Cran, Purple, White.");
//        // busy-wait
//    }
//    uint8_t receivedCode = UART3_InChar();
//    LED = receivedCode;  // Optionally display that color

//    // Enter color wheel
//    OutCRLF();
//    UART_OutString((uint8_t *)"Mode 2 MCU2: In Color Wheel State.");
//    OutCRLF();
//    UART_OutString((uint8_t *)"Please press sw2 to cycle through the colors:");
//    OutCRLF();
//    UART_OutString((uint8_t *)"Dark, Red, Green, Blue, Yellow, Cran, Purple, White.");
//    OutCRLF();
//    UART_OutString((uint8_t *)"Press sw1 to send the selected color to MCU1.");
//    OutCRLF();

//    // color arrays
//    const uint8_t colorWheel[] = { DARK, RED, GREEN, BLUE, YELLOW, CRAN, PURPLE, WHITE };
//    const uint8_t *colorNames[] = {
//        (uint8_t *)"Dark", (uint8_t *)"Red",   (uint8_t *)"Green", (uint8_t *)"Blue",
//        (uint8_t *)"Yellow",(uint8_t *)"Cran", (uint8_t *)"Purple",(uint8_t *)"White"
//    };
//    #define NUM_COLORS (sizeof(colorWheel)/sizeof(colorWheel[0]))
//    uint8_t currentIndex = 0;

//    // Show initial color
//    OutCRLF();
//    UART_OutString((uint8_t *)"Current color: ");
//    UART_OutString(colorNames[currentIndex]);
//    OutCRLF();

//    // color wheel loop
//    while(1) {
//        // if sw2 is pressed, cycle color
//        if(SW2_Pressed()) {
//            // simple debounce
//            for(volatile uint32_t i=0; i<500000; i++);
//            while(SW2_Pressed());
//            currentIndex = (currentIndex + 1) % NUM_COLORS;
//            LED = colorWheel[currentIndex];
//            OutCRLF();
//            UART_OutString((uint8_t *)"Current color: ");
//            UART_OutString(colorNames[currentIndex]);
//            OutCRLF();
//        }
//        // if sw1 is pressed, send color to MCU1, then exit mode 2
//        if(SW1_Pressed()) {
//            for(volatile uint32_t i=0; i<500000; i++);
//            while(SW1_Pressed());
//            UART3_OutChar(colorWheel[currentIndex]);
//            OutCRLF();
//            UART_OutString((uint8_t *)"Color sent to MCU1: ");
//            UART_OutString(colorNames[currentIndex]);
//            OutCRLF();
//            break;
//        }
//    }

//    // Turn off LED and return to main
//    LED = DARK;
//    OutCRLF();
//    UART_OutString((uint8_t *)"Exiting Mode 2. Returning to main menu...");
//    OutCRLF();
//}
void UART3_Handler(void){
  if(UART3_RIS_R&UART_RIS_RXRIS){       // received one item
        if ((UART3_FR_R&UART_FR_RXFE) == 0)
						idle_state = 0; // turn off idle state
            LED = UART3_DR_R&0xFF;
						current_color = LED;
						//((uint8_t *)"success");
						switch(LED){
							case GREEN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Green");
								break;
							case RED:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Red");
								break;
							case BLUE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Blue");
								break;
							case DARK:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Dark");
								break;
							case YELLOW:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Yellow");
								break;
							case CRAN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Cran");
								break;
							case PURPLE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Purple");
								break;
							case WHITE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: White");
								break;
							default:
								break;
						}
						UART3_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
  }
}


void GPIOPortF_Handler(void)
{		
	// simple debouncing code: generate 20ms to 30ms delay
	for (uint32_t time=0;time<200000;time++) {}
	
  if((GPIO_PORTF_RIS_R & SW2) && ((idle_state == 0)))
	{
		GPIO_PORTF_ICR_R = SW2;
		colorIndex = (colorIndex + 1) % 8;
		LED = color_wheel[colorIndex];
		OutCRLF();
		UART_OutString((uint8_t*)"");
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
							switch(LED){
							case GREEN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Green");
								break;
							case RED:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Red");
								break;
							case BLUE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Blue");
								break;
							case DARK:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Dark");
								break;
							case YELLOW:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Yellow");
								break;
							case CRAN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Cran");
								break;
							case PURPLE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Purple");
								break;
							case WHITE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: White");
								break;
							default:
								break;
						}
	}
	
	if((GPIO_PORTF_RIS_R & SW1) && ((idle_state == 0)))
	{
		idle_state = 1; // turn on idle state whenever we send 
	  OutCRLF();
		UART_OutString((uint8_t*)"");
		OutCRLF();
		UART_OutString((uint8_t*)"MCU Mode 2");
						switch(LED){
							case GREEN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Green");
								break;
							case RED:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Red");
								break;
							case BLUE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Blue");
								break;
							case DARK:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Dark");
								break;
							case YELLOW:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Yellow");
								break;
							case CRAN:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Cran");
								break;
							case PURPLE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: Purple");
								break;
							case WHITE:
							OutCRLF();
							UART_OutString((uint8_t*)" Current Color: White");
								break;
							default:
								break;
						}
		OutCRLF();
		UART_OutString((uint8_t*)"Waiting for color code from MCU1");
		GPIO_PORTF_ICR_R = SW1;
		UART3_OutChar(LED);		
	}
}

// ---------------------------------------------------------------------
// main()
// ---------------------------------------------------------------------
int main(void) {
		DisableInterrupts();
    PLL_Init();               // 50 MHz
    UART_Init(false, false);  // UART0 -> PC2 at 57,600 bps
    UART3_INIT(true, false); // UART3 -> MCU1 at 38,400 bps
    PORTF_INIT();             // Onboard LEDs and switches
		EnableInterrupts();
		OutCRLF();
    UART_OutString((uint8_t *)"Welcome to CECS 447 Project 2 - UART");
    OutCRLF();
    UART_OutString((uint8_t *)"MCU2");
    OutCRLF();
    UART_OutString((uint8_t *)"Waiting for command from MCU1 ...");
    OutCRLF();

    // Show the initial message for MCU2 on PC2
	while(1){
		WaitForInterrupt();
		
	}
    // Now just wait for a '2' from MCU1 to enter Mode2
//    while(1) {
//        if(UART3_DataAvailable()) {
//					UART_OutString((uint8_t *)"Welcome to CECS 447 Project 2 - UART");
//            char cmd = UART3_InChar();
//            // If MCU1 sends the character '2', that indicates "enter Mode2"
////            if(cmd == '2') {
////               // Mode2();
////                // After Mode2 finishes, show the initial message again
////                OutCRLF();
////                UART_OutString((uint8_t *)"Welcome to CECS 447 Project 2 - UART");
////                OutCRLF();
////                UART_OutString((uint8_t *)"MCU2");
////                OutCRLF();
////                UART_OutString((uint8_t *)"Waiting for command from MCU1 ...");
////                OutCRLF();
////            }
//        }
//    }
}
