/*
 * File:  MyUART2.h
 * Author: Olivier POCHON
 * Company: 
 * Desc.: Libraire pour la communication serie avec le PORT UART1
 * 
 * Ver. Date: 230102 V00 Création (YYYYMMDD)
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef My_UART2_H
#define	My_UART2_H

//----- Including libraries --
#include <stdint.h>
#include <stdbool.h>

//***************MODEM DEFINITION***************
//--- Define AT  CMD
#define CR  0x0D     // '\r'  // Défini le carriage return en code ASCII (retour à la ligne)
#define LF  0x0A     // '\n'  // Défini le Line Feed en code ASCII (saut de ligne)

#define MAX_SIZE_MESSAGE_RS232 25
static char read_RS232[MAX_SIZE_MESSAGE_RS232]; // data modem communication

bool cmdRS323Receive;  // indication de la réception d'une commande

#define POELE_DataReady  (POELE_DataIsReceived())

// USART definiton
#define USART2_RX_IO   TRISBbits.TRISB3    // define I/O for Rx
#define USART2_TX_IO   TRISBbits.TRISB2    // define I/O for Tx

//----- Function Declarations -----------------------------
void UART2_Init(void);
void UART2_Write(uint8_t data);
void POELE_SendStringCRLF(char *str);
void POELE_SendString(char *str);
uint8_t UART2_Read(void);
bool POELE_DataIsReceived(void);
void POELE_Read(void);
void POELE_EmptyData(void);
void POELE_cmd(char *str);
void POELE_SendOK(void);

#endif	/* UART1_H */

