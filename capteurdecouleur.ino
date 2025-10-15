#define S0 3
#define S1 2
#define S2 43
#define S3 44
#define sensorOut 38

int frequency=0;

void setup() 
{
pinMode(S0, OUTPUT);
pinMode(S1, OUTPUT);
pinMode(S2, OUTPUT);
pinMode(S3, OUTPUT);
pinMode(sensorOut, INPUT);

digitalWrite(S0, HIGH);
digitalWrite(S1, LOW);

Serial.begin(115200);
}

void loop()
{

// Lecture du filtre rouge
digitalWrite(S2, LOW);
digitalWrite(S3, LOW);
// La frequence
frequency=pulseIn (sensorOut, LOW);
// Affichage de la couleur
Serial.print("Red=  ");
Serial.print(frequency);
Serial.print(" ");
delay(500);


// Lecture du filtre vert
digitalWrite(S2, HIGH);
digitalWrite(S3, HIGH);
// Lecture de la frequence
frequency=pulseIn (sensorOut, LOW);
// Affichage de la valeur
Serial.print("Green=  ");
Serial.print(frequency);
Serial.print(" ");
delay(500);


// Lecture du filtre Bleu
digitalWrite(S2, LOW);
digitalWrite(S3, HIGH);
// Lecture de la frequence
frequency=pulseIn (sensorOut, LOW);
// Affichage de la valeur
Serial.print("Blue=  ");
Serial.print(frequency);
Serial.print(" \n");
delay(100);



}





