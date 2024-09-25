// Medidor de distancia a OLED hecho por Guadalupe Torres
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 64 // Altura de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int trigPin = 19;   // Pin de Trigger del sensor ultrasónico
const int echoPin = 18;  // Pin de Echo del sensor ultrasónico
#define SOUND_SPEED 0.034 // Velocidad del sonido en cm/us

long duration;
float distanceCm;

void setup() {
  Serial.begin(115200); // Inicia la comunicación serial
  pinMode(trigPin, OUTPUT); // Establece el pin de Trigger como salida
  pinMode(echoPin, INPUT);  // Establece el pin de Echo como entrada

  // Inicia el display OLED, verifica la conexión
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Bucle infinito si falla la inicialización
  }
  display.clearDisplay(); // Limpia la pantalla
  display.setTextSize(1); // Establece el tamaño del texto
  display.setTextColor(WHITE); // Establece el color del texto
  display.setCursor(0,0); // Posiciona el cursor
  display.println("Iniciando sensor..."); // Muestra mensaje inicial
  display.display(); // Actualiza la pantalla
  delay(2000); // Retardo inicial
}

void loop() {
  digitalWrite(trigPin, LOW); // Asegura que el Trigger está desactivado
  delayMicroseconds(2); // Breve pausa
  digitalWrite(trigPin, HIGH); // Envía un pulso
  delayMicroseconds(10); // Duración del pulso
  digitalWrite(trigPin, LOW); // Finaliza el pulso

  duration = pulseIn(echoPin, HIGH); // Mide el tiempo del eco
  distanceCm = duration * SOUND_SPEED / 2; // Calcula la distancia

  // Muestra la distancia en el OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Distancia: ");
  display.print(distanceCm);
  display.println(" cm");
  display.display();

  // También envía la distancia al Monitor Serial
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  delay(1000); // Espera antes de la próxima medición
}