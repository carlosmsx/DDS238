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

typedef struct {
  uint32_t ID;
  float total;
  float V;
  float I;
  int16_t P;
  int16_t Kvar;
  float CosPhi;
  float F;
  uint16_t settings;
  uint32_t bauds;
} READING;

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
  uint32_t tim = millis() + 5000;
  READING r;
  uint8_t result = node.readHoldingRegisters(0x0, 32); //leo 32 registros
  if (result == node.ku8MBSuccess)
  {
    r.total   = (float)(node.getResponseBuffer(0x00)*0x10000 + node.getResponseBuffer(0x01))/100.0;
    r.V       = (float)node.getResponseBuffer(0x0C)/10.0;
    r.I       = (float)node.getResponseBuffer(0x0D)/100.0;
    r.P       = node.getResponseBuffer(0x0E);
    r.Kvar    = node.getResponseBuffer(0x0F);
    r.CosPhi  = (float)node.getResponseBuffer(0x10)/1000.0;
    r.F       = (float)node.getResponseBuffer(0x11)/100.0;
    r.settings= node.getResponseBuffer(0x15);
    r.ID = r.settings>>8;

    uint32_t baudTable[] = {0, 9600, 4800, 2400, 1200};
    r.bauds = baudTable[r.settings & 0xff];

    Serial.write("ID="); Serial.println(r.ID);
    Serial.write("bauds="); Serial.println(r.bauds);
    Serial.print("acumulado="); Serial.print(r.total); Serial.println(" KW/h");
    Serial.print("V="); Serial.print(r.V); Serial.println(" volts");
    Serial.print("I="); Serial.print(r.I); Serial.println(" ampers");
    Serial.print("P="); Serial.print(r.P); Serial.println(" watts");
    Serial.print("KVar="); Serial.print(r.Kvar); Serial.println(" watts");
    Serial.print("cos PHI="); Serial.println(r.CosPhi);
    Serial.print("F="); Serial.print(r.F); Serial.println(" Hz");
  }
  Serial.println("-----");

  while (millis()<=tim);
}
