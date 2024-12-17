/*
 * File:   mayen_01
 * Author: Olivier POCHON
 * Company: EMF (Ecole des Métiers de Fribourg) http://www.emf.ch
 * Desc.:  Communication serial entre le PIC16f1788 et un hyperterminal
 * 
 * Ver. Date:   V01 210128 Création (YYMMDD) not working
 *              V02 211111 correction TRISC RC7 input pour RxD
 *              V03 211111 char -> uint8_t
 *              V04 241025 mise a jour pour hardware "modem2g4g"
 */

//----- Including libraries --
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mayen_01.h"

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT1    // Power-up default value for COSC bits->HFINTOSC (1MHz)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; i/o or oscillator function on OSC2
#pragma config CSWEN = ON    // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable bit->FSCM timer enabled

// CONFIG2
#pragma config MCLRE = ON    // Master Clear Enable bit->MCLR pin is Master Clear function
#pragma config PWRTE = OFF    // Power-up Timer Enable bit->PWRT disabled
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON    // Brown-out reset enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices
#pragma config ZCD = OFF    // Zero-cross detect disable->Zero-cross detect circuit is disabled at POR.
#pragma config PPS1WAY = ON    // Peripheral Pin Select one-way control->The PPSLOCK bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset

// CONFIG3
#pragma config WDTCPS = WDTCPS_31    // WDT Period Select bits->Divider ratio 1:65536; software control of WDTPS
#pragma config WDTE = OFF    // WDT operating mode->WDT Disabled, SWDTEN is ignored
#pragma config WDTCWS = WDTCWS_7    // WDT Window Select bits->window always open (100%); software control; keyed access not required
#pragma config WDTCCS = SC    // WDT input clock selector->Software Control

// CONFIG4
#pragma config BBSIZE = BB512    // ->512 words boot block size
#pragma config BBEN = OFF    // ->Boot Block disabled
#pragma config SAFEN = OFF    // ->SAF disabled
#pragma config WRTAPP = OFF    // ->Application Block not write protected
#pragma config WRTB = OFF    // ->Boot Block not write protected
#pragma config WRTC = OFF    // ->Configuration Register not write protected
#pragma config WRTSAF = OFF    // ->SAF not write protected
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.

// CONFIG5
#pragma config CP = OFF    // UserNVM Program memory code protection bit->UserNVM code protection disabled

#define CONF_OSCCON 0b01101010  //4MHz clock speed
#define _XTAL_FREQ 8000000      // 8MHz

// INTERUP
#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)

//----- Constant Definiton  ----------------------------
#define IO_TRISA    0b00000000  // RA0..7 LCD (output)

#define IO_TRISB    0b11001000  // RB3 RX,B2 Tx, LCD, RB4..5 LED,RB6,7 ISCP (input) 

#define IO_TRISC    0b01000111  // RC7 TxD->Input (TxD modem)
// RC6 RxD->Output (TxD modem)
// RC0..2 boutton(Input) 
// RC4-> Output PowerKey modem

#define REP_PORTA   0b11111111 // LCD
#define REP_PORTB   0b00101111 // B0,B1 RS.EN LCD
// B3 Rxd B4 TxD
#define REP_PORTC   0b00000000 // C0..2 boutton, (other not use ->0) 

// LED
#define P2_ROUGE    LATBbits.LATB4   // LED P2 rouge
#define P2_VERTE    LATBbits.LATB5   // LED P2 verte
#define LED_ON      0
#define LED_OFF     1

// Switch
#define S0          PORTCbits.RC0   // Switch S0
#define S1          PORTCbits.RC1   // Switch S1 
#define S2          PORTCbits.RC2   // Switch S2
#define PRESS   0
#define RELEASE 1



static bool message = false; // use to debug


//    {"AT", "AT", "OK"},
//    {"AT+GMI", "Cinterion", "OK"},
//    {"AT+GMM", "BGS2-W", "OK"},
//    {"AT+CMGF=1", "OK", 0},
//    {"ATE0", "ATE0", "OK"},
//    {"ATF", "ATF", "OK"},
//    {"AT+CNMI=0,0,0,0,1", "OK", 0},
//    {"AT+CMGD=1", "OK", 0},
//    {"AT+CMGD=2", "OK", 0},
//    {"AT+CMGD=3", "OK", 0},
//};

//----- Function Declarations -----------------------------


//===== Main Function Implementation ======================
void main(void)
{
    char str_modem[20] = {0};
    char *ptr;
    char str_POELE[20] = {0};

    PIC_Init(); //Initialise le PIC
    LCD_Init();
    UART1_Init(); // Modem
    UART2_Init(); // Poele
    P2_ROUGE = LED_ON; // LED on

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    Modem_BOOT();
    P2_ROUGE = LED_OFF; // LED on
    P2_VERTE = LED_ON; // LED on
    LCD_PrintString("Modem:ON"); //indique que le pic est en fonctionement

    do
    {
        if (S0 == PRESS)
        {
            LCD_Clear();
            Modem_EmptyData();
            POELE_EmptyData();
            Modem_BOOT();
            while (S0 == PRESS);
            LCD_CursorPosition(1, 1);
            LCD_PrintString("M:RESET"); //indique que le pic est en fonctionement
            LCD_CursorPosition(2, 1);
            LCD_PrintString("P:RESET"); //indique que le pic est en fonctionement
        }

        if (S1 == PRESS)
        {
            LCD_Clear();
            ptr = "AT+CGMM"; //AT+GMI Request Manufacturer Identification
            LCD_CursorPosition(1, 1);
            LCD_PrintString("P:");
            LCD_PrintString(ptr); //indique que le pic est en fonctionement
            while (S1 == PRESS);
            Modem_write_cmd(ptr);
        }

        if (S2 == PRESS)
        {
            LCD_Clear();
            ptr = "AT+GMI"; //AT+GMI Request Manufacturer Identification
            LCD_CursorPosition(1, 1);
            LCD_PrintString("P:");
            LCD_PrintString(ptr); //indique que le pic est en fonctionement
            //POELE_cmd(str_POELE);
            while (S2 == PRESS);
            Modem_write_cmd(ptr);
        }

        if (Modem_DataIsReceived()) // Modem Receive Buffer is full ('1')
        {
            Modem_read_cmd(str_modem); // read data form modem
            LCD_CursorPosition(2, 1);
            LCD_PrintString("M:");
            LCD_PrintString(str_modem); //indique que le pic est en fonctionement
            P2_VERTE = LED_ON;
            if (strcmp("sms", str_modem) == 0)
            {

            }
            else
            {
                POELE_SendStringCRLF(str_modem);
                POELE_SendOK();
            }
            P2_VERTE = LED_OFF;
        }

        if (POELE_DataReady) // Poele Receive Buffer is full ('1')
        {
            P2_ROUGE = LED_ON;
            POELE_cmd(str_POELE); // read data form poele

            if (strcmp("AT+GMI", str_POELE) == 0)
            {
                POELE_SendStringCRLF("Cinterion");
                POELE_SendOK();
            }
            else if (strcmp("AT+GMM", str_POELE) == 0)
            {
                POELE_SendStringCRLF("BGS2-W");
                POELE_SendOK();
            }
                //            else if (strcmp("AT", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT&F", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("ATE0", str_POELE) == 0)
                //            {
                //                POELE_SendString("ATE0");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+GMI", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("Cinterion");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+GMM", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("BGS2-W");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CNMI=0,0,0,0,1", str_POELE) == 0)
                //            {
                //                POELE_SendString("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGD=1", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGD=2", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGD=3", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGV=1", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGF=1", str_POELE) == 0)
                //            {
                //                POELE_SendStringCRLF("");
                //                POELE_SendOK();
                //            }
                //            else if (strcmp("AT+CMGR=1", str_POELE) == 0)
                //            {
                //                if (message)
                //                {
                //                    POELE_SendString("\r\n+CMGR:0,\"REC UNREAD\",\"+41793018256\",,\"23/01/01,22:10:00+08\"\r\n");
                //                    //UART2_Write(LF);
                //                    POELE_SendString("4139 ?");
                //                    //UART2_Write('\x1A');
                //                    POELE_SendStringCRLF("");
                //                    POELE_SendOK();
                //                }
                //                else
                //                {
                //                    POELE_SendStringCRLF("");
                //                    POELE_SendStringCRLF("+CMGR: 0,,0");
                //                    POELE_SendOK();
                //                }
                //            }
                //            else if (strcmp("AT+CMGS=\"+41793018256\"", str_POELE) == 0)
                //            {
                //                while (!POELE_DataReady); //Attend le message
                //                POELE_cmd(str_POELE);
                //                POELE_SendStringCRLF("*CF+CMGS: 73");
                //                POELE_SendOK();
                //            }
            else
            {
                Modem_write_cmd(str_POELE);
            }

            P2_ROUGE = LED_OFF;
        }
    }
    while (1);

    while (1);
}
//---------------------------------------------------------

//---------------------------------------------------------
// Sous programme INTERRUPT_InterruptManager
// Auteur: POC
// Desc.: Gestion des interruption pour la réception  
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------
void __interrupt() INTERRUPT_InterruptManager(void)
{
    // interrupt handler
    if (INTCONbits.PEIE == 1)
    {
        if (PIE3bits.RC2IE == 1 && PIR3bits.RC2IF == 1)
        {
            POELE_Read();
        }
        //        else if (PIE3bits.TX2IE == 1 && PIR3bits.TX2IF == 1)
        //        {
        //            PIR3bits.TX1IF = 0;
        //        }
        //        else 
        if (PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            Modem_Read(); // Reiceiver interrupt
        }
        //        else if (PIE3bits.RC2IE == 1 && PIR3bits.RC2IF == 1)
        //        {
        //            POELE_Read();
        //        }
        //        else if (PIE3bits.TX1IE == 1 && PIR3bits.TX1IF == 1)
        //        {
        //            PIR3bits.TX1IF = 0;
        //        }
        //        {
        //        }
    }
}

//---------------------------------------------------------
// Sous programme InitPic
// Auteur: POC
// Desc.: Initialisation du PIC
// Ver. Date: V00 20170604 Création (YYYYMMDD)	
//---------------------------------------------------------
void PIC_Init(void)
{
    /// Config Oscillator    
    //OSCCON = CONF_OSCCON; //clock speed

    // NOSC HFINTOSC; NDIV 1; 
    OSCCON1 = 0x60;
    OSCCON1bits.NOSC = 0b110; //New Oscillator Source Request HFINTOSC (1-32MHz)

    // CSWHOLD may proceed; SOSCPWR Low power; 
    OSCCON3 = 0x00;
    // MFOEN disabled; LFOEN disabled; ADOEN disabled; SOSCEN disabled; EXTOEN disabled; HFOEN disabled; 
    OSCEN = 0x00;
    // HFFRQ 8_MHz; 
    OSCFRQ = 0x03;

    //Configuration AD PIC
    ANSELA = 0; // Configuration des IO en mode Digital 
    ANSELB = 0; // Configuration des IO en mode Digital 
    ANSELC = 0;

    //  Config repos Port  
    LATA = REP_PORTA;
    LATB = REP_PORTB;
    LATC = REP_PORTC;

    // Config IO Port
    TRISA = IO_TRISA;
    TRISB = IO_TRISB;
    TRISC = IO_TRISC;

    RB2PPS = 0x11; //RB2->EUSART2:TX2;    
    RC7PPS = 0x0F; //RC7->EUSART1:TX1 to Rx Modem  
    RX2DTPPS = 0x0B; //RB3->EUSART2:RX2;  
    RX1DTPPS = 0x16; //RC6->EUSART1:RX1 to TX Modem 
}
//---------------------------------------------------------

//-------------------------------------------------------------------
// Sous programme UART_SendString
// Auteur: POC
// Desc.: read modem message
// Ver. Date: V00 221110 Création (YYYYMMDD)	
//---------------------------------------------------------------------
void Modem_BOOT(void)
{
    POWERKEY_ON();
    __delay_ms(500);
    __delay_ms(500);
    POWERKEY_OFF();
    for (char time = 15; time != 0; time--) // wait 10 second
    {
        __delay_ms(500);
    }

}
//-------------------------------------------------------------------

//================== END Of File ======================================