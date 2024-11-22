/*
 * File:  mayen.h
 * Author: Olivier POCHON
 * Company: 
 * Desc.: include programme principale
 * 
 * Ver. Date: 230102 V00 Création (YYYYMMDD)
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef My_MAYEN_H
#define	My_MAYEN_H

//==============================================================================
//gestion IO
//==============================================================================

//LED___________________________________________________________________________
#define LEDR_OFF()            do { LATBbits.LATB4 = 1; } while(0)
#define LEDR_ON()             do { LATBbits.LATB4 = 0; } while(0)
#define LEDR_Toggle()         do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)

#define LEDV_OFF()            do { LATBbits.LATB5 = 1; } while(0)
#define LEDV_ON()             do { LATBbits.LATB5 = 0; } while(0)
#define LEDV_Toggle()         do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)

//Button________________________________________________________________________
#define BTN1_GetValue()       PORTCbits.RC0
#define BTN2_GetValue()       PORTCbits.RC1
#define BTN3_GetValue()       PORTCbits.RC2

#define PRESS   0
#define UNPRESS 1

//Power Key____________________________________________________________________
#define POWERKEY_ON()         do { LATCbits.LATC5 = 1; } while(0)
#define POWERKEY_OFF()        do { LATCbits.LATC5 = 0; } while(0)
#define POWERKEY_Toggle()     do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
//----------------------------------------------------------


//----- Function Declarations -----------------------------
void Modem_BOOT(void);
void PIC_Init(void);
//----------------------------------------------------------


//----- Global variable  -----------------------------


#endif	/* My_MAYEN_H */

