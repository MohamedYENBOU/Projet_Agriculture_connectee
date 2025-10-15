#include "DFRobot_BME280.h"
#include "Wire.h"

// ➤ Création d’un objet I2C personnalisé avec tes broches
TwoWire myWire = TwoWire(0); // I2C bus 0 (ESP32 a 2 bus I2C disponibles)

// ➤ Déclaration du BME avec l’objet personnalisé et l'adresse I2C
typedef DFRobot_BME280_IIC BME;
BME bme(&myWire, 0x77); // Adresse I2C par défaut du SEN0236 (à confirmer)

#define SEA_LEVEL_PRESSURE 1015.0f

void printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch (eStatus) {
    case BME::eStatusOK: Serial.println("everything ok"); break;
    case BME::eStatusErr: Serial.println("unknown error"); break;
    case BME::eStatusErrDeviceNotDetected: Serial.println("device not detected"); break;
    case BME::eStatusErrParameter: Serial.println("parameter error"); break;
    default: Serial.println("unknown status"); break;
  }
}

void setup()
{
  Serial.begin(115200);

  // ➤ Initialisation I2C sur tes broches SDA=42, SCL=41
  myWire.begin(9, 42); 

  Serial.println("BME280 read data test");
  bme.reset();

  // ➤ Boucle de vérif
  while (bme.begin() != BME::eStatusOK) {
    Serial.println("BME begin failed");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }

  Serial.println("BME begin success");
  delay(100);
}

void loop()
{
  float temp = bme.getTemperature();
  uint32_t press = bme.getPressure();
  float alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
  float humi = bme.getHumidity();

  Serial.println();
  Serial.println("======== Start Print ========");
  Serial.print("Température (°C) : "); Serial.println(temp);
  Serial.print("Pression (Pa)    : "); Serial.println(press);
  Serial.print("Altitude (m)     : "); Serial.println(alti);
  Serial.print("Humidité (%)     : "); Serial.println(humi);
  Serial.println("========  End Print  ========");

  delay(1000);
}
