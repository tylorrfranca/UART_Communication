/*
Project1Part1
LEDSW.h
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 2 UART Communications
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/10/2025
File Description : This File is the function header file for LEDSW.c
*/

#include <stdbool.h>


void PORTF_INIT(void);
void PWM_INIT(void);
void Set_LED_Brightness(float brightness_red, float brightness_blue, float brightness_green);
void PWM_DISABLE(void);
void PWM_ENABLE(void);
bool PWM_STATUS(void);