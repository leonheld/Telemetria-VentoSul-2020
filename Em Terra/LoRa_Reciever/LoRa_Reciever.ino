/*
 * 
 */

#include <LoRaMESH.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>


/* =========================================================== */
/*  
 Define o comando de confirmação de recebimento à ser enviado
 para o Módulo LoRa embarcado
 */
#define CMD_INTERFACE 50
/* =========================================================== */

//* =========================================================== */
/* Buffer para o envio da mensagem via LoRa  */
uint8_t bufferPayload[MAX_PAYLOAD_SIZE] = {0};
/* Tamanho do bufferPayload */
uint8_t payloadSize = 0;
/* ID do Módulo LoRa conectado ao Arduino */
uint16_t localId;
/* ID do Módulo LoRa que enviou a mensagem */
uint16_t remoteId;
/* ID Único do Módulo LoRa conectado ao Arduino */
uint32_t localUniqueId;
/* Rede do Módulo LoRa conectado ao Arduino */
uint16_t localNet;
/* Faz a ligação da função SoftwareSerial à serial de comandos
do Módulo LoRa ligado ao Arduino */
SoftwareSerial* hSerialCommands = NULL;
/* =========================================================== */

/* Função de configuração do Arduino */ 
/* =========================================================== */
void setup() {
    /* Tempo de espera para que o Módulo LoRa Inicialize */
    delay(1000);
    /* 
     Inicializa o Monitor Serial para comunicação com o Software
     no computador conectado
    */
    Serial.begin(9600);
  
    /* Inicializa a Serial de Comandos com o Módulo LoRa */
    hSerialCommands = SerialCommandsInit(7, 6, 9600);

    /*
    Tenta ler ID, Rede e ID Único do módulo e retorna estes
    parametros ou uma mensagem de erro
    */
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
