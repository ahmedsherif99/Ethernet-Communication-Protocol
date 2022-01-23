
#include "CRC.h"
#include <SoftwareSerial.h>

#define maxDataSize 512

uint8_t destinationMAC[6];
uint8_t sourceMAC[6];
uint8_t data[maxDataSize];
uint8_t sizeBuffer[2];
int dataSize;
int data_integrity;

uint8_t receive[maxDataSize];
int receive_i = 0;
bool Done = false;

SoftwareSerial comSerial(3, 4); // RX, TX


void setup() {
  Serial.begin(57600);
  comSerial.begin(4800);
}


void loop() {
  if (comSerial.available()) {
    uint8_t Input_Car = (uint8_t)comSerial.read();
    if (Input_Car == '\n') {
      Done = true;
    }
    else{
      receive[receive_i] += Input_Car;
      receive_i++;
    }
  }
  
  if (Done) {  
    decodeFrame(receive);
    
    Done = false;
    for(int i = 0; i < receive_i+1; i++)receive[i] = 0;
    receive_i = 0;
  }
  
}

void decodeFrame(uint8_t Eth_Buffer[maxDataSize+30]){  
  // Get Destination MAC address  -> byte 8 to 13 -> 6 bytes
  for(int i = 8; i < 14; i++)destinationMAC[i-8] = Eth_Buffer[i];
  
  // Get Source MAC Address -> byte 14 to 19 -> 6 bytes
  for(int i = 14; i < 20; i++)sourceMAC[i-14] = Eth_Buffer[i];
  
  // Get Length Frame -> bytes 20 and 21 -> 2 bytes
  sizeBuffer[0] = Eth_Buffer[20];
  sizeBuffer[1] = Eth_Buffer[21];
  dataSize = (Eth_Buffer[20]<<8) + Eth_Buffer[21];

  if (dataSize > maxDataSize){
    Serial.println("ERROR: MORE THAN MAXIMUM DATA SIZE WAS RECEIVED !");
  }
  
  // Get Data Frame -> bytes 22 to dataSize+22 (max 1522) (min 68) -> 46 - 1500 bytes
  for(int i = 22; i < dataSize+23; i++)data[i-22] = Eth_Buffer[i];

  // Calculate CRC32
  uint32_t crc = crc32(data, dataSize, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, false);
  uint8_t calc_crc[4] = {0x00, 0x00, 0x00, 0x00};
  calc_crc[0] = (crc & 0x000000ff);
  calc_crc[1] = (crc & 0x0000ff00) >> 8;
  calc_crc[2] = (crc & 0x00ff0000) >> 16;
  calc_crc[3] = (crc & 0xff000000) >> 24;

  // Get CRC
  uint8_t in_crc[4] = {0x00, 0x00, 0x00, 0x00};
  in_crc[0] = Eth_Buffer[dataSize+26];
  in_crc[1] = Eth_Buffer[dataSize+25];
  in_crc[2] = Eth_Buffer[dataSize+24];
  in_crc[3] = Eth_Buffer[dataSize+23];
  
  // Check CRC
  data_integrity = 0;
  
  if(in_crc[0] == calc_crc[0] && in_crc[1] == calc_crc[1] && in_crc[2] == calc_crc[2] && in_crc[3] == calc_crc[3]){
    data_integrity = 1;
  }

  // Calculate total frame size
  int Eth_Size = dataSize+26;
  
  Serial.println("----------------------------------------------------------------------------");

  Serial.print("Received Ethernet frame: \t");
  for(int i = 0; i< Eth_Size; i++) {
    Serial.print ("0x");
    Serial.print (Eth_Buffer[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");

 Serial.print("Destination MAC: \t\t");
    for(int i = 0; i< 6; i++) {
    Serial.print ("0x");
    Serial.print (destinationMAC[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");
  
  Serial.print("Source MAC: \t\t\t");
   for(int i = 0; i< 6; i++) {
    Serial.print ("0x");
    Serial.print (sourceMAC[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");
  
  Serial.print("Data Size: \t\t\t");
  Serial.println(dataSize);
  
  Serial.print("Data: \t\t\t\t");
 for(int i = 0; i< dataSize; i++) {
    Serial.print ("0x");
    Serial.print (data[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");

  Serial.print("Calculated CRC:\t\t\t");
for(int i = 0; i< 4; i++) {
    Serial.print ("0x");
    Serial.print (calc_crc[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");

  Serial.print("Received CRC:\t\t\t");
  for(int i = 0; i< 4; i++) {
    Serial.print ("0x");
    Serial.print (in_crc[i], HEX);
    Serial.print (" ");
    }
  Serial.println("");
  
  Serial.print("Data Integrity: \t\t");
  Serial.println(data_integrity);

 Serial.print("Decoded Data: \t\t\t");
  for(int i = 0; i<dataSize; i++) Serial.write (data[i]);
  Serial.println("");
  
  Serial.println("");
 Serial.println("");
}
