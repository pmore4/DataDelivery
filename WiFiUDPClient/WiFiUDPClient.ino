/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

#include <Adafruit_NeoMatrix.h>
#include <gamma.h>

#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

#define PIN 15

// WiFi network name and password:
const char * networkName = "Wifi2.4gig";
const char * networkPswd = "Acro2117";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.0.255";
const int udpPort = 3333;
char packetBuffer[255]; //buffer to hold incoming packet
//Are we currently connected?
boolean connected = false;
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(4, 8, PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);
//The udp library class
WiFiUDP udp;

int incomingByte;      // a variable to read incoming serial data into
int iB;
String inputString = "\n";         // a String to hold incoming data
boolean stringComplete = true;  // whether the string is complete
const int len=200;
char buf[len]= "";
String inS = "";
const uint16_t colors[] = {
matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255),matrix.Color(250,210,0),matrix.Color(255, 250, 255) };


void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  matrix.begin();
  matrix.setBrightness(5);
  matrix.fillScreen(0);
}

void loop(){
  //only send data when connected
  if(connected){
    //receive packet
    int packetSize =udp.parsePacket();
    if (packetSize){
      Serial.print("Received packet size: ");
      Serial.println(packetSize);
      Serial.print("From : ");
      IPAddress remoteIP = udp.remoteIP();
      Serial.print(remoteIP);
      Serial.print(", port ");
      Serial.println(udp.remotePort());

      int len = udp.read(packetBuffer,255);
      if (len > 0){
        packetBuffer[len] = 0;
      }
      Serial.println("Contents :");
      Serial.println(packetBuffer);
      int x = 0;
      int i = 0;
      int j = 0;
      boolean eFlag = false;
      matrix.fillScreen(0);
      while(eFlag == false){
        Serial.print("index ");
        Serial.print(x);
        Serial.print("  char");
        Serial.println(packetBuffer[x]);
        Serial.print(" i ");
        Serial.print(i);
        Serial.print("  ");
        Serial.print(" j ");
        Serial.print(j);
        Serial.print("  ");
        inS = packetBuffer[x];
        
        Serial.println(inS);
        matrix.drawPixel(i,j,colors[inS.toInt()]);
        matrix.show();
        if (j > 6){
          i++;
          j=0;
        }
        else{
          j++;
        }
        if (buf[x] == '\n'){
          eFlag = true;
        }
        if (x > 100){
          eFlag =true;
        }
        x+=2;
      }
      
    }
    
    //Send a packet
    //udp.beginPacket(udpAddress,udpPort);
    //udp.printf("Seconds since boot: %u", millis()/1000);
    //udp.endPacket();
  }
  //Wait for 1 second
  
  delay(1000);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
