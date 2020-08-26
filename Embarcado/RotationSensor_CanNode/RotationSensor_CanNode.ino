#include "Variaveis.h"
#include <mcp2515.h>
#include <String.h>

// Quantidade de mensagens de outros nós entre transmissões locais
const unsigned int total_messages;

/*
Inicia o objeto mcp2515 com o Pino de CS da
comunicação SPI sendo o pino digital 10
*/
MCP2515 mcp2515(10);

// Cria o frames CAN e define seu endereço
struct can_frame RotSen_RPM;

void setup() {
  // Reseta os registradores do MCP2515
  mcp2515.reset();
  // Define a velocidade de transmissão para 500Kbps
  mcp2515.setBitrate(CAN_500KBPS);
  // Define o modo de operação como Normal(Receiver/Transmitter)
  mcp2515.setNormalMode();
    
  // Inicia a interrupção no pino 3, para contar as mensagens no CAN BUS
  attachInterrupt(digitalPinToInterrupt(3), irqCounter, FALLING);

  // Inicia comunicação serial
  Serial.begin(9600);

  //Inicia interrupção no pino 2, tendo como leitura a ida de Low a High
  attachInterrupt(digitalPinToInterrupt(2), PULSO_EVENTO, RISING);

  // Esse tempo evita problemas com as funções
  delay(1000); 
}

void loop() 
{

  // Armazena duas variáveis que seram modificadas no meio da rotação e
  // serão utilizadas para fazer as contas.
  Tempo_ciclo_ant = Tempo_ant_med;
  Tempo_Total = micros();
  Ciclos_atuais = Num_ciclos;

  Periodo_medio = Tempo_total/Ciclos_atuais;

  // Em caso de ocorrer algum bug na medição da variável Temp_ciclo_ant este if o corrige
  if(Tempo_Total < Tempo_ciclo_ant){
    Tempo_ciclo_ant = Tempo_Total;
  }

  /*
   Se o tempo entre a última rotação e o tempo atual for maior que x este if 
    transforma a frequencia em 0 para evitar de mandar informação falsa em caso de o motor estar parado
    Também evita problemas caso o sistema de medição seja ligado com o motor parado 
  */
  if( Tempo_Total - Tempo_ciclo_ant > 1000000||Ciclos_atuais == 0) {
    
    Frequencia_sem_trat = 0;
  }
  else{  
    // Calcula a frequencia
    Frequencia_sem_trat = 1000000/Periodo_entre_rot_atual;   
  }
  
  // Transforma Hertz em RPM
  RPM = Frequencia_sem_trat * 60;

  DoCanFrame(RPM);

  Serial.print(RPM);

  /*
  Verifica se os outros arduinos ligados a rede ja enviaram
  suas mensagens para o CAN BUS, completando um ciclo inteiro
  de aquisição de dados, caso tenha sido completado, inicia
  a transmissão novamente
  */
  if(interruptCounter > total_messages){
      interruptCounter = 0;
      mcp2515.sendMessage(&RotSen_RPM);
      delay(10);
    }

  delay(10);
}


//==============================================================================================//
//====================================Declaração das funções====================================//
//==============================================================================================//

/*
  Por meio de interrupções conta quantas mensagens ja foram 
  recebidas do CAN BUS desde a ultima trasmissão
 */
void irqCounter(){
    interruptCounter++;
}

/*
Calcula o periodo da ultima rotação
*/
void PULSO_EVENTO(){

  // Calcula o periodo da ultima rotação
  Periodo_entre_rot_atual = micros() - Tempo_ant_med; 

  // Armazena micros para ser comparado na próxima interrupção
  Tempo_ant_med = micros();

  //Incrementa o numero de rotações
  Num_ciclos++;
  
}

/*
Converte RPM para string depois para char
para poder adicionar ao frame CAN
*/
void DoCanFrame(int valor){
  String strRPM = String(RPM); 
  
  char tmp[strRPM.length()];
  strRPM.toCharArray(tmp,strRPM.length());
  uint8_t strRPM_length = strRPM.length();

  RotSen_RPM.can_id = 0x030;
  RotSen_RPM.can_dlc = strRPM_length;
  for (int i = 0; i < strRPM_length; i++){
    RotSen_RPM.data[i] = tmp[i];
  }
}
