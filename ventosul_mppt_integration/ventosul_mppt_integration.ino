#include <SoftwareSerial.h>

// Habilita comunicação entre MPPT e Arduino, enquanto a conexão Arduino-PC está na outra porta serial
SoftwareSerial myserial(10, 11); // RX, TX

// 6 bytes de inicialização seguidos por 6 bytes de sincronização
uint8_t start[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
                    0xEB, 0x90, 0xEB, 0x90, 0xEB, 0x90 };

// Variável para a ID do MPPT
uint8_t id = 0x16;

// Comando para seleção do modo de operação
uint8_t cmd[] = { 0xA0, 0x00, 0xB1, 0xA7, 0x7F };
// Opção 1: monitoramento em tempo real
// Comando: { 0xA0, 0x00, CRCH, CRCL, 0x7F }
// Exemplo: { 0xA0, 0x00, 0xB1, 0xA7, 0x7F }
// 0xA0: para monitoramento em tempo real
// 0x00: indica o início do ciclo de conferência da integridade da informação
// 0xB1 e 0xA7: CRC code (cyclic redundancy check code, 2 byte: CRCHigh, CRCLow)
// 0x7F: indica o fim do ciclo de conferência da integridade da informação
// Retorno: { 0xA0, 0x00, strings de dados , CRCH, CRCL, 0x7F }

// Opção 2: controle manual do switch de carga, executado pelo computador
// Comando: { 0xAA, 0x01, control switch command, CRCH, CRCL, 0x7F }
// 0xAA: controle do switch de carga emitido pelo PC
// 0x01: indica o início do ciclo de conferência da integridade da informação
// Envia o comando para o load switch:
// = 1, Load ON
// = 0, Load OFF
// 0xB1 e 0xA7: CRC code (cyclic redundancy check code, 2 byte: CRCHigh, CRCLow)
// 0x7F: indica o fim do ciclo de conferência da integridade da informação
// Retorno: { 0xAA, 0x01, load switch state , CRCH, CRCL, 0x7F }
// O load switch state pode ser:
// = 1, Load ON
// = 0, Load OFF

// Opção 3: computador enviando dados de controle
// Comando: { 0xAD, 0x00, control strings, CRCH, CRCL, 0x7F }
// 0xAD: controle do MPPT a partir do computador
// 0x00: indica o início do ciclo de conferência da integridade da informação
// strings de controle
// 0xB1 e 0xA7: CRC code (cyclic redundancy check code, 2 byte: CRCHigh, CRCLow)
// 0x7F: indica o fim do ciclo de conferência da integridade da informação
// Retorno: { 0xAD, 0x00, CRCH, CRCL, 0x7F }

// Variável de buffer para o recebimento das informações do MPPT
uint8_t buff[128];

void setup() 
{
  // Inicializa a porta serial que comunica o Arduino com o PC (RX0 e TX0)
  Serial.begin(57600);

  // Inicializa a porta serial que comunica o Arduino com o MPPT (RX1 e TX1)
  myserial.begin(9600);
  // Em caso de erro, tentar usar o baudrate de 57600
}

  // Função para a conversão do buffer em formato de float
  // e.g. para leitura de tensão e temperatura da bateria
float to_float(uint8_t* buffer, int offset)
{
  unsigned short full = buffer[offset+1] << 8 | buff[offset];

  return full / 100.0;
}

void loop() 
{
  Serial.println("Reading from Tracer");

  // Envia o comando para o MPPT (ver as 3 opções de comando, embaixo da declaração da variável cmd)
  // OBS: todo comando enviado ao MPPT (variavel "cmd") deve ser precedido pelos start bytes e pelo id (variaveis "start" e "id")
  myserial.write(start, sizeof(start));
  myserial.write(id);
  myserial.write(cmd, sizeof(cmd));

  int read = 0;

  for (int i = 0; i < 255; i++){
    // Retorna o número de bytes disponíveis para leitura na porta serial. 
    // Esses são dados que já chegaram e foram guardados no buffer de recebimento (que armazena 64 bytes).
    if (myserial.available()) {
      buff[read] = myserial.read();
      read++;
    }
  }

  Serial.print("Read ");
  // Indica o número de bytes lidos
  Serial.print(read);
  Serial.println(" bytes");

  for (int i = 0; i < read; i++){
      // Imprime cada byte no formato hexadecimal    
      Serial.print(buff[i], HEX);
      Serial.print(" ");
  }

  Serial.println();

  float battery = to_float(buff, 9);
  float pv = to_float(buff, 11);
  //13-14 reserved
  float load_current = to_float(buff, 15);
  float over_discharge = to_float(buff, 17);
  float battery_max = to_float(buff, 19);
  // 21 load on/off
  // 22 overload yes/no
  // 23 load short yes/no
  // 24 reserved
  // 25 battery overload
  // 26 over discharge yes/no
  uint8_t full = buff[27];
  uint8_t charging = buff[28];
  int8_t battery_temp = buff[29] - 30;
  float charge_current = to_float(buff, 30);

  Serial.print("Load is ");
  Serial.println(buff[21] ? "on" : "off");

  Serial.print("Load current: ");
  Serial.println(load_current);

  Serial.print("Battery level: ");
  Serial.print(battery);
  Serial.print("/");
  Serial.println(battery_max);

  Serial.print("Battery full: ");
  Serial.println(full ? "yes " : "no" );

  Serial.print("Battery temperature: ");
  Serial.println(battery_temp);

  Serial.print("PV voltage: ");
  Serial.println(pv);

  Serial.print("Charging: ");
  Serial.println(charging ? "yes" : "no" );

  Serial.print("Charge current: ");
  Serial.println(charge_current);

  delay(1000);
}
