/*
 * File:   TestSerial_3.c
 * Author: Olivier POCHON
 * Company: EMF (Ecole des Métiers de Fribourg) http://www.emf.ch
 * Desc.:  Communication serial entre le PIC16f1788 et un hyperterminal
 * 
 * Ver. Date:   V01 210128 Création (YYMMDD) not working
 *              V02 211111 correction TRISC RC7 input pour RxD
 *              V03 211111 char -> uint8_t
 */

//----- Including libraries --
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "My_UART1.h"
#include "My_UART2_1.h"
//#include "MyLib_LCD.h"

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
#define IO_TRISC    0b10000111  // RC7 RxD->Input, RC0..2 boutton(Input) RC3..6 not use ->0

#define REP_PORTA   0b11111111 // 7 seg off (led active a 0)
#define REP_PORTB   0b00111111 // B0,B1 selection P1,P2 actif bas
// B2..5 -> 1 repos led (other not use ->0)
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


char read_modem[MAX_MESSAGE_MODEM][MAX_SIZE_MESSAGE_MODEM] = {0};
uint8_t positionModem = 0;
uint8_t nbLigneModem = 0;
char read_RS232[MAX_MESSAGE_RS232][MAX_SIZE_MESSAGE_RS232] = {0}; // data modem communication
uint8_t positionRS232 = 0;
uint8_t nbLigneRS232 = 0;
static bool message = false;

#define CHECK_LINE 10
char QUESTION[CHECK_LINE][20] = {
    "AT",
    "AT+GMI",
    "AT+GMM",
    "AT+CMGF=1",
    "ATE0",
    "AT&F",
    "AT+CNMI=0,0,0,0,1"
};

char ANSWER[CHECK_LINE][20] = {
    "",
    "Cinterion",
    "BGS2-W",
    "",
    "ATE0",
    "",
    "ERROR"
};


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
void PIC_Init(void);


//===== Main Function Implementation ======================
void main(void)
{
    char str_modem[20] = {0};
    char str_RS232[20] = {0};

    PIC_Init(); //Initialise le PIC
    UART1_Init();
    UART2_Init();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    P2_VERTE = LED_ON; // LED on


    do
    {
        if (S0 == PRESS)
        {
            message = true;
            while (S0 == PRESS);
            message = true;
        }

        if (S1 == PRESS)
        {
            message = false;
            Modem_EmptyData();

            while (S1 == PRESS);
        }

        if (S2 == PRESS)
        {
            Modem_cmd(str_modem);
            RS232_cmd(str_RS232);
            while (S2 == PRESS);
        }

        if (nbMessageModem() != 0) // Receive Buffer is full ('1')
        {
            Modem_cmd(str_modem);
            P2_VERTE = LED_ON;

        }
        if (nbMessageRS232() != 0) // Receive Buffer is full ('1')
        {
            P2_ROUGE = LED_ON;
            RS232_cmd(str_RS232);

            if (strcmp("AT", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT&F", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("ATE0", str_RS232) == 0)
            {
                UART2_SendString("ATE0");
                UART2_SendOK();
            }
            else if (strcmp("AT+GMI", str_RS232) == 0)
            {
                UART2_SendStringCRLF("Cinterion");
                UART2_SendOK();
            }
            else if (strcmp("AT+GMM", str_RS232) == 0)
            {
                UART2_SendStringCRLF("BGS2-W");
                UART2_SendOK();
            }
            else if (strcmp("AT+CNMI=0,0,0,0,1", str_RS232) == 0)
            {
                UART2_SendString("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGD=1", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGD=2", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGD=3", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGV=1", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGF=1", str_RS232) == 0)
            {
                UART2_SendStringCRLF("");
                UART2_SendOK();
            }
            else if (strcmp("AT+CMGR=1", str_RS232) == 0)
            {
                if (message)
                {
                    UART2_SendString("+CMGR:0,\"+41793018256\", \"23/01/01,22:10:00+08\"");
                    UART2_Write(LF);
                    UART2_SendString("4139 ?");
                    UART2_Write('\x1A');
                    UART2_SendStringCRLF("");
                }
                else
                {
                    UART2_SendStringCRLF("");
                    UART2_SendStringCRLF("+CMGR: 0,,0");
                    UART2_SendOK();
                }
            }
            else if (strcmp("AT+CMGS=\"+41793018256\"", str_RS232) == 0)
            {
                while (!UART2_DataIsReceived()); //Attend le message
                RS232_cmd(str_RS232);
                UART2_SendStringCRLF("*CF+CMGS: 73");
                UART2_SendOK();
            }
            else
            {
                //RS232_EmptyData();
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
            RS232_Read();
        }
        else if (PIE3bits.TX2IE == 1 && PIR3bits.TX2IF == 1)
        {
            PIR3bits.TX1IF = 0;
        }
        else if (PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            //Modem_Read(); // Reiceiver interrupt
        }
        else if (PIE3bits.RC2IE == 1 && PIR3bits.RC2IF == 1)
        {
            RS232_Read();
        }
        else if (PIE3bits.TX1IE == 1 && PIR3bits.TX1IF == 1)
        {
            PIR3bits.TX1IF = 0;
        }
        {
        }
    }
    else
    {
        //Unhandled Interrupt
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
    RC6PPS = 0x0F; //RC6->EUSART1:TX1;    
    RX2DTPPS = 0x0B; //RB3->EUSART2:RX2;    
    RX1DTPPS = 0x17; //RC7->EUSART1:RX1;  
}
//---------------------------------------------------------

//================== END Of File ======================================