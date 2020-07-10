// Variáveis
volatile unsigned long Periodo_entre_rot_atual; // Grava o Periodo mais recente
volatile unsigned long Tempo_ant_med; // É a variável que guarda o tempo que ocorreu a última rotação
volatile unsigned int Num_ciclos = 0; // Armazena o numero de ciclos do motor
unsigned long Periodo_medio; // Grava o periodo médio durante o uso do motor
unsigned long Tempo_ciclo_ant; // É a variável que guarda o Tempo_ant_medido para ser usado no void loop, criada devido o problema da interrupção e da perca de dados devido à ela
unsigned long Tempo_total; // Armazena a func micros para fazer um comparativo com Temp_ciclo_ant
unsigned long Frequencia_sem_trat; // É onde se calcula a frequencia em hertz
unsigned long RPM; // Variável que armazena o numero de rotações por minuto
unsigned int Ciclos_atuais = 0; // É a variável que guarda o Num_ciclos para ser usado no void loop, criada devido o problema da interrupção e da perca de dados devido à ela

unsigned long   Tempo_Total = micros();

void setup() {
Serial.begin(9600); // Começa a comunicação serial
attachInterrupt(digitalPinToInterrupt(2), PULSO_EVENTO, RISING); //Inicia interrupção no pino 2, tendo como leitura a ida de Low a High
delay(1000); // Esse tempo evita problemas com as funções 
}

void loop() 
{

  // Armazena duas variáveis que seram modificadas no meio da rotação e serão utilizadas para fazer as contas
  Tempo_ciclo_ant = Tempo_ant_med;
  Tempo_Total = micros();
  Ciclos_atuais = Num_ciclos;

  Periodo_medio = Tempo_total/Ciclos_atuais;

  // Em caso de ocorrer algum bug na medição da variável Temp_ciclo_ant este if o corrige
  if(Tempo_Total < Tempo_ciclo_ant){
    Tempo_ciclo_ant = Tempo_Total;
  }

  
  // Se o tempo entre a última rotação e o tempo atual for maior que x este if transforma a frequencia em 0 para evitar de mandar informação falsa em caso de o motor estar parado
  // Também evita problemas caso o sistema de medição for ligado e o motor esta parado 

  if( Tempo_Total - Tempo_ciclo_ant > 1000000||Ciclos_atuais == 0) {
    
    Frequencia_sem_trat = 0;
  }
  else {  
    // Calcula a frequencia
    Frequencia_sem_trat = 1000000/Periodo_entre_rot_atual; 
  
    

  
  }
  
  // Transforma Hertz em RPM
  RPM = Frequencia_sem_trat * 60;

  Serial.print(RPM);

  delay(500);
}

void PULSO_EVENTO()// Calcula o periodo da ultima rotação
{
  Periodo_entre_rot_atual = micros() - Tempo_ant_med; // Calcula o periodo da ultima rotação

  Tempo_ant_med = micros(); // Armazena micros para ser comparado na próxima interrupção

  Num_ciclos++; //Incrementa o numero de rotações
  
}
