#include <DHT.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>
#include <WiFi.h>

#define SECRET_SSID "IoT"
#define SECRET_PASS "1t3s0IoT23"
#define OPENWEATHER_REQUEST_PATH "https://api.openweathermap.org/data/2.5/weather?lat=20.60754&lon=-103.41671&appid=3551bfbb7d1890eb458bfcba7c70af2d&units=metric"
#define SECRET_CH_ID 2720648
#define SECRET_WRITE_APIKEY "VMK6Y7B6F17VDWQF" 

// Temperature Data from sensors
struct TemperatureData
{
  float dht;
  // in Celsius
  String getAllTemperatureData(){
    return "\nDHT: " + (String)dht + "*C"; 
  }
};

// dht11
const uint8_t dhtPin = 15;
const uint8_t dhtType = DHT11;
DHT dht(dhtPin, dhtType);


// data
TemperatureData temperatureData;

// timer
uint32_t startMillis;
const uint16_t timerDuration = 30000;

// wifi credentials
const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;
WiFiClient  client;

// OpenWeather
String apiTemperature;
String serverPath = OPENWEATHER_REQUEST_PATH;


// ThingSpeak
uint32_t channelNumber = SECRET_CH_ID;
const char *writeAPIKey = SECRET_WRITE_APIKEY;

void setup()
{
    Serial.begin(115200);
    dht.begin();

    ThingSpeak.begin(client);  // Initialize ThingSpeak client
    connectWiFi();

    // start timer
    startMillis = millis();
}

void loop()
{
    if (millis() >= timerDuration + startMillis)
    {
        // get sensors readings
        temperatureData.dht = getDHTTemperatureC();

        // get API temp
        apiTemperature = getTemperatureFromAPI();

        // debug on serial monitor
        Serial.println(temperatureData.getAllTemperatureData());
        Serial.println("Temperatura de la API: " + apiTemperature + "*C");
        delay(50);

        sendDataToThingSpeak();

        // restart
        startMillis = millis();
    }
}

// Helper functions
void connectWiFi(){
    
  WiFi.begin(ssid, password); 
 
  Serial.println(); 
  Serial.print("Wait for WiFi..."); 
  Serial.print(ssid);
   
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(500); 
  } 
   
  Serial.println(""); 
  Serial.println("WiFi Connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
}

void sendDataToThingSpeak(){
    // Check connection or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("No se detecto conexion a Internet");
        connectWiFi();
    }


    // first set the fields
    ThingSpeak.setField(1, temperatureData.dht);
    ThingSpeak.setField(2, apiTemperature);

    // set a status message based on any logic
    float indoorTempAverage =  temperatureData.dht;
    float outdoorTemp = apiTemperature.toFloat();
    String status;

    if (indoorTempAverage > 30)
        status = "Temperatura Relativamente Alta";
    else if (indoorTempAverage <= 30 && indoorTempAverage >= 20)
        status = "Temperatura Normal";
    else if (indoorTempAverage < 20)
        status = "Temperatura Relativamente Baja";

    // set status
    ThingSpeak.setStatus(status);

    // set a twittwer
    //String tweet = "Proyecto IoT, mensaje AUTOMATICO - La temperatura actual en mi ciudad es: " + apiTemperature + "*C";
    //ThingSpeak.setTwitterTweet("lupitatg", tweet);
    
     // write to the ThingSpeak Channel
    int code = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
    if(code == 200)
        Serial.println("Canal de Thingspeak actualizado correctamente.");  
    else
        Serial.println("Hubo un problema actualizando el canal. Error HTTP: " + String(code));

    
}


float getDHTTemperatureC(){
    float t = dht.readTemperature(false);

    if (isnan(t)){
        t = 0;
        Serial.println("Lectura fallida del sensor DHT");
    }
    return t;    
}


String getTemperatureFromAPI()
{
    // Check connection or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("No se detecto conexion a Internet");
        connectWiFi();
    }

    
    HTTPClient http;

    // Domain + URL path or IP with path
    http.begin(serverPath.c_str());
    
    String temperature;
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200)
    {
        String payload = http.getString();

        // filter temperature
        int index = payload.indexOf("\"temp\":");
        temperature = payload.substring(index + 7, payload.indexOf(",\"feels_"));
    }
    else
    {
        Serial.print("Peticion Get fallo, codigo de Error: ");
        Serial.println(httpResponseCode);
        temperature = "0.0"; // if fails
    }
    // free
    http.end();    

    return temperature;
}