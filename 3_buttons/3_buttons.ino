#include <TM1637Display.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "RTC.h"
#include "pitches.h"

//Pins on the arduino
  //buttons
const int BUTTON_0 = 2;
const int BUTTON_1 = 4;
const int BUTTON_2 = 7;
  //number display
const int CLK = 9;
const int DIO = 8;
  //speaker
const int SPEAKER = 10;

int lastBState[3] = {LOW, LOW, LOW}; 
int currentBState[3] = {LOW, LOW, LOW};
int debounceTime = 50;

int alarmTime[2] = {0,0};

bool alarm_set = false;

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

const uint8_t aOff[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  // A
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_A | SEG_E | SEG_F | SEG_G,                  // f
  SEG_A | SEG_E | SEG_F | SEG_G,                  // f
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

//Helper functions

void play_alarm(){
  int melody_length = 20;
  //Contains the notes
  int melody[] = {
    NOTE_C4, 
    NOTE_A3, NOTE_C4, NOTE_C4, NOTE_A4, NOTE_F4,
    NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_A3, NOTE_C4, NOTE_G4, NOTE_E4, NOTE_C4,
    NOTE_G4, NOTE_F4
  };
  // Contains duration
  int noteDurations[] = {
    32, 
    32, 16, 32, 16, 16,
    16, 24, 64, 32, 32, 32, 32,
    32, 16, 32, 32, 16, 32, 
    4
  };

  bool break_loop = false;
  while(!break_loop){
    for (int i = 0; i < melody_length; i++) {
      //check if alarm off is pressed:
      currentBState[0] = digitalRead(BUTTON_0);
      if(currentBState[0] != lastBState[0]){
        lastBState[0] = currentBState[0];
        if(currentBState[0] == HIGH){
          break_loop = true;
          break;
        }
      }

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 4000 / noteDurations[i];
      tone(10, melody[i], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(10);
    }
  }

  display.setSegments(aOff);
  delay(1000);
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
  alarm_set = true;

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

  //Handle alarm:
  if(currentTime.getHour() == alarmTime[0] &&
      currentTime.getMinutes() == alarmTime[1] &&
      alarm_set){
        alarm_set = false;
        play_alarm();
      }

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
