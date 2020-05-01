/*
 * 
 */

#include <LiquidCrystal.h>
#include <String.h>
#include "Variaveis.h"

void setup() {
  // Inicia a porta Serial de comunicação com o BMV 
  Serial.begin(19200);
  // Define o timeout da porta Serial para 10 milissegundos
  Serial.setTimeout(10);
  // Inicia o objeto LCD
  lcd.begin(16, 2);

}

void loop() {
  /* 
  Caso não existam erros em BMVSerialString, executa 
  a cadeia de funções que processa os dados recebidos 
  pela porta Serial e os armazena em BMVValues
  */ 
  if(BMVDataIntegrity()){
    BMVDataProcess(BMVSerialString,BMVReceivedBytes);
    BMVStringComplete = false;
    printDataToLCD();
  };

}

//==============================================================================================//
//====================================Declaração das funções====================================//
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
e chama a função BMVSetValues para cada par
*/
  int startIndex = 0;
  int endIndex = 0;
  endIndex  = s.indexOf("\t",startIndex);
  String name = s.substring(startIndex,endIndex);
  String value = s.substring(endIndex + 1, s.length());
  BMVSetValues(name , charToFloat(value));
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
