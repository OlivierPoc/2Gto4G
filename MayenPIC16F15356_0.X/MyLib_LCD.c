/*
 * File:  MyLib_LCD.c
 * Author: Olivier POCHON
 * Company: EMF (Ecole des Métiers de Fribourg) http://www.emf.ch
 * Desc.: Libraire pour le fonctionnement du LCD  avec controler  HD44780 4bits
 * 
 * Ver. Date: V00 201113 Création (YYYYMMDD)
 * Ver. Date: V04 211007 pic->16f1788
 * Ver. Date: V05 211021
 */


//----- Including libraries --
#include <xc.h>
#include "MyLib_LCD.h"  // LCD librairie

//---------------------------------------------------------
// Sous programme LCD_Init
// Auteur: POC
// Desc.: Initialisation de LCD HD44780 en mode 4bits  
// Ver. Date: V00 20201113 Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_Init(void)
{
    // Set PIC IO pin for LCD (Data0..3) as output
    LCD_DAT0_IO=0;
    LCD_DAT1_IO=0;
    LCD_DAT2_IO=0;
    LCD_DAT3_IO=0;
    
    __delay_ms(16);   // HD44780 power On and wait more than 15ms
   
    LCD_RS = CMD_MODE; 
    LCD_RW = WRITE; 
    LCD_WriteQuartet_bit(0x03);
    
    __delay_ms(5);        	// Wait more than 4.1 ms
      
     LCD_WriteQuartet_bit(0x03);
    
   __delay_us(110);      //Wait more than 100 us
  
    LCD_WriteQuartet_bit(0x03);
    
    __delay_us(110);
    
   LCD_WriteQuartet_bit(0x02);
   
    __delay_us(40);      //Wait more than 37 us
    
    // mode 4 bit 
    LCD_WriteCommand(LCD_FUNCTION_SET);
    LCD_WriteCommand(LCD_OFF);
    LCD_WriteCommand(LCD_CLEAR);    
    LCD_WriteCommand(LCD_ENTRY_MODE_SET);
    LCD_WriteCommand(LCD_CURSOR_OFF);
     
}//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_WriteQuartet_bit
// Auteur: POC
// Desc.: Ecrit 4bits bit à bit  
// Ver. Date: V00 211104 Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_WriteQuartet_bit(uint8_t data)
{
    uint8_t tempData;
    
    LCD_RW = WRITE;
    __delay_us(1);
 
    tempData = (data & 0x0F); 
  
    LCD_DAT0_W = (tempData & 0x01);
    tempData = tempData >> 1;
    LCD_DAT1_W = (tempData & 0x01);
    tempData = tempData >> 1;
    LCD_DAT2_W = (tempData & 0x01);
    tempData = tempData >> 1;
    LCD_DAT3_W = (tempData & 0x01);

    __delay_us(1);
        LCD_EN = HIGH;       // Rising Latch controle state (RS)
    __delay_us(1);
        LCD_EN = LOW;        // Falling Latch data
    __delay_us(1);
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_WriteQuartet
// Auteur: POC
// Desc.: Write 4bits  
// Ver. Date: V00 201113 Création (YYMMDD)	
//---------------------------------------------------------
void LCD_WriteQuartet(uint8_t data)
{
    unsigned char tempPORTA;
    unsigned char tempData;
     
    tempData = data & 0x0F; 
    tempPORTA = LCD_DATA_R & 0xF0; // masquage 
  
    LCD_DATA_W = tempPORTA | tempData ;

    __delay_us(1);
   	LCD_EN = HIGH;       // Rising Latch controle state (RS)
    __delay_us(1);
	LCD_EN = LOW;        // Falling Latch data
    __delay_us(1);
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_WriteData
// Auteur: POC
// Desc.: Envoie d'une donnée 8bits au LCD
// Ver. Date: V00 20201113 Création (YYYYMMDD)	
//---------------------------------------------------------
 void LCD_WriteData(uint8_t data)
 {
    uint8_t temp=0;

    temp = data>>4;		// masquage quartet poids fort
    LCD_WriteQuartet_bit(temp);

    temp = data & 0x0F;		// masquage quartet de poids faible
    LCD_WriteQuartet_bit(temp);
 }
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_WriteCommand
// Auteur: POC
// Desc.: Envoie d'une commande 8bits au LCD
// Ver. Date: V00 20201113  Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_WriteCommand (uint8_t byteCmd)// Envoi d'une commande à l'afficheur
{		
    LCD_RS = CMD_MODE;         // RS = 0 -> Command
    LCD_WriteData(byteCmd);
    __delay_us(43);
    
    if((byteCmd==LCD_CLEAR) || (byteCmd ==LCD_HOME))
    {
        __delay_ms(2);        	// Wait more than 1,5 ms
    }

    LCD_RS = DATA_MODE;        // RS = 1 -> Data Mode, by default
}
//---------------------------------------------------------	

//---------------------------------------------------------
// Sous programme LCD_WriteCommand
// Auteur: POC
// Desc.: Envoie d'une commande 8bits au LCD
// Ver. Date: V00 20201113  Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_WriteCommand_BF (uint8_t byteCmd)// Envoi d'une commande à l'afficheur
{		
    LCD_RS = CMD_MODE;         // RS = 0 -> Command
    LCD_WriteData(byteCmd);
    while(LCD_ReadBF_bool());

    LCD_RS = DATA_MODE;        // RS = 1 -> Data Mode, by default
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_Clear
// Auteur: POC
// Desc.: Efface le LCD
// Ver. Date: V00 20201113  Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_Clear(void)
{
    LCD_WriteCommand(LCD_CLEAR); 
}
//---------------------------------------------------------	

//---------------------------------------------------------
// Sous programme LCD_Clear
// Auteur: POC
// Desc.: Efface le LCD
// Ver. Date: V00 20201113  Création (YYYYMMDD)	
//---------------------------------------------------------
void LCD_Clear_BF(void)
{
    LCD_WriteCommand_BF(LCD_CLEAR); 
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_PrintString
// Auteur: 
// Desc.: 
// Ver. Date: V00 20181029 (YYYYMMDD)	
//---------------------------------------------------------
void LCD_PrintString(char *str)
{
    while (*str != '\0' )  // tant que différent de fin de chaine
    {
        LCD_WriteData(*str);
        str++;
    }
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_CursorPosition
// Auteur: 
// Desc.: 
// Ver. Date: V00 20181029 (YYYYMMDD)	
//---------------------------------------------------------
void LCD_CursorPosition(uint8_t ligne, uint8_t colonne)
{
	uint8_t myCommand;
	
    if (ligne == 1) 
    {
        myCommand = 0x00; // Adresse de la première ligne
    }				
    else
    {
        myCommand = 0x40; // Adresse de la 2ème ligne
    }
	
    myCommand += --colonne;	// Adresse du caractère
    myCommand |= 0x80;		// Commande Set Address
    LCD_WriteCommand(myCommand);
	
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_PrintChar
// Auteur: POC
// Desc.: 
// Ver. Date: V00 20180604 20181008 (YYYYMMDD)	
//---------------------------------------------------------
void LCD_PrintChar(uint8_t data)
{
    LCD_WriteData(data);
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_WriteHex
// Auteur: 
// Desc.: 
// Ver. Date: V00 20181029 (YYYYMMDD)	
//---------------------------------------------------------
void LCD_WriteHex(uint8_t data) 
{
    unsigned char temp = data >> 4;

    if (temp < 10)
    {
        LCD_WriteData(temp + 0x30);
    }
    else
    {
            temp -= 9;
            LCD_WriteData(temp + 0x40);
    }

    temp = (data & 0x0F);

    if(temp < 10)
    {
            LCD_WriteData(temp + 0x30);
    }
    else
    {
            temp -= 9;
            LCD_WriteData(temp + 0x40);
    }
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_ReadQuartet
// Auteur: POC
// Desc.: Read 4 bits  
// Ver. Date: V00 212001 Création (YYMMDD)	
//---------------------------------------------------------
uint8_t LCD_ReadQuartet(void)
{
    uint8_t tempData;
    
    LCD_RW = READ;
    __delay_us(1);
	
    LCD_EN = HIGH;       // CLK
    __delay_us(1);
    
    tempData = LCD_DATA_R & 0x0F;
    
    LCD_EN = LOW;       // CLK
   __delay_us(1);
   
    LCD_RW = WRITE;
    
    return tempData;
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_ReadQuartet_bit
// Auteur: POC
// Desc.: lecture bit à bit
// Ver. Date: V00 211104 Création (YYYYMMDD)	
//---------------------------------------------------------
uint8_t LCD_ReadQuartet_bit(void)
{
    uint8_t tempData=0;
    
    // pin LCD (Data0..3) as Input
    LCD_DAT0_IO=1;
    LCD_DAT1_IO=1;
    LCD_DAT2_IO=1;
    LCD_DAT3_IO=1;
                
    LCD_RW = READ;
    __delay_us(1);
    LCD_EN = HIGH;       // CLK
    __delay_us(1);

    
    tempData = LCD_DAT3_R;
    tempData = (uint8_t)(tempData << 1);
    tempData = tempData + LCD_DAT2_R; 
    tempData = (uint8_t)(tempData << 1);
    tempData = tempData + LCD_DAT1_R; 
    tempData = (uint8_t)(tempData << 1);
    tempData = tempData + LCD_DAT0_R; 
  
    LCD_EN = LOW;       // CLK
    __delay_us(1);
   
    LCD_RW = WRITE;
    __delay_us(1);
    
   // pin LCD (Data0..3) as output
    LCD_DAT0_IO=0;
    LCD_DAT1_IO=0;
    LCD_DAT2_IO=0;
    LCD_DAT3_IO=0;
    
    return tempData;	
	
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme LCD_ReadBF_bool
// Auteur: POC
// Desc.: Read 8 bits, BF->MSB  
// Ver. Date: V00 211105 Création (YYMMDD)	
//---------------------------------------------------------
bool LCD_ReadBF_bool(void)
{
    uint8_t tempData, tempTRIS;

    LCD_RS = CMD_MODE;         // RS = 0 -> Command
    __delay_us(1);
       
    tempData = LCD_ReadQuartet_bit();
	
    tempData = (uint8_t)(tempData<<4);
    
    tempData = tempData + LCD_ReadQuartet_bit();
    LCD_RS = CMD_MODE;         // RS = 0 -> Command
  
    __delay_us(1);
       
    tempData = tempData>>7;
    
    if(tempData==1)
    {return true;}
    else
    {return false;}
 
}
//---------------------------------------------------------


//===== END Of File ======================================