/*
 * File:  MyUART1.h
 * Author: Olivier POCHON
 * Company: 
 * Desc.: Libraire pour la communication serie avec le PORT UART1
 * 
 * Ver. Date: 230102 V00 Création (YYYYMMDD)
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef My_UART1_H
#define	My_UART1_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//***************MODEM DEFINITION***************
//--- Define AT  CMD
#define CR  0x0D        // Défini le carriage return en code ASCII (retour à la ligne)
#define LF  0x0A        // Défini le Line Feed en code ASCII (saut de ligne)

#define MAX_SIZE_MESSAGE 25
//char read_modem[MAX_SIZE_MESSAGE_MODEM]; // data modem communication


//bool cmdModemReceive;  // indication de la réception d'une commande

// USART definiton
#define USART1_RX_IO   TRISCbits.TRISC6    // define I/O for Rx
#define USART1_TX_IO   TRISCbits.TRISC7    // define I/O for Tx

//----- Constant Definiton  ----------------------------
#define LIGNE   11
#define COLONNE  20

//----- Global variable  -----------------------------
char modem_str[LIGNE][COLONNE] = {0};
char modem_buffer_index=0;
char modem_read_buffer=0;
uint8_t position = 0;

//----- Function Declarations -----------------------------
void UART1_Init(void);
void UART1_Write(uint8_t data);
void UART1_SendString(char *str);
uint8_t UART1_Read(void);
bool Modem_DataIsReceived(void);

void Modem_Read(void);
void Modem_EmptyData(void);
void Modem_read_cmd(char *str);
void Modem_write_cmd(char *str);

#endif	/* UART1_H */

