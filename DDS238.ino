/*
Project: This is an example that shows how to get data from DDS238 meters.
Author: Carlos Escobar
Board: Arduino Mega / Mega 2560
Requisites: ModbusMaster-2.0.1 - https://www.arduino.cc/reference/en/libraries/modbusmaster/

Wiring:
  Arduino 8 to DE+RE (MAX485)
  Arduino TX2 to DI  (MAX485)
  Arduino RX2 to RO  (MAX485)
*/

#include <ModbusMaster.h>
#define MAX485_DE 8

//lets instantitate the ModbusMaster object
ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  Serial.begin(9600);

  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  Serial2.begin(9600);
  
  // Modbus slave ID 1 (default)
  node.begin(1, Serial2);
  // These callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

//some data examples
//lampara led
//0x00/0x01: 0 4 (Current Total Energy)
//0x02/0x03: 513 0,
//0x04/0x05: 0 0, 
//0x06/0x07: 0 0,
//0x08/0x09: 0 0 (Current Export Energy)
//0x0A/0x0B: 0 4 (Current Import Energy)
//0x0C: V=2282/10 (voltaje)
//0x0D: I=3/100 (corriente)
//0x0E: P=(+/-)4/1000 KW (energia activa)
//0x0F: Kvar=(+/-)0/1000 KW (energia reactiva)
//0x10: COS(PHI)=550/1000 (cos de phi)
//0x11: F=5000/100 (frecuencia)
//0x12: 0 
//0x13: 0
//0x14: 0 
//0x15: 257=0x0101 -> Byte alto=ID (valores válidos entre 1 y 247), Byte bajo=baud (1:9600, 2:4800, 3: 2400, 4:1200)

//foco 42W
//0 4,513 0,0 0,0 0,0 0,0 4,2275 18,43 0,1000 4997,0 0,
//0 257,0 3,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,
//0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,
//0 0,0 0,

//cargador celular
//0 4,513 0,0 0,0 0,0 0,0 4,2276 7,9 0,555 4995,0 0,
//0 257,0 3,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,
//0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,
//0 0,0 0,


void loop()
{
  uint32_t total;
  uint16_t V;
  uint16_t I;
  int16_t P;
  int16_t Kvar;
  uint16_t CosPhi;
  uint16_t F;
  uint16_t settings;
    
  uint8_t result = node.readHoldingRegisters(0x0, 32); //leo 32 registros
  if (result == node.ku8MBSuccess)
  {
    total   = node.getResponseBuffer(0x00)<<16 | node.getResponseBuffer(0x01);
    V       = node.getResponseBuffer(0x0C);
    I       = node.getResponseBuffer(0x0D);
    P       = node.getResponseBuffer(0x0E);
    Kvar    = node.getResponseBuffer(0x0F);
    CosPhi  = node.getResponseBuffer(0x10);
    F       = node.getResponseBuffer(0x11);
    settings= node.getResponseBuffer(0x15);

    Serial.print("acumulado="); Serial.print(total); Serial.println(" KW/h");
    Serial.print("V="); Serial.print((float)V/10); Serial.println(" volts");
    Serial.print("I="); Serial.print((float)I/100); Serial.println(" ampers");
    Serial.print("P="); Serial.print(P); Serial.println(" watts");
    Serial.print("KVar="); Serial.print(Kvar); Serial.println(" watts");
    Serial.print("cos PHI="); Serial.println((float)CosPhi/1000);
    Serial.print("F="); Serial.print((float)F/100); Serial.println(" Hz");
    switch(settings&0xff)
    {
      case 1:
        Serial.println("bauds=9600"); 
        break;
      case 2:
        Serial.println("bauds=4800"); 
        break;
      case 3:
        Serial.println("bauds=2400"); 
        break;
      case 4:
        Serial.println("bauds=1200"); 
        break;
    }
    Serial.write("ID="); Serial.println(settings>>8);
  }
  Serial.println("-----");

  delay(1000);
}
