// UART.c
// Runs on TM4C123
// Simple device driver for the UART. This is an example code for UART board to board communication.
// board to board communitation uses UART1
// By Dr.Min He
#include "UART3_INIT.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>  // for C boolean data type

#define NVIC_EN1_UART3 0x08000000

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART3_INIT(bool RxInt, bool TxInt){
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R3; // activate UART1
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // activate port B
	while((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R2) == 0){}; 
		
  UART3_CTL_R = 0;                     // reset UART1
  UART3_IBRD_R = 81;                    // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.68)
  UART3_FBRD_R = 24;                     // FBRD = int(0.68 * 64 + 0.5) = 44
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART3_LCRH_R = UART_LCRH_WLEN_8;
	if (RxInt) {
	  UART3_IM_R |= UART_IM_RXIM;         // Enable RX interrupt
	}
	
	if (TxInt) {
	  UART3_IM_R |= UART_IM_TXIM;         // Enable TX interrupt
	}
  UART3_CTL_R |= UART_CTL_UARTEN|UART_CTL_RXE|UART_CTL_TXE; // enable UART, Rx, Tx
	
	if ( RxInt | TxInt) {
		NVIC_PRI14_R = (NVIC_PRI14_R&0x1FFFFFFF)|0x80000000; // bits 23-21, priority 4
		NVIC_EN1_R = NVIC_EN1_UART3;           // enable interrupt 5 in NVIC
	}
		
  GPIO_PORTC_AFSEL_R |= 0xC0;           // enable alt funct on PB1-0
  GPIO_PORTC_DEN_R |= 0xC0;             // enable digital I/O on PB1-0
                                        // configure PB1-0 as UART
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0x00FFFFFF)+0x11000000;
  GPIO_PORTC_AMSEL_R &= ~0xC0;          // disable analog functionality on PB1-0
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
uint8_t UART3_InChar(void){
  while((UART3_FR_R&UART_FR_RXFE) != 0); // wait until the receiving FIFO is not empty
  return((uint8_t)(UART3_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART3_OutChar(uint8_t data){
  while((UART3_FR_R&UART_FR_TXFF) != 0);
  UART3_DR_R = data;
}
