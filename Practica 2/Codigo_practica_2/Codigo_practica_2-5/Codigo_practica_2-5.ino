#include <WiFi.h>
#include <WiFiClient.h>
//Variables asignadas para la red de internet
const char* ssid="HUAWEI Y8p";
const char* password="218124505Ara";

WiFiServer server(80);
// Establecer el Pin asignado al led
#define pinLed 15

String estado="";

//configuracion de conexion a internet y arranque del servidor
void setup(){
  Serial.begin(9600);
  pinMode(pinLed,OUTPUT);
  digitalWrite(pinLed,LOW);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conexion a internet establecida");
  Serial.print("Direccion IP Asignada: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("Servidor Web Iniciado");
}

void loop(){
  WiFiClient client=server.available();
  if(!client){ return;}
  Serial.print("Nuevo Cliente: ");
  Serial.println(client.remoteIP());
  while(!client.available()){ delay(100);}

  //Seccion de lectura de las acciones del cliente
  String req = client.readStringUntil('\r');
  Serial.println(req);
  if(req.indexOf("on2")!=-1){
    digitalWrite(pinLed , HIGH);
    estado="encendido";
  }
  if (req.indexOf("off2") != -1){
    digitalWrite(pinLed , LOW);
    estado = "Apagado";
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><meta charset=utf-8></head>");
  client.println("<body><center><font face='Arial'>");
  client.println("<h1>Servidor web con ESP32.</h1>");
  client.println("<h1>Rendon Alonso Luis Alberto</h1>");
  client.println("<h2><font color='#009900'>Webserver 1.0</font></h2>");
  client.println("<h3>Práctica boton</h3>");
  client.println("<br><br>");
  client.println("<a href='on2'><button>Enciende LED</button></a>");
  client.println("<a href='off2'><button>Apaga LED</button></a>");
  client.println("<br><br>");
  client.println(estado);
  client.println("</font></center></body></html>");
  Serial.print("Cliente desconectado: ");
  Serial.println(client.remoteIP());
  client.flush();
  client.stop();

}

