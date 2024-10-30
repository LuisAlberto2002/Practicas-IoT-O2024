/**************************************** 
 * Include Libraries 
 ****************************************/ 
#include <WiFi.h> 
#include <PubSubClient.h> 
 
/**************************************** 
 *  
 * Define Constants 
 ****************************************/ 
#define WIFISSID "IoT" //WIFI SSID aqui 
#define PASSWORD "1t3s0IoT23" // WIFI pwd 
#define TOKEN "BBUS-cDAteYkq9szRvaSpA4JTPfgqF1pC3H" // Token Ubidots Luis Alberto
#define MQTT_CLIENT_NAME "LARASENSOR" //ID del cliente, 8 a 12 chars alfanumericos (ASCII), debe ser random y unico dif a otros devices 
#define VARIABLE_LABEL_temp "temperatura" // Variable Temperatura 56
#define VARIABLE_LABEL_hum "humedad" // Variable Humedad 
#define DEVICE_LABEL "Luis-ESP32-DEVICE" // Nombre del dispositivo a crear 
 
char mqttBroker[]= "industrial.api.ubidots.com"; 
char payload[200]; // Leer y entender el payload aqui una de tantas referencias https://techterms.com/definition/payload
char topic[150]; //Espacio para el nombre del topico 
 
// Space to store values to send 
char str_temp[10]; 
char str_hum[10]; 
 
 
#include "DHT.h" 
#define pin1 15 
DHT dht1(pin1, DHT11);    //El sensor de temp y humedad azul. 
 
/**************************************** 
 * Funciones auxiliares 
 ****************************************/ 
WiFiClient ubidots; 
PubSubClient client(ubidots); 
 
void callback(char* topic, byte* payload, unsigned int length) { 
  char p[length + 1]; 
  memcpy(p, payload, length); 
  p[length] = NULL; 
  String message(p); 
  Serial.write(payload, length); 
  Serial.println(topic); 
} 
 
void reconnect() { 
  // Loop until we're reconnected 
  while (!client.connected()) { 
    Serial.println("Attempting MQTT connection..."); 
     
    // Attemp to connect 
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) { 
      Serial.println("Connected"); 
    } else { 
      Serial.print("Failed, rc="); 
      Serial.print(client.state()); 
      Serial.println(" try again in 2 seconds"); 
      // Wait 2 seconds before retrying 
      delay(2000); 
    } 
  } 
} 
 
/**************************************** 
 * Main Functions 
 ****************************************/ 
void setup() { 
  Serial.begin(115200); 
  WiFi.begin(WIFISSID, PASSWORD); 
 
   
  Serial.println(); 
  Serial.print("Wait for WiFi..."); 
   
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(500); 
  } 
   
  Serial.println(""); 
  Serial.println("WiFi Connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
  client.setServer(mqttBroker, 1883); 
  client.setCallback(callback);   
 
  // Sensor de temp y humedad 
  dht1.begin(); 
} 
 
void loop() { 
  if (!client.connected()) { 
    reconnect(); 
  } 
  //Leer los datos del sensor
  Serial.println(); 
  Serial.println("A continuación los datos de los sensores:");
  Serial.println();
  // Publica en el topic de temperatura 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); // Cleans the payload 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_temp); // Adds the variable label 
  float t1 = dht1.readTemperature(); 
  Serial.print("  1. La temperatura detectada en el sensor es de: ");
  Serial.println(t1); // Imprime temperatura en el serial monitor  
 
  // numero maximo 4 precision 2 y convierte el valor a string
  dtostrf(t1, 4, 2, str_temp); 
    
  sprintf(payload, "%s {\"value\": %s", payload, str_temp); // formatea el mensaje a publicar 
  sprintf(payload, "%s } }", payload); // cierra el mensaje 
  Serial.println("  Enviando la Temp al virtualdevice en ubidots via MQTT....");  
  client.publish(topic, payload); 

  // Agregar una linea para separar temp de humedad
  Serial.println();
  // Publica en el topic de humedad 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); // Cleans the payload 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_hum); // Adds the variable label 
  float h1 = dht1.readHumidity(); 
  Serial.print("  2. La humedad detectada por el sensor es de: ");
  Serial.println(h1); // Imprime la humedad en el monitor serial 
 
  // numero maximo 4 precision 2 y convierte el valor a string
  dtostrf(h1, 4, 2, str_hum); 
  sprintf(payload, "%s {\"value\": %s", payload, str_hum); // formatea el mensaje a publicar 
  sprintf(payload, "%s } }", payload); // cierra el mensaje 
  Serial.println("  Enviando Humedad al virtual device en ubidots via MQTT....");  
  client.publish(topic, payload); 
   
  client.loop();
  
  //Esperare 10 seg a leer de nuevo los sensores
  Serial.println();
  Serial.println("Esperaré 12 seg para leer nuevamente los sensores.....");
  Serial.println();
  Serial.println(".......................................................");
  delay(12000); // 12 segundos entre publicaciones en ubidots 
} 
 
 