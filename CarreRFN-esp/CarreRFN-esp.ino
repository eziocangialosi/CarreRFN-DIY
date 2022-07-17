//CarreMQTT Version 1.0 Beta, Ezio Cangialosi, 2022

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#define OUTTOPIC  "CarreMQTT/TX"
#define INTOPIC   "CarreMQTT/RX"
#define ERRORSTR  "Error when trying to understand message or no corresponding action."

#define PINSTRIP 5
#define NBRPIXELS 18

#define NBRIMAGES 7
#define NBRPIXELS_PER_IMG 8

const char* RECO_PAYLOAD[10] = {"P" , "VL", "ACLI", "A", "SCLI", "S", "C", "GetSig", "SelfReboot", "Ping"}; 
const byte SPECIFIC_LED[NBRIMAGES] = {  3,   2,   3,   2,   3,   2,   0}; //2 = Oeuilleton, 1 = Cli, 3 = les deux, 0 = aucun
const byte LED_COL[3][NBRIMAGES] = {{   0,   0, 215, 215, 255, 255, 255},//Rouge
                                    {   0,   0,   0,   0,   0,   0,   0},//Bleu
                                    { 255, 255,  90,  90,   0,   0,   0}};//Vert
const byte NUM_LED_PIXEL[NBRPIXELS_PER_IMG][NBRIMAGES] = {{ 10,  10,   2,   2,   6,   6,   6},
                                                          { 11,  11,   3,   3,   7,   7,   7},
                                                          { 12,  12,   4,   4,   8,   8,   8},
                                                          { 13,  13,   5,   5,   9,   9,   9},
                                                          {255, 255, 255, 255, 255, 255,  14},
                                                          {255, 255, 255, 255, 255, 255,  15},
                                                          {255, 255, 255, 255, 255, 255,  16},
                                                          {255, 255, 255, 255, 255, 255,  17}};
                           
const byte RGB_OEUIT[3] = {255,255,255};
#define DELAY_CLI 500
const byte OEUIT_PIXELS[2] = {0,1}; 

bool g_flagCli = false;

const char* ssid = "YOUR SSID";
const char* password = "YOUR WIFI PASSWORD";
const char* mqtt_server = "YOUR BROKER";

byte g_curMode = 6;

long g_lastAttemptLed = 0;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBRPIXELS, PINSTRIP, NEO_GRB + NEO_KHZ800);

long lastReconnectAttempt = 0;

void(* resetFunc) (void) = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {//Est appelée automatiquement quand un nouveau message est détecté sur un topic
  String msg="";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    msg +=(char)payload[i];//On rentre le message dans une string pour le traiter en suite
  }
  master(msg);//Fonction de traitement principale
  Serial.println();
}

boolean reconnect() {
  Serial.print("Attempting MQTT connection...");
  // On essaye de se connecter en se nommant WakeOnDuino
  if (client.connect("CarreMQTT")) {
    Serial.println("connected");
    //Une fois connectée on publie un msg sur le topic de status
    client.publish(OUTTOPIC,"CarreMQTT Version 1.0 Beta Connected");
    //Et on écoute sur le topic de commande
    client.subscribe(INTOPIC);
    return client.connected();
  }
  else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
  }
}

void setup() {
  Serial.begin (9600); //initialisation de communication série
  strip.begin(); //Init Strip led
  delay(10);
  setAllStripBlank();
  strip.show();
  setup_wifi();
  client.setServer(mqtt_server, 1883);//Démarrage serveur mqtt sur port 1883
  client.setCallback(callback);//Ce qui permet de récupérer les messages entrants

  Serial.println("CarreMQTT Version 1.0 Beta");

  delay(1000);

  lastReconnectAttempt = 0;

  Serial.println("\nWait for MQTT Broker connection");
}

void loop(){
  if (!client.connected()) {//Si on n'est pas connecté au Broker
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {//Un essai toutes les 5secs
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }
  else {
    // Client connected
    client.loop();
    if(millis() - g_lastAttemptLed > DELAY_CLI){
      g_lastAttemptLed = millis();
      Serial.println("Do Led");
      doLed();
    }
  }
}

void master(String msg){
  for(byte i = 0; i < 10; i++){
    if(msg == RECO_PAYLOAD[i]){
      if(i == 7){
        returnSig();
      }else if (i == 8){
        selfReboot();
      }else if (i == 9){
        pingReceived();
      }else{
        g_lastAttemptLed = 0;
        g_curMode = i;
      }
    }
  }
}

void doLed(){
  setAllStripBlank();
  if(SPECIFIC_LED[g_curMode] & 0x02){
    strip.setPixelColor(OEUIT_PIXELS[0], strip.Color(RGB_OEUIT[0], RGB_OEUIT[1], RGB_OEUIT[2]));
    if(OEUIT_PIXELS[1] < NBRPIXELS){
      strip.setPixelColor(OEUIT_PIXELS[1], strip.Color(RGB_OEUIT[0], RGB_OEUIT[1], RGB_OEUIT[2]));
    }
  }
  for(unsigned int i = 0; i < NBRPIXELS_PER_IMG; i++){
    if(NUM_LED_PIXEL[i][g_curMode] < NBRPIXELS && !g_flagCli){
      strip.setPixelColor(NUM_LED_PIXEL[i][g_curMode], strip.Color(LED_COL[0][g_curMode], LED_COL[1][g_curMode], LED_COL[2][g_curMode]));
      Serial.println("Set lum");
    }
  }
  if(SPECIFIC_LED[g_curMode] & 0x01){
    g_flagCli = !g_flagCli;
  }else{
    g_flagCli = false;
  }
  
  strip.show();
}

void selfReboot(){
  Serial.println("Ordre de redémarrage reçu\nExecution dans 5 secondes");
  client.publish(OUTTOPIC,"Redémarrage demandé, execution dans 5 secondes...");
  delay(5000);
  Serial.println("Redémarrage");
  client.publish(OUTTOPIC,"Rebooting, See you soon world !");
  delay(100);
  resetFunc();
}

void returnSig(){
  Serial.println("Demande de retour de l'image affichée");
  client.publish(OUTTOPIC,RECO_PAYLOAD[g_curMode]);
  Serial.println("Image actuelle envoyée");
  delay(100);
}

void setAllStripBlank(){
  for(unsigned int i = 0; i < NBRPIXELS ; i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
}

void pingReceived(){
  Serial.println("Ping reçu, renvoi d'un 'pong'");
  client.publish(OUTTOPIC,"Pong");
  delay(10);
}
