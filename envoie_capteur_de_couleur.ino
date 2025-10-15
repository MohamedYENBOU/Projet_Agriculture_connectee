// === Capteur couleur ===
#define S0 3
#define S1 2
#define S2 43
#define S3 44
#define sensorOut 38

int frequency = 0;
int red = 0, green = 0, blue = 0;

// === LoRa - Définition UART et broches de mode ===
#define mySerial Serial2
const int M0 = 47;
const int M1 = 48;

void setup() {
  // Initialisation capteur couleur
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);

  Serial.begin(115200);  // Pour affichage local
  mySerial.begin(9600, SERIAL_8N1, 18, 17); // Communication avec module LoRa

  // Mode normal LoRa
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  Serial.println("LoRa prêt. Début lecture capteur...");
  delay(500);
}

void loop() {
  // Lecture Rouge
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red = pulseIn(sensorOut, LOW);
  delay(100);

  // Lecture Vert
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green = pulseIn(sensorOut, LOW);
  delay(100);

  // Lecture Bleu
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue = pulseIn(sensorOut, LOW);
  delay(100);

  // Affichage local
  Serial.print("Red = "); Serial.print(red); Serial.print(" | ");
  Serial.print("Green = "); Serial.print(green); Serial.print(" | ");
  Serial.print("Blue = "); Serial.println(blue);

  // Envoi LoRa
  mySerial.print("Red: "); mySerial.print(red);
  mySerial.print(" Green: "); mySerial.print(green);
  mySerial.print(" Blue: "); mySerial.println(blue);

  delay(1000); // Attente entre deux mesures
}
