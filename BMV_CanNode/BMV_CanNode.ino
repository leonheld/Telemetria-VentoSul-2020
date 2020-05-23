/*
 * 
 */

#include <LiquidCrystal.h>
#include <String.h>
#include "Variaveis.h"
#include <mcp2515.h>
#include <SPI.h>

/*
Inicia o objeto mcp2515 com o Pino de CS da
comunicação SPI sendo o pino digital 10
*/
MCP2515 mcp2515(10);

// Cria os frames CAN e define seus endereços
struct can_frame BMV_Voltage;
struct can_frame BMV_StartedVoltage;
struct can_frame BMV_Current;
struct can_frame BMV_ConsumedEnergy;
struct can_frame BMV_StateOfCharge;
struct can_frame BMV_TimeToGo;


void setup() {
  // Inicia a porta Serial de comunicação com o BMV 
  Serial.begin(19200);
  // Define o timeout da porta Serial para 10 milissegundos
  Serial.setTimeout(10);
  // Inicia o objeto LCD
  lcd.begin(16, 2);

  // Reseta os registradores do MCP2515
  mcp2515.reset();
  // Define a velocidade de transmissão para 500Kbps
  mcp2515.setBitrate(CAN_500KBPS);
  // Define o modo de operação como Normal(Receiver/Transmitter)
  mcp2515.setNormalMode();
  
  attachInterrupt(0, irqCounter, FALLING);
}

void loop() {
  /* 
  Caso não existam erros em BMVSerialString, executa 
  a cadeia de funções que processa os dados recebidos 
  pela porta Serial
  */ 
  if(BMVDataIntegrity()){
    BMVDataProcess(BMVSerialString,BMVReceivedBytes);
    BMVStringComplete = false;
    printDataToLCD();

    /*
    Verifica se os outros arduinos ligados a rede ja enviaram
    suas mensagens para o CAN BUS, completando um ciclo inteiro
    de aquisição de dados, caso tenha sido completado, inicia
    a transmissão novamente
    */
    if(interruptCounter > 13){
      interruptCounter = 0;
      mcp2515.sendMessage(&BMV_Voltage);
      delay(10);
      mcp2515.sendMessage(&BMV_StartedVoltage);
      delay(10);
      mcp2515.sendMessage(&BMV_Current);
      delay(10);
      mcp2515.sendMessage(&BMV_ConsumedEnergy);
      delay(10);
      mcp2515.sendMessage(&BMV_StateOfCharge);
      delay(10);
      mcp2515.sendMessage(&BMV_TimeToGo);
      delay(10);
    }
  }

}

//==============================================================================================//
//====================================Declaração das funções====================================//
//==============================================================================================//

//==============================================================================================//
void irqCounter(){
  /*
  Por meio de interrupções conta quantas mensagens ja foram 
  recebidas do CAN BUS desde a ultima trasmissão
  */
  interruptCounter++;
}
//==============================================================================================//

//==============================================================================================//
void serialEvent() {
/* 
Função executada antes de cada loop() que 
verifica se existem dados no buffer da porta
serial e os armazena em BMVSerialString
*/
  while(Serial.available()){
    BMVReceivedBytes = Serial.readBytes(BMVSerialString,140);
    BMVStringComplete = true;

  }
}
//==============================================================================================//

//==============================================================================================//
boolean BMVDataIntegrity(){
/*
Confere a integridade dos dados recebidos pela porta Serial
pelo checksum, se a soma de todos os dados recebidos pela porta
Serial for divisível por 256, não existem erros 
*/
  if(BMVStringComplete){
    checksum = 0;
    for(volatile int i = 0 ; i < BMVReceivedBytes ; i++){
      checksum += (int)BMVSerialString[i];
    }

  if((checksum % 256) == 0){
      return(true);
    }
  }
}
//==============================================================================================//

//==============================================================================================//
void BMVDataProcess(String s, int receivedBytes){
/*
OBS: BMVSerialString tem os dados no formato: <\n><Label><\t><Value>
Quebra o vetor de dados em cada \n e chama a função BMVGetValues
*/
  int startIndex = 2;
  int endIndex = 3;
  String line = "";
  while(startIndex < receivedBytes){
    endIndex = s.indexOf("\n", startIndex);
    if(endIndex < startIndex || endIndex > receivedBytes){
      endIndex = receivedBytes;
    }
    line = s.substring(startIndex, endIndex);
    BMVGetValues(line);
    startIndex = endIndex + 1;
  }
}
//==============================================================================================//

//==============================================================================================//
void BMVGetValues(String s){
/*
Identifica o nome e o valor presente em cada linha recebida de BMVDataProcess
e chama as funções BMVSetValues e DoCanFrame para cada par
*/
  int startIndex = 0;
  int endIndex = 0;
  endIndex  = s.indexOf("\t",startIndex);
  String name = s.substring(startIndex,endIndex);
  String value = s.substring(endIndex + 1, s.length());
  BMVSetValues(name , charToFloat(value));
  DoCANFrame(name, value);

}
//==============================================================================================//

//==============================================================================================//
void DoCANFrame(String label, String data){
/*
Identidica o nome de cada valor presente em cada linha recebida de BMVDataProcess
e escreve no frame correspondente
*/

  char tmp[data.length()];
  data.toCharArray(tmp,data.length());
  uint8_t data_length = data.length(); 

  if (label = "V"){
    BMV_Voltage.can_id  = 0x033;
    BMV_Voltage.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_Voltage.data[i] = tmp[i];
    }
  }

  if (label = "VS"){
    BMV_StartedVoltage.can_id  = 0x034;
    BMV_StartedVoltage.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_StartedVoltage.data[i] = tmp[i];
    }
  }

  if (label = "I"){
    BMV_Current.can_id  = 0x035;
    BMV_Current.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_Current.data[i] = tmp[i];
    }
  }

  if (label = "CE"){
    BMV_ConsumedEnergy.can_id  = 0x036;
    BMV_ConsumedEnergy.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_ConsumedEnergy.data[i] = tmp[i];
    }
  }

  if (label = "SOC"){
    BMV_StateOfCharge.can_id  = 0x037;
    BMV_StateOfCharge.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_StateOfCharge.data[i] = tmp[i];
    }
  }

  if (label = "TTG"){
    BMV_TimeToGo.can_id  = 0x038;
    BMV_TimeToGo.can_dlc = data_length;
    for(int i =0; i > data_length; i++){
      BMV_TimeToGo.data[i] = tmp[i];
    }
  }
else;
}
//==============================================================================================//

//==============================================================================================//
void BMVSetValues(String label, float value){
/*
Verifica em qual posição do Array BMVValues deve ir o dados e armazena nele
*/
  if (label == "V")         BMVValues[V]   = value;
  else if (label == "VS")   BMVValues[VS]  = value;
  else if (label == "I" )   BMVValues[I]   = value;
  else if (label == "CE")   BMVValues[CE]  = value;
  else if (label == "SOC")  BMVValues[SOC] = value;
  else if (label == "TTG")  BMVValues[TTG] = value;
  else;
  //batata
}
//==============================================================================================//

//==============================================================================================//
inline float charToFloat(String value){
/*
Converte value de BMVGetValues de String para float
*/
  char tmp[value.length()];
  // Converte de String para vetor de caracteres
  value.toCharArray(tmp,value.length());
  // Converte de vetor de caracteres para float
  return atof(tmp);
}
//==============================================================================================//

//==============================================================================================//
void printDataToLCD(){
/*
Imprime os dados do BMV no display LCD 
[Isso está aqui apenas para teste, na versão final essa função nao existe]
[Estou printando apenas Tensão e Corrente]
*/
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("V: ");
  lcd.setCursor(3,0);
  lcd.print(BMVValues[V]);

  lcd.setCursor(0,1);
  lcd.print("I: ");
  lcd.setCursor(3,1);
  lcd.print(BMVValues[I]);
}
