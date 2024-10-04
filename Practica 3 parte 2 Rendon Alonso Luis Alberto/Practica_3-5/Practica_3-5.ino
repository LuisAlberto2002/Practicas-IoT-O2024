// Práctica hecha por Guadalupe Torres - clase IoT
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  15     // Pin para el reset del RC522
#define SS_PIN   21     // Pin para el SS (SDA) del RC522
//String code_tarjeta = "";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crear el objeto para el RC522

void setup() {
  Serial.begin(9600);    // Iniciar la comunicación serial
  SPI.begin();           // Iniciar el bus SPI para la comunicación con el RC522
  mfrc522.PCD_Init();    // Inicializar el lector RFID
  Serial.println("Sistema iniciado. Esperando una tarjeta RFID...");
}

void loop() {
  delay(500); // Pausa para evitar lecturas continuas
  
  // Revisar si hay una tarjeta RFID presente
  if (mfrc522.PICC_IsNewCardPresent()) {
    delay(50); // Pausa adicional para asegurar la lectura
    
    // Leer la tarjeta detectada
    if (mfrc522.PICC_ReadCardSerial()) {
      
      // Agregar un salto de línea antes de la primera línea de asteriscos
      Serial.println();
      
      // Imprimir una línea de asteriscos antes de la lectura
      Serial.println("***************************");
      
      // Imprimir el mensaje de tarjeta detectada
      Serial.println("Tarjeta leída. El número de UID (identificador) es:");

      // Verificar si el UID tiene un tamaño válido antes de imprimir
      if (mfrc522.uid.size > 0) {
        // Imprimir el UID de la tarjeta en formato hexadecimal
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(mfrc522.uid.uidByte[i], HEX);
          //code_tarjeta = code_tarjeta + Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();  // Nueva línea después del UID
        //Serial.println(code_tarjeta);
        //code_tarjeta = " ";
      } else {
        Serial.println("No se pudo leer el UID.");
      }

      // Imprimir una línea de asteriscos después de la lectura
      Serial.println("***************************");
      
      // Mensaje informando que se espera 5 segundos antes de continuar
      Serial.println("Esperando 5 segundos para leer la siguiente tarjeta...");
    
      // Agregar un salto de línea después del mensaje de espera
      Serial.println();
    
      // Detener la comunicación con la tarjeta actual
      mfrc522.PICC_HaltA();

      // Esperar 5 segundos antes de permitir la lectura de otra tarjeta
      delay(5000);
    }
  } else {
    // Mensaje si no hay tarjeta detectada
    Serial.println("No se detecta ninguna tarjeta... esperando una.");
  }
}
