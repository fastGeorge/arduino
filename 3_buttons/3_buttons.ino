#include <TM1637Display.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "RTC.h"

//Pins on the arduino
  //buttons
const int BUTTON_0 = 2;
const int BUTTON_1 = 4;
const int BUTTON_2 = 7;
  //number display
const int CLK = 9;
const int DIO = 8;

int lastBState[3] = {LOW, LOW, LOW}; 
int currentBState[3] = {LOW, LOW, LOW};
int debounceTime = 50;

int alarmTime[2] = {8,30};

const uint8_t tSet[] = {
  SEG_F | SEG_G | SEG_E | SEG_D,          // t
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,  // S
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,  // E
  SEG_F | SEG_G | SEG_E | SEG_D           // t
};

const uint8_t aSet[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  // A
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,          // S
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,          // E
  SEG_F | SEG_G | SEG_E | SEG_D                   // t
};

//Inits

TM1637Display display(CLK, DIO);
ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(115200);
  Buttons:
  pinMode(BUTTON_0, INPUT_PULLUP);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  //Led display
  matrix.begin();
  matrix.stroke(0xFFFFFFFF);

  display.setBrightness(1);
  display.clear();

  RTC.begin();

  RTCTime startTime(1, Month::JANUARY, 2024, 00, 00, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);

  RTC.setTime(startTime);

}

void show_number(const char nbr[]) {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(255, 255, 255);
  matrix.textFont(Font_5x7);
  matrix.text(nbr, 2, 1);
  matrix.endDraw();
}

void displayTime(int hour, int minutes){
  int displayTime = hour * 100 + minutes;
  display.showNumberDecEx(displayTime, 0b01000000, true, 4, 0);
}

void timeInputButtonLoop(int time[]){
  int i = 0;
  while(i < 2){
    currentBState[1] = digitalRead(BUTTON_1);
    currentBState[2] = digitalRead(BUTTON_2);

    if(currentBState[1] != lastBState[1]){
      if(currentBState[1] == HIGH){
        if(i == 0){
          time[1] += 1;
          if(time[1] == 60){
            time[1] = 0;
          }
        } else if(i == 1){
          time[0] += 1;
          if(time[0] == 24){
            time[0] = 0;
          }
        } 
      }
      displayTime(time[0], time[1]);
    }

    if(currentBState[2] != lastBState[2]){
      if(currentBState[2] == HIGH){
        i += 1;
      }
    }

    delay(debounceTime);
    lastBState[1] = currentBState[1];
    lastBState[2] = currentBState[2];
  }
}

void setAlarm(){
  displayTime(alarmTime[0],alarmTime[1]);

  timeInputButtonLoop(alarmTime);
  display.setSegments(aSet);

  delay(1000);
}

void setClockTime(){

  RTCTime currentTime;
  RTC.getTime(currentTime);

  int newTime[2] = {currentTime.getHour(), currentTime.getMinutes()};

  displayTime(newTime[0], newTime[1]);

  timeInputButtonLoop(newTime);
  display.setSegments(tSet);

  //give new time to RTC
  RTCTime newRTCTime(1, Month::JANUARY, 2024, newTime[0], newTime[1], 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(newRTCTime);

  delay(1000);
}


void loop() {

  currentBState[0] = digitalRead(BUTTON_0);
  currentBState[1] = digitalRead(BUTTON_1);
  currentBState[2] = digitalRead(BUTTON_2);

  RTCTime currentTime;

  RTC.getTime(currentTime);

  displayTime(currentTime.getHour(), currentTime.getMinutes());

  if(currentBState[0] != lastBState[0]){
    lastBState[0] = currentBState[0];
    if (currentBState[0] == HIGH){
      show_number("1");
    }
  }

  if(currentBState[1] != lastBState[1]){
    lastBState[1] = currentBState[1];
    if (currentBState[1] == HIGH){
      show_number("2");
      setAlarm();
    }
  }

  if(currentBState[2] != lastBState[2]){
    lastBState[2] = currentBState[2];
    if (currentBState[2] == HIGH){
      setClockTime();
    }
  }
  
}
