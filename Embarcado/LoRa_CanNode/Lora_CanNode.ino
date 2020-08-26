/*
 * 
 */

#include <LoRaMESH.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>

#include <SPI.h>
#include <mcp2515.h>

/* =========================================================== */
/*  Cria o frame para armazenar a mensagem recebida do CAN BUS */
struct can_frame canMsg;
/*
 Inicia o objeto mcp2515 com o Pino de CS da  comunicação SPI
 sendo o pino digital 10
*/
MCP2515 mcp2515(10);
/* =========================================================== */


/* =========================================================== */
/*  
 Define o comando de interface entre o CAN BUS e o Módulo LoRa
 a ser enviado para o módulo LoRa pela serial do Arduino
 */
#define CMD_INTERFACE 50
/* =========================================================== */


/* =========================================================== */
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
  
  /* Inicializa a porta serial para Debug */ 
  Serial.begin(9600);

  /*
  Inicializa a interface Serial emulado via Software
  para comunicação com o Módulo LoRa
  */
  hSerialCommands = SerialCommandsInit(7, 6, 9600);

  /* Reseta os registradores do MCP2515 */
  mcp2515.reset();
  /* Define a velocidade de transmissão para 500Kbps */
  mcp2515.setBitrate(CAN_500KBPS);
  /* Define o modo de operação como Normal(Recebe/Envia) */
  mcp2515.setNormalMode();
  
  /*
  Tenta ler ID, Rede e ID Único do módulo e retorna estes
  parametros ou uma mensagem de erro
  */
  if(LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
    Serial.print("Não foi possivel ler as informações do módulo\n\n");
  else
  {
    Serial.print("ID Local: ");
    Serial.println(localId);
    Serial.print("Rede: ");
    Serial.println(localNet);
    Serial.print("ID Unico: ");
    Serial.println(localUniqueId, HEX);
    Serial.print("\n");
  }
}
/* =========================================================== */

/* Função que rodará enquanto o Arduino estiver ligado */
/* =========================================================== */
void loop(){
     /* Lê a mensagem no CAN BUS e armazena em canMsg */
    if(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK){
        /*
        Caso tudo corra bem nessa leitura mostra no monitor serial
         */
        Serial.print("Mensagem recebida com sucesso");
    }
    
    /*  
    Atribui o ID da mensagem recebida da rede CAN  aos quatro
    primeiros bytes do Buffer a ser enviado para o Módulo LoRa
    */
    bufferPayload[0] = canMsg.can_id >> 24;
    bufferPayload[1] = canMsg.can_id >> 16;
    bufferPayload[2] = canMsg.can_id >> 8;
    bufferPayload[3] = canMsg.can_id;
    
    /*
    Transfere os bytes de dados de canMsg para o Buffer a ser
    enviado para o Módulo LoRa
    */
    for(int i = 4 ; i < canMsg.can_dlc + 4; i++){
        bufferPayload[i] = canMsg.data[i];
    }

    /*
    Prepara o Frame a ser enviado para o Módulo LoRa com as
    informações contidas no buffer
    */
      PrepareFrameCommand(localId, #define CMD_INTERFACE 55, bufferPayload, canMsg.can_dlc + 4);
    /* Envia o Frame para o Módulo LoRa */
      SendPacket();
}
/* =========================================================== */