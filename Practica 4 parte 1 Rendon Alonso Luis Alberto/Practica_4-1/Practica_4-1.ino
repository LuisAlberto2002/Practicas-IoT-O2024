// Recepción de temperatura vía Bluetooth, visualización en OLED y envío de alertas
// Código clase IoT - Guadalupe Torres

// Este ESP32 actúa como receptor de temperatura enviada por otro ESP32 (emisor).
// Además de mostrar la temperatura en una pantalla OLED y activar un LED de alerta,
// envía una señal al emisor para que su LED también parpadee si la temperatura excede el umbral.
// De este modo, hay comunicación en dos direcciones: el emisor envía la temperatura y el receptor
// responde con una señal de alerta que hace que el LED del emisor parpadee.
// NO olvides poner atención especial y cambiar los PARAMETROS CONFIGURABLES que abajo se indican
// ======== INICIO DEL CÓDIGO ========
// Inclusión de bibliotecas necesarias
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==== PARÁMETROS CONFIGURABLES ====
// Nombre del dispositivo BLE del emisor
// Nota: Este nombre debe coincidir exactamente con el nombre configurado en el emisor.
// Si no coinciden, el receptor no podrá encontrar y conectarse al emisor.
#define TARGET_DEVICE_NAME "Axel"  // Cambiar según el nombre del emisor configurado

// Umbral de temperatura que activará la alerta
// Nota: Si la temperatura recibida supera este valor, el receptor activará el LED de alerta
// y enviará una señal de alerta al emisor.
const float TEMP_THRESHOLD = 27.9;  // Cambiar según el umbral deseado
// ===============================================

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin para el LED de alerta
#define LED_PIN 32

// UUIDs para la comunicación BLE (no modificar)
static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID CHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
static BLEUUID RESPONSE_CHARACTERISTIC_UUID("47a9744c-2d2c-4b8f-ba98-7a13c7c1234e");

// Variables globales para el manejo del estado BLE y datos
bool deviceConnected = false, doConnect = false, doScan = false, alertSent = false, needToSendAlert = false;
float lastTemperature = 0.0;
static BLERemoteCharacteristic *pRemoteCharacteristic, *pResponseCharacteristic;
static BLEAdvertisedDevice* myDevice;

// Variables para el control de tiempos
unsigned long lastUpdateTime = 0, lastReconnectAttempt = 0;
const unsigned long WATCHDOG_TIMEOUT = 30000, RECONNECT_INTERVAL = 5000;

// Clase para manejar eventos de conexión y desconexión BLE
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    deviceConnected = true;
    Serial.println("Conectado al ESP32 Sensor");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Conectado a Sensor");
    display.display();
  }

  void onDisconnect(BLEClient* pclient) {
    deviceConnected = false;
    Serial.println("Desconectado del ESP32 Sensor");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Desconectado");
    display.display();
  }
};

// Función callback para recibir notificaciones de temperatura
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Serial.println("Notificación recibida");
  lastUpdateTime = millis();
  if (length > 0) {
    // Convertir los datos recibidos a una cadena y luego a float
    char tempStr[8];
    memcpy(tempStr, pData, length < sizeof(tempStr) ? length : sizeof(tempStr) - 1);
    tempStr[length < sizeof(tempStr) ? length : sizeof(tempStr) - 1] = '\0';
    
    lastTemperature = atof(tempStr);
    Serial.printf("Temperatura recibida: %.2f\n", lastTemperature);
    
    updateDisplay();
  
    // Verificar si la temperatura supera el umbral
    if (lastTemperature > TEMP_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED encendido - Temperatura alta");
      //Enviar alerta inmediatamente
      needToSendAlert = true;
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED apagado - Temperatura normal");
      alertSent = false;
      needToSendAlert = false;
    }
  } else {
    Serial.println("Notificación recibida con longitud 0");
  }
}

// Función para actualizar la pantalla OLED con la temperatura actual
void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Temperatura:");
  display.setTextSize(2);
  display.setCursor(0,16);
  display.printf("%.2f C", lastTemperature);
  if (lastTemperature > TEMP_THRESHOLD) {
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print("ALERTA: Temp. Alta");
  }
  display.display();
  Serial.println("Display actualizado");
}

// Clase para manejar el escaneo de dispositivos BLE
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Dispositivo BLE encontrado: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.getName() == TARGET_DEVICE_NAME) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;
      Serial.printf("Dispositivo %s encontrado\n", TARGET_DEVICE_NAME);
    }
  }
};

// Función para conectar al servidor BLE (ESP32 emisor)
bool connectToServer() {
  Serial.println("Conectando al servidor BLE...");
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  pClient->connect(myDevice);

  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find our service UUID");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
  pResponseCharacteristic = pRemoteService->getCharacteristic(RESPONSE_CHARACTERISTIC_UUID);
  if (pRemoteCharacteristic == nullptr || pResponseCharacteristic == nullptr) {
    Serial.println("Failed to find our characteristic UUID");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic->registerForNotify(notifyCallback);
  Serial.println("Conectado exitosamente al servidor BLE");
  return true;
}

// Función para enviar una señal de alerta al emisor.
// Cuando la temperatura del receptor supera el umbral, se envía "ALERTA ON" 
// al emisor para que su LED parpadee. 
void sendAlert() {
  if (pResponseCharacteristic != nullptr && needToSendAlert && !alertSent) {
    Serial.println("Intentando enviar alerta...");
    pResponseCharacteristic->writeValue("ALERT", 5);
    Serial.println("Alerta enviada al ESP32 A");
    alertSent = true;
    needToSendAlert = false;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Inicializar pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Buscando sensor...");
  display.display();

  // Inicializar BLE
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  Serial.printf("Buscando dispositivo: %s\n", TARGET_DEVICE_NAME);
}

void loop() {
  // Intento de conexión si se encontró el dispositivo
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Conexión exitosa, esperando datos...");
      doConnect = false;
    } else {
      Serial.println("Fallo en la conexión, reiniciando escaneo...");
      doScan = true;
    }
  }

  // Iniciar escaneo si es necesario
  if (doScan) {
    BLEDevice::getScan()->start(0);
    Serial.println("Iniciando escaneo...");
  }

  // Manejo de la conexión y reconexión
  if (deviceConnected) {
    sendAlert();
  } else if (millis() - lastReconnectAttempt > RECONNECT_INTERVAL) {
    lastReconnectAttempt = millis();
    if (connectToServer()) {
      Serial.println("Reconectado exitosamente");
    } else {
      doScan = true;
    }
  }

  // Verificación de inactividad
  if (millis() - lastUpdateTime > WATCHDOG_TIMEOUT) {
    Serial.println("No se han recibido actualizaciones. Reiniciando escaneo...");
    deviceConnected = false;
    doScan = true;
    lastUpdateTime = millis();
  }

  delay(100); // Pequeña pausa para evitar sobrecarga del CPU
}