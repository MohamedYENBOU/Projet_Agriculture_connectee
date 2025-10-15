#include "DFRobot_BME280.h"
#include "Wire.h"

// Création d’un objet I2C personnalisé avec SDA=42, SCL=41
TwoWire myWire = TwoWire(0);
typedef DFRobot_BME280_IIC BME;
BME bme(&myWire, 0x77); // Adresse I2C du BME280

#define SEA_LEVEL_PRESSURE 1015.0f

// LoRa UART et broches de mode
#define LORA_SERIAL Serial2
#define LORA_RX 18
#define LORA_TX 17
#define M0 47
#define M1 48

void printLastOperateStatus(BME::eStatus_t eStatus) {
  switch (eStatus) {
    case BME::eStatusOK: Serial.println("everything ok"); break;
    case BME::eStatusErr: Serial.println("unknown error"); break;
    case BME::eStatusErrDeviceNotDetected: Serial.println("device not detected"); break;
    case BME::eStatusErrParameter: Serial.println("parameter error"); break;
    default: Serial.println("unknown status"); break;
  }
}

void setLoRaNormalMode() {
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  delay(100);
  Serial.println("LoRa Normal Mode selected");
}

void setup() {
  Serial.begin(115200);
  LORA_SERIAL.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX); // UART LoRa

  // Initialisation I2C pour le capteur
  myWire.begin(42, 41);
  bme.reset();

  // Initialisation des broches M0 et M1
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  setLoRaNormalMode();

  // Vérification du capteur
  while (bme.begin() != BME::eStatusOK) {
    Serial.println("BME begin failed");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }

  Serial.println("BME begin success");
  delay(100);
}

void loop() {
  float temp = bme.getTemperature();
  uint32_t press = bme.getPressure();
  float alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
  float humi = bme.getHumidity();

  // Construction de la chaîne à envoyer, chaque donnée sur une ligne différente
  String data = "";
  data += "TEMP:" + String(temp, 2) + "\n";
  data += "HUM:" + String(humi, 2) + "\n";
  data += "PRESS:" + String(press) + "\n";
  data += "ALTI:" + String(alti, 2) + "\n";

  // Affichage sur le moniteur série
  Serial.println("======== Envoi LoRa ========");
  Serial.print(data);
  Serial.println("============================");

  // Envoi via LoRa (UART)
  LORA_SERIAL.print(data);

  delay(1000); // Pause avant la prochaine lecture
}
