#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "math.h"

WiFiUDP Udp;
const char *ssid = "nn";  //
const char *password = "nneunaiz";

char packetBuffer[255];
unsigned int localPort = 9999;
IPAddress ipServidor(192, 168, 4, 1);
IPAddress ipCliente(192, 168, 4, 10);
IPAddress Subnet(255, 255, 255, 0);

#define DEVICE (0x53) // Device address as specified in data sheet

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
char DATAY0 = 0x34;    //Y-Axis Data 0
char DATAY1 = 0x35;    //Y-Axis Data 1
char DATAZ0 = 0x36;    //Z-Axis Data 0
char DATAZ1 = 0x37;    //Z-Axis Data 1

void writeTo(byte address, byte val) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[]) 
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while(Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA); // para que solo sea STA y no genere la IP 192.168.4.1
  WiFi.config(ipCliente, ipServidor, Subnet);
  Udp.begin(localPort);

  Wire.begin(0,2);      
  
  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);
  
  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);
}
void loop() {

  uint8_t howManyBytesToRead = 6;
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
  
  // each axis reading comes in 10 bit resolution, ie 2 bytes. Least Significat Byte first!!
  // thus we are converting both bytes in to one
  // cave: esp8266 is 32bit, thus use short and not int as in Arduino examples
  short x = (((short)_buff[1]) << 8) | _buff[0];
  short y = (((short)_buff[3]) << 8) | _buff[2];
  short z = (((short)_buff[5]) << 8) | _buff[4];

  unsigned long total = 100*abs(sqrtf(x*x+y*y+z*z)*2./512-0.95);

//ENVIO
    Udp.beginPacket(ipServidor,9999);
    char buf[20];
    Serial.println(total);     
    sprintf(buf, "%lu", total); 
    Udp.write(buf);
    Udp.write("\r\n");
    Udp.endPacket();
    Serial.print("enviando: ");
    Serial.println(buf);
 
delay(50);
}
