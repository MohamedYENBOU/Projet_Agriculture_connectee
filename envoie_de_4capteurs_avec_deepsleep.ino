#include <Wire.h>
#include "DFRobot_BME280.h"
#include "esp_sleep.h"

// ---------- LoRa UART ----------
#define mySerial Serial2
const int M0 = 47;
const int M1 = 48;

// ---------- BH1750 (luminosité) ----------
#define BH1750_ADDR 0x23
#define SDA_PIN 21
#define SCL_PIN 16
byte buff[2];

// ---------- BME280 (température, pression, humidité) ----------
typedef DFRobot_BME280_IIC BME;
BME bme(&Wire, 0x77);
#define SEA_LEVEL_PRESSURE 1015.0f

// ---------- Capteur humidité du sol ----------
#define SOIL_SENSOR_PIN 19

// ---------- TCS3200 (capteur couleur) ----------
#define S0 3
#define S1 2
#define S2 43
#define S3 44
#define sensorOut 38

// Durées
const uint64_t SLEEP_TIME_US = 30ULL * 1000000ULL; // 30 secondes en microsecondes
const uint32_t AWAKE_TIME_MS = 30UL * 1000UL;      // 30 secondes en millisecondes

unsigned long awakeStartTime = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 18, 17); // LoRa UART
  pinMode(M0, OUTPUT); pinMode(M1, OUTPUT);
  normalMode();

  Wire.begin(SDA_PIN, SCL_PIN);
  bme.reset();
  while (bme.begin() != BME::eStatusOK) {
    delay(2000);
  }

  // Capteur TCS3200
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT); pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH); digitalWrite(S1, LOW);

  // Capteur humidité du sol
  pinMode(SOIL_SENSOR_PIN, INPUT);

  // Affiche la cause du réveil
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  Serial.print("🔁 Réveil cause : ");
  Serial.println(cause);

  // Début de la période éveillée
  awakeStartTime = millis();
}

void loop() {
  unsigned long elapsedAwake = millis() - awakeStartTime;

  if (elapsedAwake < AWAKE_TIME_MS) {
    // Lecture des capteurs
    uint16_t luminosite = lireLuminosite();
    float temp = bme.getTemperature();
    float hum = bme.getHumidity();
    float press = bme.getPressure();
    float alt = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    int solRaw = analogRead(SOIL_SENSOR_PIN);
    String solEtat = interpretationSol(solRaw);

    // RGB via TCS3200
    digitalWrite(S2, LOW); digitalWrite(S3, LOW); int rouge = pulseIn(sensorOut, LOW); delay(100);
    digitalWrite(S2, HIGH); digitalWrite(S3, HIGH); int vert = pulseIn(sensorOut, LOW); delay(100);
    digitalWrite(S2, LOW); digitalWrite(S3, HIGH); int bleu = pulseIn(sensorOut, LOW); delay(100);

    // Message à envoyer
    String message = " Temp : " + String(temp, 1) + " C\n"
                   + " Hum : " + String(hum, 1) + " %\n"
                   + " Pression : " + String(press, 1) + " Pa\n"
                   + " Alt : " + String(alt, 1) + " m\n"
                   + " Lumi : " + String(luminosite) + " lx\n"
                   + " Sol : " + solEtat + " (" + String(solRaw) + ")\n"
                   + " RGB = " + String(rouge) + ", " + String(vert) + ", " + String(bleu) + "\n";

    Serial.println("📡 Envoi par LoRa :\n" + message);
    mySerial.print(message);

    delay(2000);

  } else {
    // Temps écoulé → passage en deep sleep
    String sleepMsg = "esp en mode veille";
    Serial.println(sleepMsg);
    mySerial.println(sleepMsg);

    Serial.flush();     // attendre l'envoi sur USB
    mySerial.flush();   // attendre l'envoi sur LoRa
    delay(100);         // sécurité

    esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
    esp_deep_sleep_start();
  }
}

void normalMode() {
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
}

// ---------- Fonctions BH1750 ----------
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
  Wire.write(0x10); // Mode haute résolution
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

// ---------- Lecture humidité sol ----------
String interpretationSol(int value) {
  if (value > 3000) return "Sec";
  else if (value > 2000) return "Humide";
  else return "Mouillé";
}
