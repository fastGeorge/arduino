#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

const int BUTTON_PIN = 2;
const int LED = 13;

bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

}

const uint32_t heart_animation[][4] = {
	{
		0x0,
		0x600600,
		0x0,
		66
	},
	{
		0x9,
		0x1f80f00,
		0x60000000,
		66
	},
	{
		0x19826,
		0x42041080,
		0x90060000,
		66
	},
	{
		0x39c46240,
		0x24022041,
		0x8090060,
		66
	}
};

void text_scroll(const char text[]) {
  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  // add the text
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}




void loop() {

  if(digitalRead(BUTTON_PIN) == HIGH){
    //digitalWrite(LED, HIGH);
    buttonPressed = true;
  }

  if(buttonPressed){
    text_scroll("    Hej, vet du en sak?    Jag   ");
      
    for(int i = 0; i < 32; i++){
      matrix.loadFrame(heart_animation[i % 4]);
      delay(100);
    }

    text_scroll("   dig Nelly!   ");
    buttonPressed = false;
    //digitalWrite(LED, LOW);
  }

  

}
