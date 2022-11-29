
#include "Wire.h"
#include "DHT.h"
#include <U8x8lib.h>
#define DHTTYPE DHT20

DHT dht(DHTTYPE);
 
#if defined(ARDUINO_ARCH_AVR)
    #define debug  Serial
 
#elif defined(ARDUINO_ARCH_SAMD) ||  defined(ARDUINO_ARCH_SAM)
    #define debug  SerialUSB
#else
    #define debug  Serial
#endif

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

int rotaryPin = A0;
int ledPin = 4;
int buzzerPin = 5;
int buttonPin = 6;

int buttonState = 0;
int degreeState = 0;

char degree = 'C';

float hum = 0;

float tempC = 0;
float tempF = 0;

float rotaryValue = 0;
float maxTemp = 0;
 
void setup() {
 
    debug.begin(9600);
    debug.println("Temperature Sensor On!");

    pinMode(ledPin, OUTPUT);
    pinMode(rotaryPin, INPUT);

    Wire.begin();
    dht.begin();

    u8x8.begin();
    u8x8.setPowerSave(0);  
    u8x8.setFlipMode(1);

}
 
void loop() {
    rotaryValue = (1023 - analogRead(rotaryPin));
    maxTemp = 21 + (rotaryValue/75);

    checkDegreeSetting();
    getTemp();
    checkAlarm();
    printTemp();
    delay(1500);
}

void checkDegreeSetting() {
  buttonState = digitalRead(buttonPin);
    if(buttonState == 1) {
      if(degreeState == 0){
        degreeState = 1;
      } else {
        degreeState = 0;
      }
    }
    if(degreeState == 1) {
      degree = 'F';
      tempF = ((tempC * 1.8)+32);
      maxTemp = ((maxTemp * 1.8)+32);
      buttonState = LOW;
    }
    else if(degreeState == 0) {
      degree = 'C';
      tempF = tempC;
      buttonState = LOW;
    }
}

void getTemp() {
  float temp_hum_val[2] = {0};
  if (!dht.readTempAndHumidity(temp_hum_val)) {
        tempC = temp_hum_val[1];
        hum = temp_hum_val[0];
    } else {
        debug.println("Failed to get temprature and humidity value.");
    }
}

void checkAlarm() {
  if(degreeState == 0) {
        if(tempF >= maxTemp && maxTemp > 21.5) {
          digitalWrite(ledPin, HIGH);
          analogWrite(buzzerPin, 128);
      } else {
          digitalWrite(ledPin, LOW);
          analogWrite(buzzerPin, 0);
      }
    } else {
        if(tempF >= maxTemp && maxTemp > 70.7) {
          digitalWrite(ledPin, HIGH);
          analogWrite(buzzerPin, 128);
      }  else {
          digitalWrite(ledPin, LOW);
          analogWrite(buzzerPin, 0);
      } 
    }
}

void printTemp() {
    u8x8.setFont(u8x8_font_chroma48medium8_r);

    u8x8.setCursor(0, 0);
    u8x8.print("Temp:");
    u8x8.print(tempF);
    u8x8.print(degree);

    debug.println("Temprature: ");
    debug.print(tempF);
    debug.print("  ");

    u8x8.setCursor(0,25);
    u8x8.print("Humidity:");
    u8x8.print(hum);
    u8x8.print("%");

    debug.print("Humidity: ");
    debug.print(hum);
    debug.print("  ");

    u8x8.setCursor(0,50);
    u8x8.print("Crit Temp:");

    if(degreeState == 0){
      if(maxTemp < 21.5) {
        u8x8.print("Off");
        u8x8.print(degree);
        u8x8.print("  ");
      } else {
      u8x8.print(maxTemp);
      u8x8.print(degree);
      }
    } else {
      if(maxTemp < 70.7) {
        u8x8.print("Off");
        u8x8.print(degree);
        u8x8.print("  ");
      } else {
      u8x8.print(maxTemp);
      u8x8.print(degree);
      }
    }

      debug.print("Crit Temp: ");
      debug.print(maxTemp);
      debug.print("  ");

    u8x8.refreshDisplay();
}