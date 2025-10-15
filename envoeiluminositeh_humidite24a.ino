#include <Wire.h>

// 📡 LoRa UART config
#define mySerial Serial2
const int M0 = 47;
const int M1 = 48;

// 💡 Capteur BH1750 config
#define BH1750_ADDR 0x23
#define SDA_PIN 1
#define SCL_PIN 2
byte buff[2];

void setup() {
  Serial.begin(115200);  // Moniteur série
  mySerial.begin(9600, SERIAL_8N1, 18, 17); // TX=18, RX=17 pour LoRa

  Wire.begin(SDA_PIN, SCL_PIN);  // I2C pour le capteur

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  normalMode(); // Mode normal du module LoRa
  delay(500);
  
  Serial.println("Prêt à envoyer la luminosité par LoRa !");
}

void loop() {
  uint16_t luminosite = lireLuminosite();

  if (luminosite > 0) {
    // Affiche localement
    Serial.print("Luminosité : ");
    Serial.print(luminosite);
    Serial.println(" lx");

    // Prépare le message
    String message = "Luminosité : " + String(luminosite) + " lx\n";

    // Envoie via LoRa
    mySerial.print(message);
  } else {
    Serial.println("Erreur lecture capteur BH1750");
  }

  delay(2000);
}

void normalMode() {
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  Serial.println("Mode LoRa normal activé");
}

uint16_t lireLuminosite() {
  BH1750_Init(BH1750_ADDR);
  delay(200);
  int val = 0;

  if (BH1750_Read(BH1750_ADDR) == 2) {
    val = ((buff[0] << 8) | buff[1]) / 1.2;
  }

  return val;
}

void BH1750_Init(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10); // Mode continu haute résolution
  Wire.endTransmission();
}

int BH1750_Read(int address) {
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) {
    buff[i++] = Wire.read();
  }
  Wire.endTransmission();
  return i;
}
