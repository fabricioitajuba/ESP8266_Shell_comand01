/*
Shell Command utiizando o ESP8266
Autor: Fabrício Ribeiro
Data: 16/11/2024

Neste exemplo podemos conectar ao esp8266 utilizando o programa
Putty ou Curl

Para conectar utilizando o programa Putty:

- Conecte o putty a este servidor através da porta 123 usando Raw

Neste exemplo o client fica o tempo todo conectado ao servidor impedindo
de outros se conectarem

*/

#define DEBUG_ON

#include <ESP8266WiFi.h>

#define SSID "xxx"
#define PASSWD "xxx"
#define SOCK_PORT 123

#define led 2

WiFiServer sockServer(SOCK_PORT);

int StringCount;
String comandos[5];

void setup(){

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

#ifdef DEBUG_ON
  Serial.begin(9600);
#endif

  delay(1000);
  WiFi.begin(SSID,PASSWD);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }

#ifdef DEBUG_ON
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
#endif

  sockServer.begin(); //abre a porta 123
}

void loop(){

  WiFiClient client = sockServer.available();
    
  if(client){

    client.flush();

#ifdef DEBUG_ON        
    Serial.println("Client connected");
#endif

    client.print("#>>");
        
    while(client.connected()){

      while(client.available() > 0){

        String comand = client.readStringUntil('\n');
        client.flush();

#ifdef DEBUG_ON               
        Serial.println(comand);
#endif

        StringCount = 0;
              
              // Split the string into substrings
        while(comand.length() > 0){

          int index = comand.indexOf(' ');

          if(index == -1){
            comandos[StringCount++] = comand;
            break;
          }
          else{
            comandos[StringCount++] = comand.substring(0, index);
            comand = comand.substring(index+1);
          }
        }              

#ifdef DEBUG_ON                             
        // Show the resulting substrings
        for(int i = 0; i < StringCount; i++){

          Serial.print(i);
          Serial.print(": \"");
          Serial.print(comandos[i]);
          Serial.println("\"");
        }
#endif
        //Interpreta o comando, para comando de 1 argumento,
        //colocar "\r" no final.
        if(comandos[0] == "help\r"){
          client.println("Comandos:");
          client.println("on - Liga o led;");
          client.println("off - Desliga o led;");
          client.println("tempo_vezes - Pisca o led por n vezes com o tempo escolhido.");
          client.println("exit - Sair;");
        }
        else if(comandos[0] == "exit\r"){                
          client.stop(); //Sai do programa
        }               
        else if(comandos[0] == "on\r"){
          digitalWrite(led, LOW);
        }    
        else if(comandos[0] == "off\r"){
          digitalWrite(led, HIGH);
        }        
        else if(comandos[0] == "tempo_vezes"){
                
          int tempo = comandos[1].toInt();
          int vezes = comandos[2].toInt();

          do{
            digitalWrite(led, LOW);
            delay(tempo);
            digitalWrite(led, HIGH);
            delay(tempo);
            vezes--;
          }while(vezes != 0);

        }               
        else{
          client.println("Comando não reconhecido!");
        }              
              
          client.print("#>>");
        }
          delay(10);
        }
        client.stop(); //acabou a leitura dos dados. Finaliza o client.

#ifdef DEBUG_ON          
        Serial.println("Client disconnected");
#endif

    }
}