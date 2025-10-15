#include <Wire.h>
#include "DFRobot_BME280.h"

// ---------- LoRa UART ----------
#define mySerial Serial2
const int M0 = 47;
const int M1 = 48;

// ---------- BH1750 (capteur de lumière) ----------
#define BH1750_ADDR 0x23
#define SDA_PIN 21
#define SCL_PIN 16
byte buff[2];

// ---------- BME280 (capteur de température, humidité et pression) ----------
typedef DFRobot_BME280_IIC BME;
BME bme(&Wire, 0x77);
#define SEA_LEVEL_PRESSURE 1015.0f

// ---------- Humidité du sol ----------
#define SOIL_SENSOR_PIN 19

// ---------- TCS3200 (capteur de tempéarture) ----------
#define S0 3
#define S1 2
#define S2 43
#define S3 44
#define sensorOut 38
int frequency = 0;


// ---------------------------- setup()  --------------------------------------
void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 18, 17); // UART avec LoRa
  pinMode(M0, OUTPUT); pinMode(M1, OUTPUT);
  normalMode();

  Wire.begin(SDA_PIN, SCL_PIN);
  bme.reset();
  while (bme.begin() != BME::eStatusOK) {
    delay(2000);
  }

  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT); pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH); digitalWrite(S1, LOW);
  pinMode(SOIL_SENSOR_PIN, INPUT);
}


// ---------------------------- Loop()  --------------------------------------
void loop() {
  uint16_t luminosite = lireLuminosite();
  float temp = bme.getTemperature();
  float hum = bme.getHumidity();
  float press = bme.getPressure(); // pression en hPa
  float alt = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
  int solRaw = analogRead(SOIL_SENSOR_PIN);
  String solEtat = interpretationSol(solRaw);

  // Lecture couleur
  digitalWrite(S2, LOW); digitalWrite(S3, LOW); int rouge = pulseIn(sensorOut, LOW); delay(100);
  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH); int vert = pulseIn(sensorOut, LOW); delay(100);
  digitalWrite(S2, LOW); digitalWrite(S3, HIGH); int bleu = pulseIn(sensorOut, LOW); delay(100);

  // Construction du message
  String message = " Temp : " + String(temp, 1) + " C \n  "
                 + " Hum : " + String(hum, 1) + " % \n "
                 + " Pression : " + String(press, 1) + " Pa \n "
                 + " Alt : " + String(alt, 1) + " m \n "
                 + " Lumi : " + String(luminosite) + " lx \n "
                 + " Sol_etat : " + solEtat + " ( " + String(solRaw) + " ) \n "
                 + " RGB = " + String(rouge) + " , " + String(vert) + "," + String(bleu) + "\n";

  Serial.println("🔄 Envoi par LoRa:\n" + message);
  mySerial.print(message);

  delay(2000);
}

void normalMode() {
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
}

// Fonctions BH1750
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
  Wire.write(0x10); // Haute résolution
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

// Humidité sol
String interpretationSol(int value) {
  if (value > 3000) return "Sec";
  else if (value > 2000) return "Humide";
  else return "Mouillé";
}
