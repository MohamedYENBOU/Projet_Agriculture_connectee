// Broche analogique à laquelle le capteur est connecté
const int soilSensorPin = 19;

void setup() {
  Serial.begin(115200);  // Démarrer la communication série à 115200 bauds
  delay(1000);
  Serial.println("Lecture du capteur d'humidité du sol Grove MKR-SOIL");
}

void loop() {
  int sensorValue = analogRead(soilSensorPin);  // Lire la valeur analogique du capteur
  // La valeur brute est entre 0 (sec) et environ 4095 (très humide) sur ESP32 12 bits ADC

  // Optionnel : convertir en pourcentage d'humidité
  // Ici, il faut calibrer les valeurs pour ton capteur et ton sol.
  // Exemple :
  int moisturePercent = map(sensorValue, 4095, 0, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Valeur brute : ");
  Serial.print(sensorValue);
  Serial.print(" - Humidité du sol approximative : ");
  Serial.print(moisturePercent);
  Serial.println("%");

  delay(1000);  // Attendre 1 seconde avant la prochaine lecture
}
