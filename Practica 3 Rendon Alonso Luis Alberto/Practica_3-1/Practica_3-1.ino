#include "DHT.h"
#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();

}

void loop() {
  delay(2000);
  float h=dht.readHumidity();
  float t = dht.readTemperature();

  if(isnan(h)||isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!"));
  }
  Serial.print(F("Humedad: "));
  Serial.print(h);
  Serial.print(F("% Temperatura: "));
  Serial.print(t);
  Serial.println(F("*C"));

}
