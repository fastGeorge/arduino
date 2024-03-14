#include <Arduino.h>
#include <TM1637Display.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"



#define TEST_DELAY 2000




//Pins on the arduino
  //buttons
const int BUTTON_1 = 2;
const int BUTTON_2 = 4;
const int BUTTON_3 = 7;
  //number display
const int CLK = 9;
const int DIO = 8;


//Inits

TM1637Display display(CLK, DIO);
ArduinoLEDMatrix matrix;



void setup() {
  Serial.begin(115200);
  Buttons:
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  //Led display
  matrix.begin();
  matrix.stroke(0xFFFFFFFF);

  display.setBrightness(1);
  display.clear();
}

void show_number(const char nbr[]) {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(255, 255, 255);
  matrix.textFont(Font_5x7);
  matrix.text(nbr, 2, 1);
  matrix.endDraw();
}


void loop() {

  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };

  if(digitalRead(BUTTON_1) == HIGH){
    show_number("1");
    display.showNumberDec(1, false);
  }

  if (digitalRead(BUTTON_2) == HIGH){
    show_number("2");
    display.showNumberDec(2, false);
  }
  
  if (digitalRead(BUTTON_3) == HIGH){
    show_number("3");
    display.showNumberDec(3, false);
  }

  


  

}
