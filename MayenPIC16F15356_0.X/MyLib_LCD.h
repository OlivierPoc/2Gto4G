/*
 * File:  MyLib_LCD.h
 * Author: Olivier POCHON
 * Company: EMF (Ecole des Métiers de Fribourg) http://www.emf.ch
 * Desc.: Libraire pour le fonctionnement du LCD  avec controler  HD44780 4bits
 * 
 * Ver. Date: V00 201113 Création (YYMMDD)
 *              V06 use LAT, uint8_t & check BF
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MY_LIB_LCD
#define	MY_LIB_LCD
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 12000000      // 4MHz

// ------ LCD PORT DEFINITOn --------------
#define LCD_DATA_W    LATA      // DATA0..3 Must be on port 0 1 2 3 !
#define LCD_DATA_R    PORTA     // DATA0..3 Must be on port 0 1 2 3 !
// LCD Write Data
#define LCD_DAT0_W    LATAbits.LATA0   	// Write Data bit 0						
#define LCD_DAT1_W    LATAbits.LATA1      // WriteData bit 1						
#define LCD_DAT2_W    LATAbits.LATA2      // WriteData bit 2					
#define LCD_DAT3_W    LATAbits.LATA3      // WriteData bit 3
// LCD Read Data
#define LCD_DAT0_R    PORTAbits.RA0   	// Read Data bit 0						
#define LCD_DAT1_R    PORTAbits.RA1      // Read Data bit 1						
#define LCD_DAT2_R    PORTAbits.RA2      // Read Data bit 2						
#define LCD_DAT3_R    PORTAbits.RA3      // Read Data bit 3

// LCD IO Definition
#define LCD_DAT3_IO    	TRISAbits.TRISA3
#define LCD_DAT2_IO    	TRISAbits.TRISA2
#define LCD_DAT1_IO   	TRISAbits.TRISA1 
#define LCD_DAT0_IO    	TRISAbits.TRISA0 
				
// LCD PIN Command Definition
#define LCD_EN      LATAbits.LATA4    	// LCD enable line
#define LCD_RS      LATBbits.LATB0  	// LCD register select line	
#define LCD_RW      LATBbits.LATB1		// LCD read/write line

// ------ LCD controller setup command
#define LCD_FUNCTION_SET    0b00101000  // 4 bit interface, 2 lines, 5x8 font
#define LCD_ENTRY_MODE_SET  0b00000110  // 
#define LCD_ON              0b00001111  // Display On, Curseur clignotant
#define LCD_OFF             0b00001000  // Diplay Off

// ------ Commande set for 44780/ST7760
#define LCD_CLEAR           0b00000001
#define LCD_HOME            0x02
#define LCD_CURSOR_BACK     0x10
#define LCD_CURSOR_FWD      0x14
#define LCD_PAN_LEFT        0x18
#define LCD_PAN_RIGHT       0x1C
#define LCD_CURSOR_OFF      0x0C
#define LCD_CURSOR_ON       0x0E
#define LCD_CURSOR_BLINK    0x0F
#define LCD_CURSOR_LINE2    0xC0

// ------ Commande set for 44780/ST7760
#define CMD_MODE    0
#define DATA_MODE   1
#define WRITE   0
#define READ    1
#define HIGH    1
#define LOW     0

//----- Function Prototype -----------------------------
void LCD_Init(void);
void LCD_WriteQuartet(uint8_t data);
void LCD_WriteQuartet_bit(uint8_t data);
void LCD_WriteData(uint8_t data);
void LCD_WriteCommand (uint8_t byteCmd);
void LCD_WriteCommand_BF (uint8_t byteCmd);
void LCD_Clear(void);
void LCD_Clear_BF(void);
void LCD_PrintString(char *str);
void LCD_CursorPosition(uint8_t ligne, uint8_t colonne);
void LCD_PrintChar(uint8_t data);
void LCD_WriteHex(uint8_t data);
uint8_t LCD_ReadQuartet(void);
uint8_t LCD_ReadQuartet_bit(void);
bool LCD_ReadBF_bool(void);


#endif	/* MY_LIB_LCD */
//===== END Of File ======================================
