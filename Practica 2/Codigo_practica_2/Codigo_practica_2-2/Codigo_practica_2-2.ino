

#include <WiFi.h>



WiFiServer server(80);
WiFiClient client;



#ifdef __cplusplus
extern "C"{
  #endif
  uint8_t temprature_sens_read();
  #ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

void setup(){
  Serial.begin(9600);

  
  WiFi.disconnect();
  delay(3000);
  Serial.println("Iniciando Conexion a internet");
  WiFi.begin("HUAWEI Y8p","218124505Ara");
  while((!(WiFi.status()==WL_CONNECTED))){
    Serial.println("....");
    delay(300);
  }
  Serial.println("Conexion establecida a la red, IP Asignada: ");
  Serial.println((WiFi.localIP()));
  server.begin();
  Serial.println("Servidor Iniciado");
}

void loop(){
  client = server.available();
  if(!client){return;}
  while(!client.available()){ delay(100);}
  client.flush();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
    client.println("<title>""MiESP32""</title>");
  client.println("</head>");
  client.println("<body>");
    client.println("<h1 style=""color:#3366ff"">");
    client.println("</h1>");
    client.println("<span style=""color:#33cc00;font-size:14px"">");
    client.println("Bienvenidos a mi pagina web");
    client.println("</span>");
    client.println("<br>");
    client.println("<span style=""color:#cc6600;font-size:14px"">");
    client.println("Esta pagina web ha estado activa por");
    client.println("</span>");
    client.println("<span style=""color:#009900;font-size:20px"">");
    client.println((millis()/1000));
    client.println("</span>");
    client.println("<span style=""color:#cc6600;font-size:14px"">");
    client.println("segundos");
    client.println("</span>");
    client.println("<br>");
    client.println("<span style=""color:#ffcc33;font-size:14px"">");
    client.println("La temperatura interna del ES32 es de:");
    client.println("</span>");
    client.println("<span style=""color:#000000;font-size:14px"">");
    client.println(((temprature_sens_read() - 32 ) / 1.8));
    client.println("</span>");
    client.println("<span style=""color:#ffcc33;font-size:14px"">");
    client.println("grados");
    client.println("</span>");
  client.println("</body>");
  client.println("</html>");
  delay(1);
}