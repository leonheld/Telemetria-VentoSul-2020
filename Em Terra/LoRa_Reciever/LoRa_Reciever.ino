/*
 * 
 */

#include <LoRaMESH.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>


/* Defines ----------------------- */
#define CMD_ANALOG  50

/* Payload buffer */
uint8_t bufferPayload[MAX_PAYLOAD_SIZE] = {0};
uint8_t payloadSize = 0;

/* Local device ID */
uint16_t localId;

/* Remote device ID */
uint16_t remoteId;

/* Local device Unique ID */
uint32_t localUniqueId;

/* Local device Net */
uint16_t localNet;

/* Received command */
uint8_t command;

/* SoftwareSerial handles */
SoftwareSerial* hSerialCommands = NULL;

/* Initialization routine */
void setup() {
  delay(1000);
  Serial.begin(9600); /* Initialize monitor serial */
  
  /* Initialize SoftwareSerial */
  hSerialCommands = SerialCommandsInit(7, 6, 9600);

  /* Gets the local device ID */
  if(LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
    Serial.print("Couldn't read local ID\n\n");
  else
  {
    Serial.print("Local ID: ");
    Serial.println(localId);
    Serial.print("Local NET: ");
    Serial.println(localNet);
    Serial.print("Local Unique ID: ");
    Serial.println(localUniqueId, HEX);
    Serial.print("\n");
  }

}

/* Main loop */
void loop() {
    if(ReceivePacketCommand(&remoteId, &command, bufferPayload, &payloadSize, 5000) == MESH_OK)
    {
      /* Treats command */
      // ...
      
      /* Sends to monitor */
      Serial.print("\nID: ");
      Serial.print(remoteId);
      Serial.print("\nGPIO5: ");
      Serial.print(float(bufferPayload[0] | (bufferPayload[1]<<8))*8.0586e-4);
      Serial.print("V\n");

      /* Replies the message - ACK*/
      bufferPayload[0] = 0;
      bufferPayload[1] = 0;

      PrepareFrameCommand(remoteId, CMD_ANALOG, bufferPayload, 2);  // Payload size = 2
      SendPacket();
    }
  
  }
