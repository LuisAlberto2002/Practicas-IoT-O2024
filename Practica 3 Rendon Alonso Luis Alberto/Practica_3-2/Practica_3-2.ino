#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,&Wire, -1);

#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

}

void loop() {
  delay(5000);
  float h=dht.readHumidity();
  float t = dht.readTemperature();

  if(isnan(h)||isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0,35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0,45);
  display.print(h);
  display.print("%");
  display.display();

}
