#include "clic3.h"

void Initial(void);
void BusRead(void);
void BusWrite(void);
void Delay_ms(unsigned int ms);




// ===============================
//  Delay Function (~1ms @ 1MHz)
// ===============================
void Delay_ms(unsigned int ms) {
    while (ms--) {
        unsigned int i;
        for (i = 0; i < 1000; i++) {
            __no_operation(); // NOP = 1us @ 1MHz
        }
    }
}

// ===============================
//  Debounce Function
// ===============================
#define DEBOUNCE_DELAY_MS 20

unsigned char ReadSwitchDebounced(void) {
    unsigned char firstRead, secondRead;

    BusAddress = SwitchesAddr;
    BusRead();
    firstRead = BusData & 0x80;

    Delay_ms(DEBOUNCE_DELAY_MS);

    BusAddress = SwitchesAddr;
    BusRead();
    secondRead = BusData & 0x80;

    // Return stable state, else 0xFF
    if (firstRead == secondRead) {
        return firstRead;
    } else {
        return 0xFF;
    }
}

// ===============================
//  Globals
// ===============================
unsigned char Count = 0;
unsigned char prevSwitchState = 0;

// ===============================
//  Display Update Function
// ===============================
void UpdateDisplay(unsigned char value) {
    unsigned char ones = value % 10;
    unsigned char tens = (value / 10) % 10;

    BusData = LookupSeg[ones];
    BusAddress = Seg7AddrL;
    BusWrite();

    BusData = LookupSeg[tens];
    BusAddress = Seg7AddrH;
    BusWrite();
}

// ===============================
//  Main Program
// ===============================
void main(void) {
    Initial();

    UpdateDisplay(Count); // initialize display

    for (;;) {
      
// Read current LED state
BusAddress = LedsAddr;
BusRead();
unsigned char ledState = BusData;

// Read S3
BusAddress = SwitchesAddr;
BusRead();
unsigned char s3State = BusData & 0x80; // mask bit 7

// Update LED07 (active-low)
if (s3State) {
    ledState &= ~0x08;  // S3 = 1 ? LED07 OFF
} else {
    ledState |= 0x08;   // S3 = 0 ? LED07 ON
}

// Write back
BusData = ledState;
BusAddress = LedsAddr;
BusWrite();
        
        
        unsigned char currSwitchState = ReadSwitchDebounced();

        if (currSwitchState != 0xFF) { // stable read
            // Rising edge: OFF -> ON
            if (currSwitchState && !prevSwitchState) {
                if (Count + 5 > 99)
                    Count = 99;
                else
                    Count += 5;

                UpdateDisplay(Count);
            }

            // Falling edge: ON -> OFF
            if (!currSwitchState && prevSwitchState) {
                if (Count == 0)
                    Count = 0;
                else
                    Count -= 1;

                UpdateDisplay(Count);
            }

            prevSwitchState = currSwitchState;
        }
    }
}