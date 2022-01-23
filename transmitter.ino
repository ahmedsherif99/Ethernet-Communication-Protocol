
#include "CRC.h"
#include <SoftwareSerial.h>

#define maxDataSize 512

uint8_t Eth_Buffer[maxDataSize+30];
int Eth_Size = 0;

uint8_t source_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t destination_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

uint8_t transmit[maxDataSize];
int transmit_i = 0;
bool Done = false;

SoftwareSerial comSerial(3, 4); // RX, TX

void setup() {
  Serial.begin(57600);
  comSerial.begin(4800);
}


void loop() {
   if (Serial.available()) {
    uint8_t Input_Char = (uint8_t)Serial.read();
    if (Input_Char == '\n') {
      Done = true;
    }
    else{
      transmit[transmit_i] += Input_Char;
      transmit_i++;
    }
  }

  if (Done) {
    encodeFrame(destination_MAC, source_MAC, transmit, transmit_i);

    for(int i = 0; i<Eth_Size; i++) comSerial.write(Eth_Buffer[i]);
    comSerial.write('\n');

    Done = false;
    for(int i = 0; i < transmit_i+1; i++)transmit[i] = 0;
    transmit_i = 0;
  }
}

void encodeFrame(uint8_t destinationMAC[6], uint8_t sourceMAC[6], uint8_t data[maxDataSize], int dataSize){
  // Clear Buffer
  for(int i = 0; i< maxDataSize+30; i++) Eth_Buffer[i] = 0x00;

  // Latch minimum data to 46 charachters
  if (dataSize < 46){
    for(int i = dataSize; i < 47; i++){
      data[i] = 0x00;
    }
    dataSize = 46;
  }

  // Add Preamble Frame -> byte 0 to 6 -> 7 bytes
  for(int i = 0; i < 7; i++)Eth_Buffer[i] = 0xAA;

  // Add Start Frame Delimeter -> byte 7 -> 1 byte
  Eth_Buffer[7] = 0xAB;

  // Add Destination MAC address  -> byte 8 to 13 -> 6 bytes
  for(int i = 8; i < 14; i++)Eth_Buffer[i] = destinationMAC[i-8];

  // Add Source MAC Address -> byte 14 to 19 -> 6 bytes
  for(int i = 14; i < 20; i++)Eth_Buffer[i] = sourceMAC[i-14];

  // Add Length Frame -> bytes 20 and 21 -> 2 bytes
  Eth_Buffer[20] = dataSize / 512; // First byte of size frame
  Eth_Buffer[21] = dataSize % 512; // Second byte of size frame

  // Add Data Frame -> bytes 22 to dataSize+22 (max 1522) (min 68) -> 46 - 1500 bytes
  for(int i = 22; i < dataSize+23; i++)Eth_Buffer[i] = data[i-22];

  // Calculate CRC32
  uint32_t crc = crc32(data, dataSize, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, false);

  // Add CRC Frame -> bytes dataSize+22 to dataSize+26 -> 4 bytes
  Eth_Buffer[dataSize+26] = (crc & 0x000000ff);
  Eth_Buffer[dataSize+25] = (crc & 0x0000ff00) >> 8;
  Eth_Buffer[dataSize+24] = (crc & 0x00ff0000) >> 16;
  Eth_Buffer[dataSize+23] = (crc & 0xff000000) >> 24;

  // Calculate total frame size
  Eth_Size = dataSize+27;

  Serial.print("Encoded ethernet frame in HEX: \t");
  for(int i = 0; i<Eth_Size; i++) {
    Serial.print ("0x");
    Serial.print (Eth_Buffer[i], HEX);
    Serial.print (" ");
    }
   Serial.println("");
   Serial.println("");
}
