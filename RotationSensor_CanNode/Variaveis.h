// Grava o Periodo mais recente
volatile unsigned long Periodo_entre_rot_atual;

// É a variável que guarda o tempo que ocorreu a última rotação
volatile unsigned long Tempo_ant_med;

// Armazena o numero de ciclos do motor
volatile unsigned int Num_ciclos = 0; 

// Grava o periodo médio durante o uso do motor
unsigned long Periodo_medio; 

// É a variável que guarda o Tempo_ant_medido para ser usado no void loop, 
// criada devido o problema da interrupção e da perca de dados devido à ela
unsigned long Tempo_ciclo_ant; 

// Armazena a func micros para fazer um comparativo com Temp_ciclo_ant
unsigned long Tempo_total;

// É onde se calcula a frequencia em hertz
unsigned long Frequencia_sem_trat;

// Variável que armazena o numero de rotações por minuto
unsigned long RPM;

// É a variável que guarda o Num_ciclos para ser usado no void loop, 
// criada devido o problema da interrupção e da perca de dados devido à ela
unsigned int Ciclos_atuais = 0; 

unsigned long Tempo_Total;