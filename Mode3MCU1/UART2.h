/*
Project1Part2
UART2.h
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/13/2025
File Description : This File is the function header file for UART2.c
*/

#include <stdint.h>
#include <stdbool.h>

#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F
#define NULL 0


// Initialize UART2 for inter?MCU communication at 38,400 bps.
// RxInt and TxInt enable receive and transmit interrupts if needed.
void UART2_Init(bool RxInt, bool TxInt);

// Wait for and return a character received via UART2.
uint8_t UART2_InChar(void);

// Transmit a character via UART2.
void UART2_OutChar(uint8_t data);

// Returns true if data is available to be read from UART2.
bool UART2_DataAvailable(void);

void UART2_InString(uint8_t *bufPt, uint16_t max);

void UART2_OutString(uint8_t *pt);