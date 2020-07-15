// Associa cada posição do Vetor BMVValues a um nome em BMVLabel
static const int V = 0;
static const int VS = 1;
static const int I = 2;
static const int CE = 3;
static const int SOC = 4;
static const int TTG = 5;

// Indica se a String recebida do BMV pela porta Serial está completa
boolean BMVStringComplete = false;

// Armanzena a quatidade de bytes lidos pela porta Serial
int BMVReceivedBytes = 0;

// Armazena a soma de todos os dados recebidos pela porta Serial para conferir a integridade dos dados
int checksum = 0;

// Array que armazena os dados lidos pela porta Serial
char BMVSerialString[140]="";

// Quantidade de dados lidos pela porta Serial
const int BMVValSIZE = 6;

// Array que armazena os dados do BMV depois de processados
float BMVValues[BMVValSIZE];

// Array que contém os nomes de cada dado contido em BMVValues
String BMVlabel[BMVValSIZE]={"V","VS","I","CE","SOC","TTG"};

// Tempo entre dois envios de informação para o CAN Bus
unsigned int interruptCounter = 0;

// Define os pinos onde o Display LCD está conectado
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
