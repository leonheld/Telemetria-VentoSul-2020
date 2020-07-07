/*
 * 
 */

#include <LoRaMESH.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>

#include <SPI.h>
#include <mcp2515.h>


// Cria o frame para armazenar a mensagem recebida da Rede CAN
struct can_frame canMsg;

/*
Inicia o objeto mcp2515 com o Pino de CS da
comunicação SPI sendo o pino digital 10
*/
MCP2515 mcp2515(10);

// Defines e declarações para funcionamento do Módulo LoRa
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


void setup() {
  
  // Inicializa a porta serial para Debug
  Serial.begin(9600);

  /*
  Inicializa a interface Serial emulado via Software
  para comunicação com o Módulo LoRa
  */
  hSerialCommands = SerialCommandsInit(7, 6, 9600);

  // Reseta os registradores do MCP2515
  mcp2515.reset();
  // Define a velocidade de transmissão para 500Kbps
  mcp2515.setBitrate(CAN_500KBPS);
  // Define o modo de operação como Normal(Receiver/Transmitter)
  mcp2515.setNormalMode();

    /*
    Tenta ler ID, Rede e ID Único do módulo e caso não seja possível
    mostra no monitor serial
    */
  if(LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
    Serial.print("Couldn't read local module info\n\n");
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


void loop(){
    // Lê a mensagem no CAN BUS e armazena no Struct canMsg
    if(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK){
        // Caso tudo corra bem nessa leitura mostra no monitor serial
        Serial.print("Mensagem recebida com sucesso");
    }
    
    /*  
    Atribui o ID da mensagem recebida da rede CAN 
    aos quatro primeiros bytes do Buffer do LoRa
    */
    bufferPayload[0] = canMsg.can_id >> 24;
    bufferPayload[1] = canMsg.can_id >> 16;
    bufferPayload[2] = canMsg.can_id >> 8;
    bufferPayload[3] = canMsg.can_id;
    
    // Transfere os bytes de dados de canMsg para o Buffer do LoRa
    for(int i = 4 , i < canMsg.can_dlc + 4, i++){
        bufferPayload[i] = canMsg.data[i];
    }

    // Prepara o Frame do LoRa com as informações contidas no buffer
      PrepareFrameCommand(localId, CMD_ANALOG, bufferPayload, canMsg.can_dlc + 4);
    // Envia o Frame via LoRa
      SendPacket();
}
