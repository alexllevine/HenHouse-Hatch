#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10   // Slave Select pin for RC522
#define RST_PIN 9   // Reset pin for RC522
#define BUTTON_PIN 2   // Push button pin
#define PHOTOCELL_PIN A0   // Photocell pin connected to A0

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

bool darkOut = false;   // Track brightness state
bool motorActive = false;
int inputState = 0;

void setup() {
  Serial.begin(9600);   // Start Serial Communication
  SPI.begin();          // Initialize SPI bus
  rfid.PCD_Init();      // Initialize the RFID reader

  pinMode(BUTTON_PIN, INPUT);
  pinMode(PHOTOCELL_PIN, INPUT);

  Serial.println("Scan an RFID tag or press a button....");
}

void loop() {
  static byte lastCardUID[10];  // Store last scanned UID
  static byte lastCardSize = 0; // Store size of last UID

  // Read ambient light
  darkOut = (analogRead(PHOTOCELL_PIN) <= 50);

  bool newRFID = false;

  // Check if a new RFID card is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Check if the scanned UID is different from the last one
    if (memcmp(lastCardUID, rfid.uid.uidByte, rfid.uid.size) != 0) {
      memcpy(lastCardUID, rfid.uid.uidByte, rfid.uid.size);
      lastCardSize = rfid.uid.size;

      Serial.print("RFID Scanned! Card UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");  // Leading zero
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
      delay(2000);

      motorActive = true;
      inputState = 1;
      newRFID = true;
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // Check if button is pressed
  if (digitalRead(BUTTON_PIN) == HIGH && !newRFID) {
    Serial.println("Button Pressed");
    motorActive = true;
    inputState = 1;
    delay(500);
  }

  if (!darkOut) {
    if (motorActive) {
      Serial.println("Door is opening...");
      delay(2000);
      
      // ADD MOTOR OPEN CODE HERE

      motorActive = false;
    } else if (inputState == 1) {
      Serial.println("Door is closing...");
      delay(2000);
      
      // ADD MOTOR CLOSE CODE HERE

      inputState = 0;
    }
  } else {
    Serial.println("Intruder alert: It's dark outside. Any button press is invalid.");
    delay(2000);
    inputState = 0;

    // Ensure door remains closed
  }
}
