/*
 * File:  My_MODEM_UART.c
 * Author: Olivier POCHON
 * Company: 
 * Desc.: Libraire pour la communication serie avec le PORT UART1
 * 
 * 230102 V00 Création
 */

#include <xc.h>
#include "My_MODEM_UART.h"

//---------------------------------------------------------
// Sous programme UART1_Init modem communication
// Auteur: POC
// Desc.: Initialisation du port de communication USART
// Ver. Date: V00 221111 Création (YYYYMMDD)	
//---------------------------------------------------------
void UART1_Init(void)
{
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;

    /*================ TXSTA: TRANSMIT STATUS AND CONTROL REGISTER ===========*/
    //TXEN: Transmit Enable bit(1)
    //  1 = Transmit enabled
    //  0 = Transmit disabled
    TX1STAbits.TXEN = 0;

    //TX9: 9-bit Transmit Enable bit
    //  1 = Selects 9-bit transmission
    //  0 = Selects 8-bit transmission
    TX1STAbits.TX9 = 0;

    //SYNC: EUSART Mode Select bit
    //  1 = Synchronous mode
    //  0 = Asynchronous mode
    TX1STAbits.SYNC = 0;

    //SENDB: Send Break Character bit
    //  Asynchronous mode:
    //      1 = Send Sync Break on next transmission 
    //          (cleared by hardware upon completion)
    //      0 = Sync Break transmission completed
    //  Synchronous mode:
    //      Don?t care
    TX1STAbits.SENDB = 0;

    //BRGH: High Baud Rate Select bit
    //  Asynchronous mode:
    //      1 = High speed
    //      0 = Low speed
    //  Synchronous mode:
    //      Unused in this mode
    TX1STAbits.BRGH = 1;

    //TRMT: Transmit Shift Register Status bit
    //  1 = TSR empty
    //  0 = TSR full
    //TX1STAbits.TRMT = 0;

    //TX9D: Ninth bit of Transmit Data
    //Can be address/data bit or a parity bit.
    TX1STAbits.TX9D = 0;

    /* ============== Port Rx Receive as input ============================== */
    USART1_RX_IO = 1; // 1 input
    USART1_TX_IO = 0;

    /*================ RCSTA: RECEIVE STATUS AND CONTROL REGISTER ============*/
    //SPEN: Serial Port Enable bit
    //  1 = Serial port enabled (configures RX/DT and TX/CK as serial port pins)
    //  0 = Serial port disabled (held in Reset)
    RC1STAbits.SPEN = 1;

    //RX9: 9-bit Receive Enable bit
    //  1 = Selects 9-bit reception
    //  0 = Selects 8-bit reception
    RC1STAbits.RX9 = 0;

    //CREN: Continuous Receive Enable bit
    //      1 = Enables receiver
    //      0 = Disables receiver
    RC1STAbits.CREN = 1;

    /*================== BAUDCTL: BAUD RATE CONTROL REGISTER =================*/
    SP1BRG = 16; //115.2k baud rate 

    //BRG16: 16-bit Baud Rate Generator bit
    //      1 = 16-bit Baud Rate Generator is used
    //      0 = 8-bit Baud Rate Generator is used
    BAUD1CONbits.BRG16 = 1;

    // Clock Transmit Polarity Selesction Bit
    // 1 = Idle state is low (TX)
    // 0 = Idle state is high (TX)
    BAUD1CONbits.SCKP = 0;

    /*================ TXSTA: TRANSMIT STATUS AND CONTROL REGISTER ===========*/
    //TXEN: Transmit Enable bit(1)
    //  1 = Transmit enabled
    //  0 = Transmit disabled
    TX1STAbits.TXEN = 1;

    // Enable interupt
    PIE3bits.RC1IE = 1;

    Modem_EmptyData();
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme UART_Write
// Auteur: POC
// Desc.: Envoie d'un caractère sur le port UART
// Ver. Date: V00 20201104 Création (YYYYMMDD)	
//---------------------------------------------------------	
void UART1_Write(uint8_t data)
{
    while (PIR3bits.TX1IF == 0); // Transmit Buffer is empty ('1')
    TX1REG = data;
    while (TX1STAbits.TRMT == 0); // Wait until character was send(TSR empty)('1')
}
//------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_Read
// Auteur: POC
// Desc.: Lecture d'un caractère sur le port UART
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------------------
uint8_t UART1_Read()
{
    //while(!PIR1bits.RCIF);        // Receive Buffer is full ('1')
    return RC1REG;
}
//---------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_DataIsReceived
// Auteur: POC
// Desc.: data is received return true or false
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
bool Modem_DataIsReceived(void)
{
    if (modem_buffer_index != modem_read_buffer)
    {
        return true;

    }
    else
    {
        //modem_buffer_index = 0;
        //modem_read_buffer = 0;
        return false;
    }
}
//---------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: Send String
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void UART1_SendString(char *str)
{
    while (*str != '\0')
    {
        UART1_Write(*str);
        str++;
    }
    UART1_Write(CR);
    UART1_Write(LF);
}
//---------------------------------------------------------------------	

//---------------------------------------------------------
// Sous programme Modem_Read
// Auteur: POC
// Desc.: mise en forme des données recut du modem
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------
void Modem_Read(void)
{

    char data;

    data = UART1_Read();

    if (data != '\0') //&& (data != LF) && (data != CR)
    {
        if (data == CR) // si fin de transmission 
        {
            if (position < 1)
            {
                modem_buffer_index++;
            }
            else
            {
                modem_str[modem_buffer_index][position] = '\0';
            }
            position = 0;
        }
        else if (data != LF) // si fin de transmission 
        {
            modem_str[modem_buffer_index][position] = data;
            position++;
           
        }
    }
    else
    {
        position = 0;
    }

    if (modem_buffer_index == 9)
    {
        modem_buffer_index = 0;
    }
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme EmptyReceiver
// Auteur: POC
// Desc.: vide la buffer de reception
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------
void Modem_EmptyData(void)
{
    PIE3bits.RC1IE = 0;

    for(uint8_t ligne=LIGNE;ligne!=0;ligne--)
    {
        for(uint8_t colonne=COLONNE;colonne!=0;colonne--)
        {
            modem_str[ligne][colonne] = '\0';
        }
    }
    modem_buffer_index = 0;
    modem_read_buffer = 0;
    position = 0;

    PIE3bits.RC1IE = 1;
}
//---------------------------------------------------------

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: read modem message
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void Modem_read_cmd(char *str)
{
    uint8_t readPosition = 0;

    while ((modem_str[modem_read_buffer][readPosition] != '\0') && (modem_str[modem_read_buffer][readPosition] != CR))
    {
        //*str = read_modem[position];
        *str = modem_str[modem_read_buffer][readPosition];
        str++;
        *str ='\0';
        //modem_str[modem_read_buffer][readPosition]='\0';
        readPosition++;
    }
    modem_read_buffer++;
    
    if (modem_read_buffer == 9)
    {
        modem_read_buffer = 0;
    }
    // cmdModemReceive = false;
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: read modem message
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void Modem_write_cmd_ToModem(char *str)
{
    UART1_SendString(str);
}
//-------------------------------------------------------------------

//================== END Of File ======================================
