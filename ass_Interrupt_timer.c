#include "clic3.h"

volatile unsigned int seconds = 0;

void updateSevenSegDisplay(unsigned char seconds) {
    // Display units (right)
    BusAddress = Seg7AddrL;
    BusData = LookupSeg[seconds % 10];
    BusWrite();

    // Display tens (left)
    BusAddress = Seg7AddrH;
    BusData = LookupSeg[(seconds / 10) % 10];
    BusWrite();
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog first
    Initialise();


    // =================================
    // Clock Setup (1 MHz DCO, REFO=ACLK)
    // =================================
    UCSCTL3 = SELREF__REFOCLK;          
    UCSCTL4 = SELA__REFOCLK | SELS__DCOCLK | SELM__DCOCLK;  
    UCSCTL0 = 0x0000;                   
    UCSCTL1 = DCORSEL_5;                
    UCSCTL2 = FLLD_1 + 30;              
    UCSCTL8 |= ACLKREQEN;               // ? Enable ACLK request (important)

    __delay_cycles(750000);             // Allow DCO to settle

    // =================================
    // Timer_A0 Setup (1s interrupt)
    // =================================
    TA0CTL = TASSEL_1 + MC_1 + TACLR;   // ACLK, Up mode
    TA0CCR0 = 32768 - 1;                // 1-second
    TA0CCTL0 = CCIE;                    // Enable interrupt

    __enable_interrupt();               // Enable GIE

    while (1) {
        // main loop work
      int k = 1;
    }
}

// =================================
// Timer ISR (fires every 1s)
// =================================
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    seconds++;
    updateSevenSegDisplay(seconds);
}