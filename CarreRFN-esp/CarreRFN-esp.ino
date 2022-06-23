//CarreMQTT Version 0.0.1 Alpha, Ezio Cangialosi, 2022

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#define OUTTOPIC  "CarreMQTT/TX"
#define INTOPIC   "CarreMQTT/RX"
#define ERRORSTR  "Error when trying to understand message or no corresponding action."

#define PINSTRIP 2
#define NUMPIXELS 5

#define RECO_PAYLOAD  ("P" , "VL", "ACLI", "A", "SCLI", "S", "C", "GetSig") 
#define SPECIFIC_LED  (   3,    1,      0,   0,      3,   2,   0)  //2 = Oeuilleton, 1 = Cli, 3 = les deux, 0 = aucun
const byte LED_COL[][3] = {{ 50,  50, 255, 255, 255, 255, 255},
                           {100, 100, 200, 200,   0,   0,   0},
                           { 50,  50,   0,   0,   0,   0,   0}};

const char* ssid = "Livebox-B84500";
const char* password = "........";
const char* mqtt_server = "192.168.1.16";

byte curMode = 6;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PINSTRIP, NEO_GRB + NEO_KHZ800);

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
    client.publish(OUTTOPIC,"CarreMQTT V0.0.1 Alpha Connected");
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
  setup_wifi();
  client.setServer(server, 1883);//Démarrage serveur mqtt sur port 1883
  client.setCallback(callback);//Ce qui permet de récupérer les messages entrants
  Ethernet.begin (mac, ip); //initialisation de la communication Ethernet

  Serial.println("CarreMQTT Version 0.0.1 Alpha");

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
    doLed(SPECIFIC_LED[aActionMode]);
  }
}

void master(String message){
  byte actionMode=checkString(message);//Traitement de la string et renvoie d'un code
  if(actionMode != 255){//Si il n'y a pas eu d'erreur dans la comprehension de la string
    selectAction(actionMode);
  }
  else{
    //byte errorStr[200]=+message;
    client.publish(OUTTOPIC,ERRORSTR);
  }
}

byte checkString(String aMessage){
 for(byte i = 0; i < sizeof(RECO_PAYLOAD); i++){
  if(aMessage == RECO_PAYLOAD[i]){
    return i;
  }
 }
 return 255;
}

void selectAction(byte aActionMode){
  if(aActionMode < 7){
    curMode = aActionMode;
  }
  else{
    client.publish(OUTTOPIC,RECO_PAYLOAD[curMode]);
  }
}

void doLed(byte options = 0){
  
}
