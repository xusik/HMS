
// 	                               +-\/-+
// 	                        VCC   1|    |20  GND
// 	                        P1.0  2|    |19  XIN
// 	                        P1.1  3|    |18  XOUT
// 	                        P1.2  4|    |17  TEST
// 	                        P1.3  5|    |16  RST#
// 	                        P1.4  6|    |15  P1.7 MOSI  6
// 	                 5  SCK P1.5  7|    |14  P1.6 MISO  7
// 	                 3   CE P2.0  8|    |13  P2.5 DHT11 2
// 	   	         4  CSN P2.1  9|    |12  P2.4 SCL
//		         8  IRQ P2.2 10|    |11  P2.3 SDA
// 			               +----+

#include "Energia.h"
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>
#include "I2C_SoftwareLibrary.h"
#include <dht.h>


#define SCL_PIN P2_4 ///< pin for SCL
#define SDA_PIN P2_3 ///< pin for SDA
SoftwareWire Wire(SDA_PIN, SCL_PIN); ///< Instantiate SoftwareWire

#define _address 0x18
#include <BMP085_t.h>      // import BMP085 template library
// Include application, user and local libraries


Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

const char *str_on = "ON";
const char *str_off = "OFF";
String Pres, TempL, TempH;

unsigned char pin = P2_5;

void dump_radio_status_to_serialport(uint8_t);

BMP085<0> PSensor;         // instantiate sensor, 0 = low precision pressure reading

void setup(void) {
    
    Serial.begin(9600);
    Serial.println("*** START");
    
    Serial.print("I2C begin... ");
    
    Wire.begin();
    
    PSensor.begin();         // initalize pressure sensor    
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);

    radio.begin();  // Defaults 1Mbps, channel 0, max TX power
    radio.setChannel (120);
    radio.setTXaddress((void*)txaddr);    
}
void loop(void)
{
  int16_t rawtemperature, rawhumidity;
  if (dht::readRawData(pin, &rawtemperature, &rawhumidity,false) == 0)
  {    
      Serial.print("(raw) T: ");
      Serial.print(rawtemperature);
      Serial.print(" H: ");
      Serial.print(rawhumidity); 
      
  }  
  
  delay (1000);
  PSensor.refresh();                    // read current sensor data
  delay (1000);
  PSensor.calculate();                  // run calculations for temperature and pressure
  TempH  = String(PSensor.temperature/10);
  TempL = String(PSensor.temperature%10);
  Pres = String((PSensor.pressure+50)/100);

                        // wait a while before next loop
//  Serial.println("Sending packet: ");
  Serial.println(Pres);
  radio.print("P"+Pres);
  radio.flush();
  
  Serial.print("Sending packet: ");
  Serial.println(rawhumidity);
  radio.print("H"+String(rawhumidity));
  radio.flush();

//  Serial.print("Sending packet: ");
  Serial.println(TempH + "."+ TempL);
  radio.print("T"+TempH + "."+ TempL);
  radio.flush();
  
  sleepSeconds(300);
}

