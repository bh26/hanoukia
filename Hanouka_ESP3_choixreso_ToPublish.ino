
//****************************************************
//* this program allows to light up the various lamps for Hanouka, each day in the order defined 
//* for that day. it can be run directly from a browser using a pc or a smartphone, or it can be called 
//* from an application built with MIT appinventor stored in an Android phone.
//* In order to connect the server to Wifi, a list of networks for which the password is known 
//* is written in the program. For flexibility it is possible to setup a mobile access point using a smartphone
//****************************************************
#include <ESP8266WiFi.h>


//********************
////////////////
char* ssidListe[] = {"SSID1","SSID2","SSID3","SSID4"};     // SSIDs for each known wifi network
char* passwordListe[] = {"Password1","Password2","Password3","Password4"};  // the passwords associated to each network
int nbresx = 4; // Number of known and authorized networks

char ssidIntermediaire[20];
char ssidTest[20];
char ssid[20];
char password[30];
int rssiListe[] ={-50,-50,-50,-50};
int rssi = -100; // Network signal power, best is -100, worst is 0
int numResMax = 0; // the best network indice among all networks found


//**********************
uint8_t MAC_array[6];
 char MAC_char[18];

WiFiServer server(80);

int tDelay = 200;
int latchPin = 14;      // (11) ST_CP [RCK] on 74HC595
int clockPin = 13;      // (9) SH_CP [SCK] on 74HC595
int dataPin = 15;     // (12) DS [S1] on 74HC595
int chamach = 4;
byte leds = 0;
int value = 0;

//********** function to command each light using the 74HC595 transistor  ******************
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

//******************************************
void scanreseau(){ // Looks for the best network and connects to it
    // ************ scan all 'visible' wifi networks  *************
  Serial.println("scan start");
  rssi = -100;
  numResMax = 0;
  rssiListe[0] = -80;
  rssiListe[1] = -80;
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0){
    Serial.println("no networks found");
  }else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.print((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"* ");
      Serial.println (WiFi.encryptionType(i));
      delay(10);
      WiFi.SSID(i).toCharArray(ssidIntermediaire,20); // store the network SSID for line i to ssidIntermediaire variable
      
      Serial.print("contenu de ssidIntermediaire : ");
      Serial.println(ssidIntermediaire);
      
    //*** if network i has a betteer RSSI (power) than the previous one, and if it is among the known networks we put keep track of its position (i into numresmax)
      if (WiFi.RSSI(i) > rssi)
      {
        int resconnu = 0;
        for (int j = 0; j < nbresx; ++j)
    {
       if  (WiFi.SSID(i) == ssidListe[j]){
        resconnu =1;
       }
    }
      if (resconnu == 1) {
        rssi = WiFi.RSSI(i);
        numResMax = i;
        Serial.print ("puissance du réseau : ");
        Serial.print(i + 1);
        Serial.print(" est : ");
        Serial.println(rssi);
      }
    }
    }
    // *** end networks analysis*************
     Serial.print("numero du reseau choisi: ");
     Serial.println(numResMax + 1);     
  }
  // *** Best network chosen ************
      
      WiFi.SSID(numResMax).toCharArray(ssidIntermediaire,20);
      
      Serial.print("nom du reseau ssidintermediaire le plus puissant: ");
      Serial.println(ssidIntermediaire);
       strcpy(ssid, ssidIntermediaire);
      Serial.print("nom du reseau ssid: ");
      Serial.println(ssid);
     //********* Look for the corresponding password in the known networks ************
      for (int j=0; j < nbresx; ++j)
    {
      if (WiFi.SSID(numResMax) == ssidListe[j])
      { 
        //Serial.print("test réseau choisi: ");
        //Serial.println(ssidIntermediaire);
        strcpy(password,passwordListe[j]); 
        Serial.print(" test password: "); 
        Serial.println(password);
      }
    }
  
       
  Serial.print("réseau choisi: ");
  Serial.println(ssid);
  Serial.print("password: "); 
  Serial.println(password);
  Serial.println("");
  // ************ fin du scan  ******************
}


//******************************************
void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(chamach, OUTPUT);
  updateShiftRegister();
  digitalWrite(chamach,LOW);
  Serial.begin(115200);

// Connect to WiFi network
scanreseau();
Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  WiFi.macAddress(MAC_array);
    for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
    }
  
    Serial.println(MAC_char);

}

void loop() 
{

// Make the shamash blink continuously
if (value >= 1 && value <= 8)
    {
for (int j = 0; j < 5; j++)
  {
  digitalWrite (chamach,HIGH);
  delay (tDelay);
  digitalWrite (chamach,LOW);
  delay (tDelay);
  }
    }
  
  // Check if a client has connected
  WiFiClient client = server.available();
 
  if (!client) {
  //Serial.println("Pas de client");
    return;
  }
   // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // Match the request
 
  
  if (request.indexOf("/x") != -1) {
     value = 0;
  }
  if (request.indexOf("/1 ") != -1) {
     value = 1;
  } 
  if (request.indexOf("/2") != -1) {
     value = 2;
  } 
  if (request.indexOf("/3") != -1) {
     value = 3;
  } 
  if (request.indexOf("/4") != -1) {
     value = 4;
  } 
  if (request.indexOf("/5") != -1) {
     value = 5;
  } 
  if (request.indexOf("/6") != -1) {
     value = 6;
  } 
  if (request.indexOf("/7") != -1) {
     value = 7;
  } 
  if (request.indexOf("/8") != -1) {
     value = 8;
  } 

  Serial.println (value);
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  if (value == 0)
  {
       client.println("La Hanoukia est éteinte ");
       client.println("<br><br>");
  }
  else
  {
  client.print("La Hanoukia est positionnee pour l'allumage du ");
   
  if(value == 1) {
    client.print("1er ");  
  } else {
    client.print(value);
    client.print("eme ");
  }
  client.println("jour");
  client.println("<br><br>");
  }
  client.println("Cliquer <a href=\"/1\"> ici</a> pour demander l'allumage du 1er soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/2\"> ici</a> pour demander l'allumage du 2eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/3\"> ici</a> pour demander l'allumage du 3eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/4\"> ici</a> pour demander l'allumage du 4eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/5\"> ici</a> pour demander l'allumage du 5eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/6\"> ici</a> pour demander l'allumage du 6eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/7\"> ici</a> pour demander l'allumage du 7eme soir de Hanouka<br>");
  client.println("Cliquer <a href=\"/8\"> ici</a> pour demander l'allumage du 8eme soir de Hanouka<br>");
    client.println("<br><br>");

  client.println("Cliquer <a href=\"/x\"> ici</a> pour eteindre la hanoukia<br>");
   
    client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
// after sending the page to the server, we light on the lights on the appropriate order, from the last to the first
  if (value >= 1 && value <= 8)
    {
      int led = value - 1;
      for (int i = led; i > -1; i--)
  {
    bitSet(leds, i);
    updateShiftRegister();
    delay(tDelay);
  }
  for (int j = 0; j < 5; j++)
  {
  digitalWrite (chamach,HIGH);
  delay (tDelay);
  digitalWrite (chamach,LOW);
  delay (tDelay);
  }
  digitalWrite (chamach,HIGH);
    }

 if (value == 0)
    {
      leds = 0;
      updateShiftRegister();
      Serial.println("Hanoukia éteinte");
      digitalWrite (chamach,LOW);
    }

}

