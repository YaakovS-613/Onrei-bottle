#include <DallasTemperature.h>
#include <OneWire.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int PinForward=5;
int PinBackward=4;
int SET_T = 70;
int heatpin = 3;
int coldpin = 2;
int HEATBUTTONp = 0;
int COLDBUTTONp = 0;
bool FAHRENHEIT = TRUE;
const int ONE_WIRE_BUS = A0;
float temperature = 0;

// Declaration for an SSD1306 display connected to I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // -1 means no hardware reset pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void disptemp() {
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Set Temp = ");
  display.print(SET_T);
  display.setTextSize(0);
  display.print("o");

  display.setTextSize(1);
  display.setCursor(115, 0);
  if (FAHRENHEIT) {
    display.print("F");
  } else {
    display.print("C");
  }

  display.setTextSize(6);
  display.setCursor(25,17);
  display.print(CURRENT);
  display.setTextSize(2);
  display.setCursor(95, 17);
  display.print("o");
  // update display
  display.display();
}

void change_peltier_temp() {
  if (temperature < SET_T) {
    digitalWrite(PinForward,HIGH);
    digitalWrite(PinBackward,LOW);
  }
  if (temperature > SET_T) {
    digitalWrite(PinForward,LOW);
    digitalWrite(PinBackward,HIGH);
  }
   if (temperature == SET_T) {
    digitalWrite(PinForward,LOW);
    digitalWrite(PinBackward,LOW);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensors.begin();
  sensors.setResolution(9);
  pinMode(PinForward,OUTPUT);
  pinMode(PinBackward,OUTPUT);
  digitalWrite(PinForward,LOW);
  digitalWrite(PinBackward,LOW);
  pinMode(heatpin, INPUT_PULLUP);
  pinMode(coldpin, INPUT_PULLUP);
  
  // Initialize the display with the I2C address 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  disptemp();
}

void loop() {
  unsigned long lastTempRead = 0;
  const unsigned long TEMP_READ_DELAY = 1000;
  HEATBUTTONp = digitalRead(heatpin);
  COLDBUTTONp = digitalRead(coldpin);
    if (HEATBUTTONp == HIGH) {
    SET_T ++;
  } else if (COLDBUTTONp == HIGH) {
    SET_T --;
  } else if (HEATBUTTONp == HIGH and COLDBUTTONp == HIGH) {
    if (FAHRENHEIT == TRUE){
      SET_T = (SET_T - 32) / 1.8;
      FAHRENHEIT = FALSE;
    }else{
      SET_T = (SET_T * 1.8)+32;
      FAHRENHEIT = TRUE;
    }
    delay(100);
  }

  if (millis() - lastTempRead >= TEMP_READ_DELAY) {
  lastTempRead = millis();

  sensors.requestTemperatures();

  if (FAHRENHEIT == 1) {
    CURRENT = sensors.getTempFByIndex(0);
  } else {
    CURRENT = sensors.getTempCByIndex(0);
  }

  disptemp();
  }
  changetemp();
}
