#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN            2
#define NUMPIXELS      7
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


WiFiUDP Udp;
char packetBuffer[255];
unsigned int localPort = 9999;
const char *ssid = "nn";  
const char *password = "nneunaiz";

void setup() {
  
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Udp.begin(localPort);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  pixels.begin(); // This initializes the NeoPixel library.
  }

void loop() {
  
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len-1] = 0;
    Serial.print("Recibido(IP/Size/Data): ");
    Serial.print(Udp.remoteIP());Serial.print(" / ");
    Serial.print(packetSize);Serial.print(" / ");
    Serial.print(packetBuffer);
    long int received = atoi(packetBuffer);
    Serial.print("---");
    Serial.println(received);
    if (received < 5) received = 0;

     for(int i=0;i<NUMPIXELS;i++){
       pixels.setPixelColor(i, pixels.Color(0,received,0)); // Moderately bright green color.
       pixels.show();
       }  
    }
   delay(50);
}
