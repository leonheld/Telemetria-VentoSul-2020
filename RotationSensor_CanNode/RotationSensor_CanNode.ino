#include 'Variaveis.h'

void setup() {
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

  Serial.print(RPM);

  delay(500);
}

// Calcula o periodo da ultima rotação
void PULSO_EVENTO(){

  // Calcula o periodo da ultima rotação
  Periodo_entre_rot_atual = micros() - Tempo_ant_med; 

  // Armazena micros para ser comparado na próxima interrupção
  Tempo_ant_med = micros();

  //Incrementa o numero de rotações
  Num_ciclos++;
  
}
