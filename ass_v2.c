#include "clic3.h"

const int Seg1 = 0;
const int Seg2 = 1;

uc_8 key;  // Temporary key holder
unsigned char elapsed_seconds;
unsigned int keypad_number;

// ************************************************************************
// Function to get a 2-digit number from the keypad
// ************************************************************************
unsigned int getTwoDigitKeypad(void) {
    char lcd_line[17];       // Internal LCD line buffer
    uc_8 digits[2];          // Internal storage for digits
    uc_8 digit_count = 0;    // Counter for digits
    unsigned int number = 0; // Final 2-digit number


    while (digit_count < 2) {
        if (keypadGet(&key)) {
            // Ignore keys 10-15 (A-F)
            if (key > 9) continue;

            // Store the digit
            digits[digit_count] = key;
            digit_count++;

            // Display digits as they are entered
            lcd_line[0] = '0' + digits[0];       // first digit
            if (digit_count == 2) {
                lcd_line[1] = '0' + digits[1];   // second digit
                lcd_line[2] = '\0';
            } else {
                lcd_line[1] = '\0';              // only one digit entered
            }
            lcdPut(lcd_line, 2);
        }
    }

    // Compute final number
    number = digits[0] * 10 + digits[1];

    return number;
}

unsigned char getElapsedSeconds(void) {
    unsigned int startTime, elapsedTime;
    unsigned char seconds;

    // Wait for switch press (S3 in this case)
    do {
        BusAddress = SwitchesAddr;
        BusRead();
    } while (!(BusData & 0x01));  // Loop until pressed

    startTime = TA0R;  // Capture start time

    // Wait for release
    do {
        BusAddress = SwitchesAddr;
        BusRead();
    } while (BusData & 0x01);

    // Compute elapsed time
    elapsedTime = TA0R - startTime;
    seconds = (elapsedTime / 1000) % 100;  // Convert to seconds (assumes 1ms ticks, 0–99)

    return seconds;
}

// ************************************************************************
// Main function
// ************************************************************************
void main(void) {
    Initialise();      // System initialization
    switchesInit();    
    LEDsInit();        
    sevenSegInit();    
    lcdInit();         
    keypadInit();      
    timerInit();
    lcdPut("Please enter two digit", 1);

    for (;;) {
        keypad_number = getTwoDigitKeypad();
        // Now 'keypad_number' contains the 2-digit number entered via keypad
        // You can use it here for LEDs, calculations, etc.


/*        BusData = LookupSeg[elapsed_seconds % 10];
        BusAddress = SegLow;
        BusWrite();

        BusData = LookupSeg[(elapsed_seconds / 10) % 10];
        BusAddress = SegHigh;
        BusWrite();*/
    }
}