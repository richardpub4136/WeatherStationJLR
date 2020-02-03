
// ---------------------------------------------------------------------------------------------
// Début Variables et Libraries
// ---------------------------------------------------------------------------------------------

#include <ArduinoJson.h>
#include <Ethernet2.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS2450.h>
#include <DS2423.h>
#include "RunningAverage.h"
 // DS3231
#include <RTClib.h>
#include <Wire.h>
#include "cactus_io_BME280_I2C.h" 

//permet afficher le titre de la version
// ---------------------------------------------------------------------------------------------
String titre = __FILE__;

//  Gestion du port spécifique à DS18B20 et à Dallas Temperature
// ---------------------------------------------------------------------------------------------

// Le câble de données est branché sur le port 6 pour DS18b20
#define ONE_WIRE_BUS 6

 // Configuration d'une instance oneWire pour communiquer avec tous les appareils OneWire
//(pas seulement les CI de température Maxim / Dallas)OneWire oneWire(ONE_WIRE_BUS);
OneWire oneWire(ONE_WIRE_BUS);

// On passe notre référence oneWire à Dallas Temperature. (Ici sur le port 6)
DallasTemperature sensors(&oneWire);

//  Gestion du port spécifique à DS2423 et à DS2450  (Ici sur le port 5)
// ---------------------------------------------------------------------------------------------

// On définit ici la broche d'E/S numérique Arduino à utiliser pour le réseau 1-Wire
// pour les DS2450 et DS2423
const uint8_t ONE_WIRE_PIN = 5;  // sur la broche 5 (une résistance de 4,7 K est nécessaire)

// Configuration d'une instance oneWire pour communiquer avec tous les appareils OneWire
//(pas seulement les CI de température Maxim / Dallas)OneWire ONEWIRE(ONE_WIRE_BUS);
// N'ayant pas réussi à solutionner la coexistance de DS18B20 avec les Autres, j'ai choisi
//de séparer le port du DS18B20.

// définit ici l'adresse 1-Wire du DS2450 quad A/D (lsb en premier)
uint8_t DS2450_address[] = { 0x20, 0x28, 0xFC, 0x08, 0x00, 0x00, 0x00, 0x63 };

// définit ici, l'adresse 1-Wire du compteur DS2423 
uint8_t DS2423_address[] = { 0x1D, 0x9B, 0x42, 0x05, 0x00, 0x00, 0x00, 0x61 };

OneWire ow(ONE_WIRE_PIN);
DS2450 ds2450(&ow, DS2450_address);
DS2423 ds2423(&ow, DS2423_address);

//  Variables pout la Vitesse du vent
// ---------------------------------------------------------------------------------------------

volatile float windspeed_kmh; // // Donne la vitesse de vent actuelle enregistrée en Km/H

long time;
int timeDifference;
int WindCount = 0;
volatile unsigned long WindCounter1;
volatile unsigned long WindCounter2;
volatile unsigned long windCounter=0;
volatile unsigned long TimeWindCounter1;
volatile unsigned long TimeWindCounter2;
volatile float   TimeWindCounter=0;
long wsTime=0;
volatile unsigned long valeurDS2423_Timestamp;
int WindDelayMilliSeconds = 0;
int RevsPerSecx100 = 0;
int windMILES_PER_HOUR = 0;
int windMETER_PER_SECOND = 0;
int windKMS_PER_HOUR = 0;
int windKNOTS = 0;
#define METER_PER_SECOND 1.096;
#define KMS_PER_HOUR 3.9477;
#define MILES_PER_HOUR 2.453;
#define KNOTS 2.130;

//  Variables pour la Force du vent
// ---------------------------------------------------------------------------------------------
String forceVent;    // valeur Force du vent
float minVent;   // Conserve la valeur de la force de vent minimale enregistrée
float maxVent;   // Conserve la valeur de la force de vent maximale enregistrée

//  Variables du Moyenne Glissante" pour le calcul du Vent Moyen
// --------------------------------------------------------------------------------------------- 
RunningAverage MoyGliss(20);  // objet "Moyenne Glissante" avec nombre d'échantillons pour le calcul
int compteur = 20;             // un compteur d'échantillons
float  moyenneVent = 0;            // la moyenne glissante des échantillons 
volatile float  echantillon ;
 
 //  Variables pout la Direction du vent
// ---------------------------------------------------------------------------------------------
#define VaneOffset 0  // définit le décalage de l'anémomètre par rapport au nord magnétique
                      // de 0 à 360
float  vaneValue;       // Valeur analogique brute de la girouette en volt
float  vaneDirection;   // traduction 0 - 360 direction
unsigned int calDirection;    // valeur convertie avec décalage appliqué
unsigned int lastDirValue;    // dernière valeur de direction enregistrée
String directionLettre;    // variable en lettre Type NW direction du vent

//  Variables du DS18B20
// --------------------------------------------------------------------------------------------- 
float currentTemp;    // Donne la valeur de la température actuelle
float minTemp =0;   // Conserve la valeur de température minimale enregistrée
float maxTemp =0;   // Conserve la valeur de température maximale enregistrée
float celsius;
float outdoorTemperature ;

//  Variables du RTC DS3231 pour le Maintient de l'Heure à partir de l'heure de compilation
// --------------------------------------------------------------------------------------------- 
RTC_DS3231 rtc;
char t[32];  //Vérifier si placé au bon endroit des paramètres

//  Variables pour l'Humidité
// --------------------------------------------------------------------------------------------- 
float outdoorHumidity =12 ;
    
//  Variables pour l'Altitude
// --------------------------------------------------------------------------------------------- 
float altitude ;
//  Variables pour la Pression Atmosphérique
// --------------------------------------------------------------------------------------------- 
float outdoorRelPressure = 1025 ;
    float WindGust ;

 //  Variables pour la Pluviométrie
// --------------------------------------------------------------------------------------------- 
   float rainCount ;
   float rainLastMinute ;
    float rainLastHeure ; 
    float rainLast24Heure ; 
    float IndexUv ;
    float Lux; 
     
//  Variables pour BME280: Pression Atmosphérique taux humdité, température
// --------------------------------------------------------------------------------------------- 
float indoorTemperature ;
    // Create BME280 object 
BME280_I2C bme; // I2C using address 0x77 
// or BME280_I2C bme(0x76); // I2C using address 0x76 
  
//  Variables Connexion Ethernet Serveur
// --------------------------------------------------------------------------------------------- 
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xEB, 0x73};
byte ip[] = { 192, 168, 1, 177 }; // Adresse locale à adapter en fonction de votre réseau
EthernetServer server(80);

// ---------------------------------------------------------------------------------------------
// Début void setup()
// ---------------------------------------------------------------------------------------------

void setup() {
  // Initialize serial port
  Serial.begin(115200);
    while (Serial.available())
    Serial.read(); // Vide Serial.print avant de commencer remove 1 character
    //  Démarrage du DS2423  (Ici sur le port 5)
// ---------------------------------------------------------------------------------------------
    ds2423.begin();
        //  Démarrage du DS2450  (Ici sur le port 5)
// ---------------------------------------------------------------------------------------------
    ds2450.begin();
  //DS18B20    Démarrage la bibliothèque
// ---------------------------------------------------------------------------------------------
  sensors.begin();
 //mini max temp; 
  minTemp = sensors.getTempCByIndex(0);
  minTemp =floor(10*minTemp +0.5)/10;
  maxTemp = minTemp;
  maxTemp =floor(10*maxTemp +0.5)/10;
  
  // Setup pour afficher le titre de la version
// -----------------------------------------
    
int debut;
int fin;
debut = titre.lastIndexOf("Arduino");
fin = titre.length()-4;
debut = debut +((fin-2)/2)-1;
titre = (titre.substring(debut,fin));
Serial.println(titre.substring(debut,fin));

// ---------------------------------------------------------------------------------------------
     
      echantillon = 0;
//  getTempDS18B20 ();
  //minTemp = currentTemp; 
  // Initialize serial port

  Serial.begin(115200);
 // Initialisation de l'objet "Moyenne Glissante"
  MoyGliss.clear();
    ds2423.begin(DS2423_COUNTER_A|DS2423_COUNTER_B);
    getWindDirection();
       // Donne la force du vent en Beaufort
    getWindStrength(windspeed_kmh);
      Wire.begin();
rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  Ethernet.begin(mac, ip);
  server.begin();
    DateTime now = rtc.now();
   Serial.println("Le serveur est pret.");
  Serial.print("Merci de vous connecter à http://");
  Serial.println(Ethernet.localIP());
  Serial.println(titre);
  Serial.println("\t");
    sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
    Serial.print(F("Date/Time: "));
    Serial.println(t);



// initialise BME280
// ---------------------------------------------------------------------------------------------
    if (!bme.begin()) { 
Serial.println("Could not find a valid BME280 sensor, check wiring!"); 
while (1); 
} 
bme.setTempCal(0);// Temp was reading high so subtract 1 degree 

 


}
// ---------------------------------------------------------------------------------------------
// Début void loop()
// ---------------------------------------------------------------------------------------------

void loop() {
 
        //  Lecture du DS18B20
// --------------------------------------------------------------------------------------------- 
  sensors.requestTemperatures(); // Envoye la commande pour obtenir les températures
      // appelle sensor.requestTemperatures () pour recevoir une température globale
      // demande à tous les appareils sur le bus
      // Nous utilisons la fonction ByIndex, et à titre d'exemple, nous obtenons
      //la température du premier capteur uniquement.
  float outdoorTemperature = sensors.getTempCByIndex(0);
  // Affichage avec 1 chiffre après la virgule
  outdoorTemperature =floor(10*outdoorTemperature +0.5)/10;
  
  // update min and max temp values
if(outdoorTemperature <= minTemp) {
minTemp = outdoorTemperature;
}
if(outdoorTemperature > maxTemp) {
maxTemp = outdoorTemperature;
}

   
  // Vérifie si la lecture a réussi
  if(outdoorTemperature != DEVICE_DISCONNECTED_C) 
  {
   //Serial.println("outdoorTemperature"); 
   //Serial.println(outdoorTemperature);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  
    Serial.read(); // Vide Serial.print avant de commencer remove 1 character
    

        //  Lecture du DS3133 donne l'heure et la date enregistrée
// ---------------------------------------------------------------------------------------------
  DateTime now = rtc.now();

//getTempDS18B20 ();
currentTemp = celsius;
  ds2423.update();
  if (ds2423.isError()) {
        Serial.println("Error reading counter");
    } else {
   TimeWindCounter1 = ds2423.getTimestamp();    
  
    WindCounter1 = ds2423.getCount(DS2423_COUNTER_B);        //Prenez le compteur pour comparer la prochaine fois
                                                             // Obtenez la valeur actuelle du compteur
        //  Lecture du DS2450 
// ---------------------------------------------------------------------------------------------
          // Donne la direction du vent en degré
        ds2450.update();
    if (ds2450.isError()) {
        Serial.println("Error reading from DS2450 device");
    } else {
        /*for (int channel = 0; channel < 4; channel++) {
            Serial.print("Channel ");
            Serial.print("A "/*char('A' + channel));
            Serial.print(" = ");
            Serial.println(ds2450.getVoltage(3), 3);*/
           
        }
    getWindDirection();
      // Donne la direction cardinale du vent
// ---------------------------------------------------------------------------------------------
    getHeading();
     vaneValue = ds2450.getVoltage(3),4;
// ---------------------------------------------------------------------------------------------
      // Donne la force du vent en Beaufort
// ---------------------------------------------------------------------------------------------
   getWindStrength(windspeed_kmh);
      // Lecture du capteur

 
      // Calcul la vitesse moyenne du vent     
// ---------------------------------------------------------------------------------------------
  volatile float  echantillon = windspeed_kmh;
  // Ajout de l'échantillon à l'objet "Moyenne Glissante"
  MoyGliss.addValue(echantillon);
  // Récupération de la moyenne glissante
   moyenneVent = MoyGliss.getAverage();
  // Affichage avec 1 chiffre après la virgule
     moyenneVent =floor(100*moyenneVent +0.05)/100;
  // Incrémentation du compteur
  compteur++;
  // Effacement de l'historique pour éviter les débordements
  if (compteur == 19)
  {
    compteur = 0;
    MoyGliss.clear();
  }
        // Calcul la vitesse du vent     
// ---------------------------------------------------------------------------------------------
    windCounter = WindCounter1 - WindCounter2;
    TimeWindCounter = TimeWindCounter1 - TimeWindCounter2;
    windspeed_kmh = windCounter * (2.25/(TimeWindCounter/1000))*1.60934;
   // Affichage avec 1 chiffre après la virgule
    windspeed_kmh =floor(100*windspeed_kmh +0.05)/100;
  
  if (WindCounter2 != 0){
    // régler les valeurs minimales et maximales de force du Vent
        
         if (windspeed_kmh > maxVent) {
             maxVent = windspeed_kmh;
        }
        
 
    } 
  WindCounter2 = WindCounter1;
  TimeWindCounter2 = TimeWindCounter1;
  
// Lecture pluviométrie
// ---------------------------------------------------------------------------------------------

rainCount = ds2423.getCount(DS2423_COUNTER_A); 
    //rainCount
    
    
    // Lecture de BME280
// ---------------------------------------------------------------------------------------------
bme.readSensor(); 

Serial.print(bme.getPressure_MB()); Serial.print(" mb\t"); // Pressure in millibars 
Serial.print(bme.getHumidity()); Serial.print(" %\t\t"); 
Serial.print(bme.getTemperature_C()); Serial.println(" *C\t");

outdoorHumidity = bme.getHumidity();
outdoorRelPressure = bme.getPressure_MB();
indoorTemperature = bme.getTemperature_C();  
Serial.print(indoorTemperature); Serial.println(" *C\t");

   }        

  // Attend une connexion entrante
  EthernetClient client = server.available();

  // Avons-nous un client?
  if (!client) return;

  //Serial.println(F("New client"));

  // Lisons la demande (nous ignorons le contenu de cet exemple)
  while (client.available()) client.read();

   // Allouer un JsonDocument temporaire
   // Utilisez arduinojson.org/v6/assistant pour calculer la capacité.
StaticJsonDocument<500> doc;

JsonArray Titre = doc.createNestedArray(titre);
  
JsonArray TempDS18B20 = doc.createNestedArray("Temperature extérieure = ");
  TempDS18B20.add(outdoorTemperature);  
  JsonArray Celsius = doc.createNestedArray(" Celsius, ");
  //TempDS18B20.add(currentTemp);
JsonArray TempMiniDS18B20 = doc.createNestedArray("Temperature Mini = ");
  TempMiniDS18B20.add(minTemp);
JsonArray TempMaxiDS18B20 = doc.createNestedArray("Temperature Maxi = ");
  TempMaxiDS18B20.add(maxTemp);
  //Vent Moyen sur 1Mn
  
JsonArray VentRafales3S = doc.createNestedArray("Rafales de vent en Km/h  3S");
  VentRafales3S.add(windspeed_kmh);
JsonArray VentMoyen1Mn = doc.createNestedArray("Vent Moyen sur 1Mn = ");
  VentMoyen1Mn.add( moyenneVent);
JsonArray MaximumVent = doc.createNestedArray("Rafales Maximum sur 24H en Km/h ");
  MaximumVent.add(maxVent);
JsonArray WinDir = doc.createNestedArray("Direction du Vent en Degré: ");
  WinDir.add(calDirection);
JsonArray WinDirLettre = doc.createNestedArray("Direction Cardinal Vent: ");
  WinDirLettre.add(directionLettre);  
JsonArray WindForce = doc.createNestedArray("Force du vent: ");
  WindForce.add(forceVent);
JsonArray RainLastMinute = doc.createNestedArray("Pluie en cours: ");
  RainLastMinute.add(rainCount);
JsonArray RainLastHeure = doc.createNestedArray("Pluie dernière heure: ");
  RainLastHeure.add(rainLastHeure);
JsonArray RainLast24Heure = doc.createNestedArray("Pluie dernière 24H: ");
  RainLast24Heure.add(rainLast24Heure);
JsonArray OutdoorRelPressure = doc.createNestedArray("Pression atmosphérique ");
  OutdoorRelPressure.add(outdoorRelPressure);
JsonArray Humidity = doc.createNestedArray("Humidité ");
  Humidity.add(outdoorHumidity);
JsonArray IndoorTemperature = doc.createNestedArray("Température bme °C ");
  IndoorTemperature.add(indoorTemperature);
JsonArray Altitude = doc.createNestedArray("Altitude ");
  Altitude.add(altitude);

 
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  
  client.println("Refresh: 10");  // rafraîchit la page automatiquement toutes les 10 secondes
  client.print(F("Content-Length: "));

  client.println(measureJsonPretty(doc));
  client.println();


  // Write JSON document
  serializeJsonPretty(doc, client);
   
  // Disconnect
  client.stop();
     
}
