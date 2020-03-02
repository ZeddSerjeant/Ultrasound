// Contains random definitions

const unsigned char charset[][5] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00 },  // 20 space
  { 0x00, 0x00, 0x5f, 0x00, 0x00 },  // 21 !
  { 0x00, 0x07, 0x00, 0x07, 0x00 },  // 22 "
  { 0x14, 0x7f, 0x14, 0x7f, 0x14 },  // 23 #
  { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },  // 24 $
  { 0x23, 0x13, 0x08, 0x64, 0x62 },  // 25 %
  { 0x36, 0x49, 0x55, 0x22, 0x50 },  // 26 &
  { 0x00, 0x05, 0x03, 0x00, 0x00 },  // 27 '
  { 0x00, 0x1c, 0x22, 0x41, 0x00 },  // 28 (
  { 0x00, 0x41, 0x22, 0x1c, 0x00 },  // 29 )
  { 0x14, 0x08, 0x3e, 0x08, 0x14 },  // 2a *
  { 0x08, 0x08, 0x3e, 0x08, 0x08 },  // 2b +
  { 0x00, 0x50, 0x30, 0x00, 0x00 },  // 2c ,
  { 0x08, 0x08, 0x08, 0x08, 0x08 },  // 2d -
  { 0x00, 0x60, 0x60, 0x00, 0x00 },  // 2e .
  { 0x20, 0x10, 0x08, 0x04, 0x02 },  // 2f /
  { 0x3e, 0x51, 0x49, 0x45, 0x3e },  // 30 0
  { 0x00, 0x42, 0x7f, 0x40, 0x00 },  // 31 1
  { 0x42, 0x61, 0x51, 0x49, 0x46 },  // 32 2
  { 0x21, 0x41, 0x45, 0x4b, 0x31 },  // 33 3
  { 0x18, 0x14, 0x12, 0x7f, 0x10 },  // 34 4
  { 0x27, 0x45, 0x45, 0x45, 0x39 },  // 35 5
  { 0x3c, 0x4a, 0x49, 0x49, 0x30 },  // 36 6
  { 0x01, 0x71, 0x09, 0x05, 0x03 },  // 37 7
  { 0x36, 0x49, 0x49, 0x49, 0x36 },  // 38 8
  { 0x06, 0x49, 0x49, 0x29, 0x1e },  // 39 9
  { 0x00, 0x36, 0x36, 0x00, 0x00 },  // 3a :
  { 0x00, 0x56, 0x36, 0x00, 0x00 },  // 3b ;
  { 0x08, 0x14, 0x22, 0x41, 0x00 },  // 3c <
  { 0x14, 0x14, 0x14, 0x14, 0x14 },  // 3d =
  { 0x00, 0x41, 0x22, 0x14, 0x08 },  // 3e >
  { 0x02, 0x01, 0x51, 0x09, 0x06 },  // 3f ?
  { 0x32, 0x49, 0x79, 0x41, 0x3e },  // 40 @
  { 0x7e, 0x11, 0x11, 0x11, 0x7e },  // 41 A
  { 0x7f, 0x49, 0x49, 0x49, 0x36 },  // 42 B
  { 0x3e, 0x41, 0x41, 0x41, 0x22 },  // 43 C
  { 0x7f, 0x41, 0x41, 0x22, 0x1c },  // 44 D
  { 0x7f, 0x49, 0x49, 0x49, 0x41 },  // 45 E
  { 0x7f, 0x09, 0x09, 0x09, 0x01 },  // 46 F
  { 0x3e, 0x41, 0x49, 0x49, 0x7a },  // 47 G
  { 0x7f, 0x08, 0x08, 0x08, 0x7f },  // 48 H
  { 0x00, 0x41, 0x7f, 0x41, 0x00 },  // 49 I
  { 0x20, 0x40, 0x41, 0x3f, 0x01 },  // 4a J
  { 0x7f, 0x08, 0x14, 0x22, 0x41 },  // 4b K
  { 0x7f, 0x40, 0x40, 0x40, 0x40 },  // 4c L
  { 0x7f, 0x02, 0x0c, 0x02, 0x7f },  // 4d M
  { 0x7f, 0x04, 0x08, 0x10, 0x7f },  // 4e N
  { 0x3e, 0x41, 0x41, 0x41, 0x3e },  // 4f O
  { 0x7f, 0x09, 0x09, 0x09, 0x06 },  // 50 P
  { 0x3e, 0x41, 0x51, 0x21, 0x5e },  // 51 Q
  { 0x7f, 0x09, 0x19, 0x29, 0x46 },  // 52 R
  { 0x46, 0x49, 0x49, 0x49, 0x31 },  // 53 S
  { 0x01, 0x01, 0x7f, 0x01, 0x01 },  // 54 T
  { 0x3f, 0x40, 0x40, 0x40, 0x3f },  // 55 U
  { 0x1f, 0x20, 0x40, 0x20, 0x1f },  // 56 V
  { 0x3f, 0x40, 0x38, 0x40, 0x3f },  // 57 W
  { 0x63, 0x14, 0x08, 0x14, 0x63 },  // 58 X
  { 0x07, 0x08, 0x70, 0x08, 0x07 },  // 59 Y
  { 0x61, 0x51, 0x49, 0x45, 0x43 },  // 5a Z
  { 0x00, 0x7f, 0x41, 0x41, 0x00 },  // 5b [
  { 0x02, 0x04, 0x08, 0x10, 0x20 },  // 5c backslash 
  { 0x00, 0x41, 0x41, 0x7f, 0x00 },  // 5d ]
  { 0x04, 0x02, 0x01, 0x02, 0x04 },  // 5e ^
  { 0x40, 0x40, 0x40, 0x40, 0x40 },  // 5f _
  { 0x00, 0x01, 0x02, 0x04, 0x00 },  // 60 `
  { 0x20, 0x54, 0x54, 0x54, 0x78 },  // 61 a
  { 0x7f, 0x48, 0x44, 0x44, 0x38 },  // 62 b
  { 0x38, 0x44, 0x44, 0x44, 0x20 },  // 63 c
  { 0x38, 0x44, 0x44, 0x48, 0x7f },  // 64 d
  { 0x38, 0x54, 0x54, 0x54, 0x18 },  // 65 e
  { 0x08, 0x7e, 0x09, 0x01, 0x02 },  // 66 f
  { 0x0c, 0x52, 0x52, 0x52, 0x3e },  // 67 g
  { 0x7f, 0x08, 0x04, 0x04, 0x78 },  // 68 h
  { 0x00, 0x44, 0x7d, 0x40, 0x00 },  // 69 i
  { 0x20, 0x40, 0x44, 0x3d, 0x00 },  // 6a j 
  { 0x7f, 0x10, 0x28, 0x44, 0x00 },  // 6b k
  { 0x00, 0x41, 0x7f, 0x40, 0x00 },  // 6c l
  { 0x7c, 0x04, 0x18, 0x04, 0x78 },  // 6d m
  { 0x7c, 0x08, 0x04, 0x04, 0x78 },  // 6e n
  { 0x38, 0x44, 0x44, 0x44, 0x38 },  // 6f o
  { 0x7c, 0x14, 0x14, 0x14, 0x08 },  // 70 p
  { 0x08, 0x14, 0x14, 0x18, 0x7c },  // 71 q
  { 0x7c, 0x08, 0x04, 0x04, 0x08 },  // 72 r
  { 0x48, 0x54, 0x54, 0x54, 0x20 },  // 73 s
  { 0x04, 0x3f, 0x44, 0x40, 0x20 },  // 74 t
  { 0x3c, 0x40, 0x40, 0x20, 0x7c },  // 75 u
  { 0x1c, 0x20, 0x40, 0x20, 0x1c },  // 76 v
  { 0x3c, 0x40, 0x30, 0x40, 0x3c },  // 77 w
  { 0x44, 0x28, 0x10, 0x28, 0x44 },  // 78 x
  { 0x0c, 0x50, 0x50, 0x50, 0x3c },  // 79 y
  { 0x44, 0x64, 0x54, 0x4c, 0x44 },  // 7a z
  { 0x00, 0x08, 0x36, 0x41, 0x00 },  // 7b {
  { 0x00, 0x00, 0x7f, 0x00, 0x00 },  // 7c |
  { 0x00, 0x41, 0x36, 0x08, 0x00 },  // 7d }
  { 0x10, 0x08, 0x08, 0x10, 0x08 },  // 7e ~
  { 0x00, 0x00, 0x00, 0x00, 0x00 }   // 7f 
};

#define _XTAL_FREQ 4000000 // Allows delays to work

//Define pins
#define PIN_CLOCK 4
#define PIN_DATA_IN 3
#define PIN_DATA_OR_COMMAND 2
#define PIN_RESET 1
#define PIN_ENABLE 0
unsigned char PIN_SETTINGS = 0x0; // It seems altering PORTC directly is dodgy

#define OUTPUT 0

#define HIGH 1
#define LOW 0

// Commands and data
#define COMMAND LOW
#define DATA HIGH

// Instruction Bytes
#define INSTRUCTON_SET_EXTENDED 0x21
#define INSTRUCTON_SET_BASIC 0x20

#define BIAS 0x13 // bias = 1:48
#define VOP 0xc2 // 7Vpp Operating point

#define DISPLAY_ON 0x09 // Turn all display segments on
#define BLANK_DISPLAY 0x08 // make the display blank
#define NORMAL_MODE 0x0c // Display is black on white
#define INVERSE_MODE 0x0d // Display is white on black

#define DISPLAY_WIDTH 84 // Width of the display
#define DISPLAY_HEIGHT 48 // Height of the display
unsigned char current_column = 0;
unsigned char current_line = 0;

unsigned char character_A[5] = { 0x7e, 0x11, 0x11, 0x11, 0x7e };


/*Write HIGH or LOW to a particular PIN (PORTC)*/
void writePIN(int pin, int level)
{
    if (level == HIGH)
    {
        PIN_SETTINGS |= (unsigned char)(1 << pin); // Set PIN HIGH
    }
    else
    {
        PIN_SETTINGS &= ~((unsigned char)(1 << pin)); // Set PIN LOW
    }

    PORTC = PIN_SETTINGS; // The pins have changed, update the PORT
}

/* Send data along a line using essentially UART, to recreate SPI (MSB first) */
void shiftOutData(unsigned char data)
{
    unsigned char data_bit; // store a single bit of the data for transfer

    writePIN(PIN_CLOCK, LOW); // Set the clock low in preparation

    for (int i=7; i>=0; i--)
    {
        data_bit = (unsigned char)((1 << i) & data); // zero for a 1 in that bit position, non-zero otherwise
        if (data_bit) // if a 1 bit
        {
            writePIN(PIN_DATA_IN, HIGH); // send a 1 bit
        }
        else // if a 0 bit
        {
            writePIN(PIN_DATA_IN, LOW); // send a 0 bit
        }

        // __delay_ms(20); // Allow the pin to stabilize

        writePIN(PIN_CLOCK, HIGH); // samples on a positive edge

        // __delay_ms(20);

        writePIN(PIN_CLOCK, LOW); // drop the clock low again

        //__delay_ms(20);
    }
}

/* Send some data, either a command for the LCD to do something, or something to display */
void sendData(int data_type, unsigned char data)
{
    writePIN(PIN_DATA_OR_COMMAND, data_type); // Set the pin to tell the LCD whether this is command or data

    writePIN(PIN_ENABLE, LOW); // Turn off the display so it can be updated
    shiftOutData(data); // Send the data
    writePIN(PIN_ENABLE, HIGH); // Turn the display back on
}

/* print some character to the screen */
void printChar(unsigned char *character)
{
    //Output the columns
    // writePIN(PIN_ENABLE, LOW); // Disable the display
    for (int i = 0; i<5; i++)
    {
        sendData(DATA, character[i]); // Send each column of the symbol

    }
    // writePIN(PIN_ENABLE, HIGH); // Re-enable the display
    // Leave pne column between characters
    sendData(DATA, 0x00);

    //Update the cursor
    current_column = (current_column + 6) % DISPLAY_WIDTH;
    if (current_column == 0)
    {
        current_line = (current_line + 1) % (DISPLAY_HEIGHT/9 + 1);
    }
}

void printString(char *string)
{
    int index = 0;
    char current_char;

    while (1) // Loop until end of string
    {
        current_char = string[index];
        if (current_char == 0x0) // Null char, end of string
        {
            break;
        }
        // Assume char is in printable ascii, which is above space
        printChar(charset[current_char - ' ']); // Print the character
        index++;
    }

}

/* Print a 4 digit number */
void printNumber(int number)
{

    int digit;
    int power;
    for (int i=3; i>=1; i--)
    {
        power = 1;
        for (int p=1; p<i; p++)
        {
            power = 10*power;
        }
        
        digit = (number / power) % 10;
        printChar(charset[digit + 16]);
    }  
}

// void printFloat(float num){
//   int number = (int)(num * 100);

//   int digit;
//   int power;
//   for (int i=4; i>=1; i--)
//   {
//       power = 1;
//       for (int p=1; p<i; p++)
//       {
//           power = 10*power;
//       }
//       digit = (number / power) % 10;
//       if(i == 2){
//         printString(".");  //print '.'
//       }
//       printChar(charset[digit + 16]);
//   }
//   //printChar("%");
// }

void setCursor(unsigned char column, unsigned char line)
{
    current_column = column % DISPLAY_WIDTH;
    current_line = line % (DISPLAY_HEIGHT/9 + 1);
    sendData(COMMAND, 0x80 | current_column);
    sendData(COMMAND, 0x40 | current_line);
}

void clearRam()
{
    setCursor(0, 0); // Move home
    //Move along setting the display to 0
    for (unsigned short i=0; i<DISPLAY_WIDTH*(DISPLAY_HEIGHT/8); i++)
    {
        sendData(DATA, 0x00);
    }
    setCursor(0, 0); // Move home after this operation
}


/* Initialize the LCD */
void initLCD()
{
    TRISC = OUTPUT; // Set all of port C to be digital output pins
    // Reset the LCD
    writePIN(PIN_RESET, HIGH);
    writePIN(PIN_RESET, HIGH);
    writePIN(PIN_RESET, LOW);
    __delay_ms(100); // Allow the pins to settle
    writePIN(PIN_RESET, HIGH);

    // Set LCD parameters
    sendData(COMMAND, INSTRUCTON_SET_EXTENDED); // turn on extended instruction set
    sendData(COMMAND, BIAS); // Set the bias
    sendData(COMMAND, VOP); // Set the operating point
    sendData(COMMAND, INSTRUCTON_SET_BASIC); // Return to the basic instruction set
    sendData(COMMAND, DISPLAY_ON); // Turn on the display

    clearRam(); // Clear the ram so the display is clear

    // Activate the LCD
    sendData(COMMAND, BLANK_DISPLAY); // Make the display blank 
    sendData(COMMAND, NORMAL_MODE); // black on white display
    __delay_ms(100);

    //Cursor at origin
    setCursor(0, 0);
}
