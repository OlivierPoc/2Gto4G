/*
 * File:  My_UART1.c
 * Author: Olivier POCHON
 * Company: 
 * Desc.: Libraire pour la communication serie avec le PORT UART1
 * UART 1 -> modem
 * UART 2 -> poele
 * 230102 V00 Création
 */

#include <xc.h>
#include "My_POELE_UART.h"

//---------------------------------------------------------
// Sous programme UART1_Init modem communication
// Auteur: POC
// Desc.: Initialisation du port de communication USART
// Ver. Date: V00 221111 Création (YYYYMMDD)	
//---------------------------------------------------------
void UART2_Init(void)
{
    // Disable interupt         
    PIE3bits.RC2IE = 0;
    /*================ TXSTA: TRANSMIT STATUS AND CONTROL REGISTER ===========*/
    //TX9: 9-bit Transmit Enable bit
    //  1 = Selects 9-bit transmission
    //  0 = Selects 8-bit transmission
    TX2STAbits.TX9 = 0;

    //TXEN: Transmit Enable bit(1)
    //  1 = Transmit enabled
    //  0 = Transmit disabled
    TX2STAbits.TXEN = 1;

    //SYNC: EUSART Mode Select bit
    //  1 = Synchronous mode
    //  0 = Asynchronous mode
    TX2STAbits.SYNC = 0;

    //SENDB: Send Break Character bit
    //  Asynchronous mode:
    //      1 = Send Sync Break on next transmission 
    //          (cleared by hardware upon completion)
    //      0 = Sync Break transmission completed
    //  Synchronous mode:
    //      Don?t care
    TX2STAbits.SENDB = 0;

    //BRGH: High Baud Rate Select bit
    //  Asynchronous mode:
    //      1 = High speed
    //      0 = Low speed
    //  Synchronous mode:
    //      Unused in this mode
    TX2STAbits.BRGH = 1;

    //TRMT: Transmit Shift Register Status bit
    //  1 = TSR empty
    //  0 = TSR full
    //TX1STAbits.TRMT = 0;

    //TX9D: Ninth bit of Transmit Data
    //Can be address/data bit or a parity bit.
    TX2STAbits.TX9D = 0;

    /* ============== Port Rx Receive as input Tx ouput====================== */
    USART2_RX_IO = 1; // 1 input
    USART2_TX_IO = 0;

    /*================ RCSTA: RECEIVE STATUS AND CONTROL REGISTER ============*/
    //SPEN: Serial Port Enable bit
    //  1 = Serial port enabled (configures RX/DT and TX/CK as serial port pins)
    //  0 = Serial port disabled (held in Reset)
    RC2STAbits.SPEN = 1;

    //RX9: 9-bit Receive Enable bit
    //  1 = Selects 9-bit reception
    //  0 = Selects 8-bit reception
    RC2STAbits.RX9 = 0;

    //CREN: Continuous Receive Enable bit
    //      1 = Enables receiver
    //      0 = Disables receiver
    RC2STAbits.CREN = 1;

    /*================== BAUDCTL: BAUD RATE CONTROL REGISTER =================*/
    SP2BRG = 16; //115.2k baud rate, 34->56k, 103->19.2k

    //BRG16: 16-bit Baud Rate Generator bit
    //      1 = 16-bit Baud Rate Generator is used
    //      0 = 8-bit Baud Rate Generator is used
    BAUD2CONbits.BRG16 = 1;

    // Clock Transmit Polarity Selesction Bit
    // 1 = Idle state is low (TX)
    // 0 = Idle state is high (TX)
    BAUD2CONbits.SCKP = 0;

    RS232_EmptyData();

    // Disable interupt         
    PIE3bits.RC2IE = 1;
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme UART_Write
// Auteur: POC
// Desc.: Envoie d'un caractère sur le port UART
// Ver. Date: V00 20201104 Création (YYYYMMDD)	
//---------------------------------------------------------	
void UART2_Write(uint8_t data)
{
    while (PIR3bits.TX2IF == 0); // Transmit Buffer is empty ('1')
    TX2REG = data;
    while (TX2STAbits.TRMT == 0); // Wait until character was send(TSR empty)('1')
}
//------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_Read
// Auteur: POC
// Desc.: Lecture d'un caractère sur le port UART
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------------------
uint8_t UART2_Read()
{
    //while(!PIR1bits.RCIF);        // Receive Buffer is full ('1')
    return RC2REG;
}
//---------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_DataIsReceived
// Auteur: POC
// Desc.: data is received return true or false
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
bool POELE_DataIsReceived(void)
{

    return cmdRS323Receive;

}
//---------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: Send String
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void POELE_SendStringCRLF(char *str)
{
    while (*str != '\0')
    {
        UART2_Write(*str);
        str++;
    }
    UART2_Write(CR);
    UART2_Write(LF);
}
//---------------------------------------------------------------------	
//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: Send String
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void POELE_SendString(char *str)
{
    while (*str != '\0')
    {
        UART2_Write(*str);
        str++;
    }
}
//---------------------------------------------------------------------	

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: Send String
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void POELE_SendOK(void)
{
    POELE_SendString("\r\nOK\r\n");
    //    UART2_Write(CR);
    //    UART2_Write(LF);
    //    UART2_Write('O');
    //    UART2_Write('K');
    //    UART2_Write(CR);
    //    UART2_Write(LF);
}
//---------------------------------------------------------------------	
//---------------------------------------------------------
// Sous programme Modem_Read
// Auteur: POC
// Desc.: mise en forme des données recut dedpuis RS232
// Les commandes (chaine de caractères) sont terminées par CR 
// Ver. Date: V00 230103 Création (YYYYMMDD)	
//---------------------------------------------------------
void POELE_Read(void)
{
    static uint8_t position = 0;
    char data;

    data = UART2_Read();

    if (data != '\0')
    {
        if (data == CR) // CR  end the transmission
        {
            read_RS232[position] = '\0';
            cmdRS323Receive = true;
            position = 0;
        }
        else if (data != LF)
        {
            read_RS232[position] = data;
            position++;
            cmdRS323Receive = false;
        }
    }
    else
    {
        cmdRS323Receive = false;
        position = 0;
    }
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme EmptyReceiver
// Auteur: POC
// Desc.: vide la buffer de reception
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------
void RS232_EmptyData(void)
{
    PIE3bits.RC2IE = 0;

    read_RS232[0] = '\0';
    cmdRS323Receive = false;

    PIE3bits.RC2IE = 1;
}
//---------------------------------------------------------

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: Send String
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void POELE_cmd(char *str)
{
    uint8_t position = 0;

    while (read_RS232[position] != '\0')
    {
        *str = read_RS232[position];
        str++;
        position++;
    }
    cmdRS323Receive = false;

}
//================== END Of File ======================================
