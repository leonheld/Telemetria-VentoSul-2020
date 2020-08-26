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
/* Comando Recebido do Módulo LoRa embarcado */
uint8_t command;
/* Faz a ligação da função SoftwareSerial à serial de comandos
do Módulo LoRa ligado ao Arduino */
SoftwareSerial* hSerialCommands = NULL;


struct LoRaMsg{
    uint32_t label; /* ID do nó do CAN BUS embarcado recebido via LoRa */
    uint8_t data[8]; /* Dados do nó do CAN BUS embarcado recebido via LoRa */
};

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
/* =========================================================== */


/* Função que rodará enquanto o Arduino estiver ligado */
/* =========================================================== */
void loop() {
    if(ReceivePacketCommand(&remoteId, &command, bufferPayload, &payloadSize, 5000) == MESH_OK)
    {
      struct LoRaMsg info = DataProcess(bufferPayload);
      Serial.write("\n");
      Serial.write(info.label);
      Serial.write("\t");
      Serial.write(info.data,8);
      
      /* 
       Envia uma confirmação de recebimento para o LoRa embarcado
      */
      bufferPayload[0] = 0;
      bufferPayload[1] = 0;
      PrepareFrameCommand(remoteId, CMD_INTERFACE, bufferPayload, 2);
      SendPacket();
    }
  }
/* =========================================================== */

inline struct LoRaMsg DataProcess(uint8_t LoRa_Data[MAX_PAYLOAD_SIZE]){
    struct LoRaMsg frame;
    frame.label += LoRa_Data[3];
    frame.label += LoRa_Data[2] << 8;
    frame.label += LoRa_Data[1] << 16;
    frame.label += LoRa_Data[0] << 24;

    for (int i = 0; i < 8; i++){
        frame.data[i] = LoRa_Data[i+4];
    }

    // frame.data += LoRa_Data[11]<< 56;
    // frame.data += LoRa_Data[10]<< 48;
    // frame.data += LoRa_Data[9] << 40;
    // frame.data += LoRa_Data[8] << 32;
    // frame.data += LoRa_Data[7] << 24;
    // frame.data += LoRa_Data[6] << 16;
    // frame.data += LoRa_Data[5] << 8;
    // frame.data += LoRa_Data[4];

    return frame;
};
