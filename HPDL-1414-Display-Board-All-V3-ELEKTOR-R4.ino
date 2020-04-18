/*
  HPDL-1414 Display Board by Roeland Riemens Feb 1 2020 strings in PROGAM MEMORY Version
  I2C address selection on Display board:
  set jumpers P4 P3 P2
              0  0  0  0x20
              0  0  1  0x21
              0  1  0  0x22
              0  1  1  0x23
              1  0  0  0x24
              1  0  1  0x25
              1  1  1  0x26

  P4  P3  P2 (SET TO 0 0 0 )
  X   X   X

  X   X   X
  |   |   |
  X   X   X

  I2C bus address is 0x20
  SDA connected to pin A4 (SDA) Arduino
  SCL connected to pin A4 (SCL) Arduino
  Display Board MCP23017-E/SP I2C to 16 bit controller
  Display positioning on GPIOB:
  bit 7 6 5 4 3 2 | 1 0
  =====================
       D D D D D | A A
       S S S S S | 1 0
       5 4 3 2 1 |

  Display select DS1 - DS5 = active low
  Segment 3 2 1 0 selected by A1 A0
  =================================
              0              0  0
              1              0  1
              2              1  0
              3              1  1

  Bit 7 for extra led / buzzer use.

  GPIOA is for tekst
  see ASCII table in datasheet HPDL-1414

  Bit 7 for extra led / buzzer use.

*/
// wire.h needed
#include "Wire.h"
const int maxSize = 21;
int i2caddress = 0x20;

const char tekst1[maxSize] PROGMEM  = "ABCDEFGHIJKLMNOPQRST";
const char tekst2[maxSize] PROGMEM  = "[<-*+0123456789+*->]";
const char tekst3[maxSize] PROGMEM  = "[CREATED BY ROELAND]";
const char tekst4[maxSize] PROGMEM  = "COPYROEL@ND 21022020";
const char tekst5[maxSize] PROGMEM  = "[HPDL-1414 DISPLAY!]";
char disptext[maxSize] = "                    ";
char buftmp_1[maxSize] = "                    ";
char buftmp_2[maxSize] = "                    ";
int PosTable[] = {0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x6f, 0x6e, 0x6d, 0x6c, 0x5f, 0x5e, 0x5d, 0x5c, 0x3f, 0x3e, 0x3d, 0x3c};
int wacht = 1000;
int karakter;
int n;
int o;
int p;
int q;

void setup()
{
  Wire.begin(); // wake up I2C bus
  // set I/O pins to outputs
  Wire.beginTransmission(i2caddress);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  Wire.beginTransmission(i2caddress);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission();
  //  Serial.begin(115200); // open the serial port at 115200 bps:
}

char displayRamString(char disptext[maxSize])
{ for (q = 0; q < 20; q++)
  {
    int pos = 0;
    char karakter = disptext[q];
    pos = PosTable[q];
    Wire.beginTransmission(0x20);
    Wire.write(0x13); // IODIRB register
    Wire.write(pos); // set position on port GPIOB
    Wire.endTransmission();
    Wire.beginTransmission(0x20);
    Wire.write(0x12); // GPIOA
    Wire.write(karakter); // write character at port GPIOA
    Wire.endTransmission();
    // disable selection of display GPIOB preventing display glitches/errors
    Wire.beginTransmission(0x20);
    Wire.write(0x13); // GPIOB
    Wire.write(0x7c); // clear pos on port GPIOB
    Wire.endTransmission();
    // data clear
    Wire.beginTransmission(0x20);
    Wire.write(0x12); // GPIOA
    Wire.write(0); // clear character at port GPIOA
    Wire.endTransmission();
  }
  delay(10);

}

char displayString(const char disptext[maxSize])
{
  // transfer data from disptext to SRAM buftmp_2
  for (n = 0; n < 20; n++) {
    buftmp_2[n] = pgm_read_byte_near(disptext + n);
  }
  displayRamString(buftmp_2);
}

char scrollInString(const char disptext[maxSize])
{
  // transfer data from disptext to SRAM buftmp_2
  for (n = 0; n < 20; n++) {
    buftmp_2[n] = pgm_read_byte_near(disptext + n);
  }
  // execute shift left complete buftmp_1
  for (o = 0; o < 20; o++)
  {
    // shift left buftmp_1 1 position <-
    for (p = 0; p < 20; p++) {
      buftmp_1[p] = buftmp_1[p + 1];
    }
    // insert corresponding new character into buftmp_1
    buftmp_1[19] = buftmp_2[o];
    // display shifted buftmp_1
    displayRamString(buftmp_1);
  }
  //keep buffers empty
  for (p = 0; p < 20; p++) {
    buftmp_1[p] = char(0x20);
    buftmp_2[p] = char(0x20);
  }
}

char scrollOutString(const char disptext[maxSize])
{
  // transfer data from disptext to SRAM buftmp_1
  for (n = 0; n < 20; n++) {
    buftmp_1[n] = pgm_read_byte_near(disptext + n);
  }
  // execute shift left complete buftmp_1
  for (o = 0; o < 20; o++)
  {
    // shift left buftmp_1 1 position <-
    for (p = 0; p < 20; p++) {
      buftmp_1[p] = buftmp_1[p + 1];
    }
    // insert corresponding new character into buftmp_1
    buftmp_1[19] = char(0x20);
    // display shifted buftmp_1
    displayRamString(buftmp_1);
  }
  //keep buffers empty
  for (p = 0; p < 20; p++) {
    buftmp_1[p] = char(0x20);
  }
}

char splitString(const char disptext[maxSize])
{
  // transfer data from disptext to SRAM buftmp_2
  for (n = 0; n < 20; n++) {
    buftmp_1[n] = pgm_read_byte_near(disptext + n);
  }
  displayRamString(buftmp_1);
  delay(wacht);
  for (o = 0; o < 10; o++)
  {
    for (p = 0; p < 10; p++) {
      buftmp_1[p] = buftmp_1[p + 1];
      buftmp_1[19 - p] = buftmp_1[19 - p - 1];
    }
    buftmp_1[9] = char(0x20);
    buftmp_1[10] = char(0x20);
    displayRamString(buftmp_1);
    delay(50);
  }
}

void loop()
{
  displayString(tekst1);
  delay(wacht);
  displayString(tekst2);
  delay(wacht);
  displayString(tekst3);
  delay(wacht);
  displayString(tekst4);
  delay(wacht);
  displayString(tekst5);
  delay(wacht);

  scrollInString(tekst1);
  delay(wacht);
  scrollOutString(tekst1);
  delay(wacht);

  scrollInString(tekst2);
  delay(wacht);
  scrollOutString(tekst2);
  delay(wacht);

  scrollInString(tekst3);
  delay(wacht);
  scrollOutString(tekst3);
  delay(wacht);

  scrollInString(tekst4);
  delay(wacht);
  scrollOutString(tekst4);
  delay(wacht);

  scrollInString(tekst5);
  delay(wacht);
  scrollOutString(tekst5);
  delay(wacht);

  splitString(tekst1);
  delay(wacht);
  splitString(tekst2);
  delay(wacht);
  splitString(tekst3);
  delay(wacht);
  splitString(tekst4);
  delay(wacht);
  splitString(tekst5);
  delay(wacht);

}
