/*
  Rendon Alonso Luis Alberto Ing. en ciberseguridad

  Descripcion del funcionamiento de la practica: Esta practica consiste en utilizar los codigos de tarjeta obtenidos en la practica anterior para determinar los accesos comparando 
  los codigos de tarjetas que registremos.
  Empleando el lector RFID y los codigos de las tarjetas RFID podemos terminar en 2 escenarios, en caso de que el codigo registrado como valido y el codigo de la tarjeta ingresada coincida va a encenderde
  el led verde, caso contrario se encendera el led rojo.
  Los codigos validos son de mi identificacion de ITESO y la tarjeta RFID del kit del sensor.



*/

#include <SPI.h>               // Incluye la biblioteca SPI para la comunicación
#include <MFRC522.h>          // Incluye la biblioteca para el lector RFID MFRC522

#define RST_PIN  15           
#define SS_PIN  21            // Pin 21 para el SS (SDA) del RC522
#define greenLed 32           // Pin para el LED verde (acceso concedido)
#define redLed 33             // Pin para el LED rojo (acceso denegado)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crea un objeto para el lector RC522

void setup() {
  Serial.begin(9600);          // Inicia la comunicación serial a 9600 baudios
  Serial.println("Inicializando el sistema...");
  SPI.begin();                 // Inicia el bus SPI
  mfrc522.PCD_Init();         // Inicializa el lector MFRC522
  pinMode(greenLed, OUTPUT);  // Configura el pin del LED verde como salida
  pinMode(redLed, OUTPUT);    // Configura el pin del LED rojo como salida
  Serial.println("Control de acceso:");
}

byte ActualUID[4];             // Almacena el código UID del Tag leído
// abajo reemplaza por el UID de alguna de tus tarjetas que quieras que tenga acceso
byte Usuario1[4] = {0x1D, 0xCC, 0xFB, 0xB8}; // UID tarjeta CON acceso (reemplaza por tu UID)
byte Usuario2[4] = {0x43, 0x32, 0xFB, 0x0F}; // UID de otra tarjeta que quieras que tenga acceso

void loop() {
  // Revisa si hay nuevas tarjetas presentes
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Selecciona una tarjeta
    if (mfrc522.PICC_ReadCardSerial()) {
      // Envía serialmente su UID
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX); // Imprime el UID en formato hexadecimal
        ActualUID[i] = mfrc522.uid.uidByte[i]; // Almacena el UID leído
      }
      Serial.print("     ");
      
      // Compara el UID leído con los usuarios permitidos
      if (compareArray(ActualUID, Usuario1)) {
        Serial.println("Acceso concedido...");
        digitalWrite(greenLed, HIGH); // Enciende el LED verde
        delay(1500);                   // Espera 1.5 segundos
        digitalWrite(greenLed, LOW);  // Apaga el LED verde
      } else if (compareArray(ActualUID, Usuario2)) {
        Serial.println("Acceso concedido...");
        digitalWrite(greenLed, HIGH); // Enciende el LED verde
        delay(1500);                   // Espera 1.5 segundos
        digitalWrite(greenLed, LOW);  // Apaga el LED verde
      } else {
        Serial.println("Acceso denegado...");
        digitalWrite(redLed, HIGH);    // Enciende el LED rojo
        delay(1500);                   // Espera 1.5 segundos
        digitalWrite(redLed, LOW);     // Apaga el LED rojo
      }
      
      // Finaliza la lectura de la tarjeta actual
      mfrc522.PICC_HaltA();
    }
  }
}

// Función para comparar dos vectores (UIDs)
boolean compareArray(byte array1[], byte array2[]) {
  // Compara cada elemento de los arrays
  if (array1[0] != array2[0]) return false;
  if (array1[1] != array2[1]) return false;
  if (array1[2] != array2[2]) return false;
  if (array1[3] != array2[3]) return false;
  return true; // Retorna true si los arrays son iguales
}