/*
Project1Part2
UART2.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/13/2025
File Description : 
*/

#include "tm4c123gh6pm.h"
#include "UART2.h"

#define NVIC_EN1_UART2 0x02

void UART2_Init(bool RxInt, bool TxInt){
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART2; // activate UART2
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate port D
   	while((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOD) == 0){}; 
		UART2_CTL_R = 0;
	
  UART2_IBRD_R = 81;                    // IBRD = int(50,000,000 / (16 * 38400)) = int(81.38)
  UART2_FBRD_R = 24;                     // FBRD = int(0.04 * 64 + 0.5) = 3.1
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART2_LCRH_R = UART_LCRH_WLEN_8;
	if (RxInt) {
	  UART2_IM_R |= UART_IM_RXIM;         // Enable RX interrupt
	}
	
	if (TxInt) {
	  UART2_IM_R |= UART_IM_TXIM;         // Enable TX interrupt
	}
  UART2_CTL_R |= UART_CTL_UARTEN|UART_CTL_RXE|UART_CTL_TXE; // enable UART, Rx, Tx
	
	if ( RxInt | TxInt) {
		NVIC_PRI8_R = (NVIC_PRI8_R&0xFFFF1FFF)|0x0000A000; // bits 15-13, priority 5
		NVIC_EN1_R = NVIC_EN1_UART2;           // enable interrupt 5 in NVIC
	}
	GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY; // 2) unlock PortD PD6 and 7
	GPIO_PORTD_CR_R |= 0xC0;           // allow changes to PD6 and 7      

  GPIO_PORTD_AFSEL_R |= 0xC0;           // enable alt funct on PD6-7
  GPIO_PORTD_DEN_R |= 0xC0;             // enable digital I/O on PD6-7
                                        // configure PB1-0 as UART
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x00FFFFFF)+0x11000000;
  GPIO_PORTD_AMSEL_R &= ~0xC0;          // disable analog functionality on PD6 and 7 


}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
uint8_t UART2_InChar(void){
  while((UART2_FR_R&UART_FR_RXFE) != 0); // wait until the receiving FIFO is not empty
  return((uint8_t)(UART2_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART2_OutChar(uint8_t data){
  while((UART2_FR_R&UART_FR_TXFF) != 0);
  UART2_DR_R = data;
}