/* Codigo Para envio HTPP

Modelo: 
  
  GET: 52.201.215.36:5000/getIST
  POST: 52.201.215.36:5000/postIST

  ESTRUTURA PRA ENVIO DO DADO

{
    "mac": "00DDFF00DD",
    "identificador": "DI0",
    "valor": 1,
    "data": "2023-03-22 12:40:00"
}


*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>
#include <SPIFFS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SSID       "Talita 2.4G"
#define password   "91272520"
#define API_POST   "http://52.201.215.36:5000/postIST"
#define API_GET    "http://52.201.215.36:5000/getIST"
#define DI0         10
#define DI1         11
#define flag        14

// ---- Variaveis Globais ----

long int aux_time           = 0;
long int Time_envio         = 10000;
String                      mac_device;
boolean                     Sinal;
String                      Pacote_Json;


// ---- Instancias de Lib's ----

WiFiClient                  client;
HTTPClient                  http;
DynamicJsonDocument         jsonConfig(1024);
WiFiUDP                     ntpUDP;
NTPClient                   TimeClient(ntpUDP);

// ---- Funções ----

void Timer_Init()
{
  TimeClient.begin(); //Inicia cliente NTP/udp
  TimeClient.setTimeOffset(-3*3600); //Fuso-horário local
  TimeClient.forceUpdate(); //Atualiza data e hora
}

String FormatDate_Time(String valor) 
{ 
//Formato desejado: 2023-03-22 12:40:00, como chega: 2023-03-26T02:52:43Z

  String strFormatted = valor.substring(0, valor.indexOf("T")) + " " + valor.substring(valor.indexOf("T") + 1, valor.indexOf("Z"));
  return strFormatted;
}

void Config_pacote(int Pino_Leitura)
{ 
  String aux;

  if (Pino_Leitura == DI0)
  {
    aux = "DI0";
  }
  else if (Pino_Leitura == DI1)
  {
    aux = "DI1";
  }

  String date_time = FormatDate_Time(TimeClient.getFormattedDate());

  jsonConfig  ["mac"]           = mac_device;
  jsonConfig  ["identificador"] = aux;
  jsonConfig  ["valor"]         = String(digitalRead(DI0));
  jsonConfig  ["data"]          = date_time; 

  serializeJson(jsonConfig,Pacote_Json);
}

void Wifi_init()
{
  pinMode(flag,OUTPUT);

  WiFi.begin(SSID,password);
  
  Serial.println("Conectando");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(flag,!digitalRead(flag));
  }

  Serial.println("");
  Serial.print("Conectado na Rede: ");
  Serial.println(SSID);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Mac do Dispositivo: ");
  Serial.println(WiFi.macAddress());
  mac_device = WiFi.macAddress();
  digitalWrite(flag,HIGH);

  
}

void Enviar_dado()
{
  if (millis() - aux_time > Time_envio)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      http.begin(client,API_POST);
      http.addHeader("Content-Type", "application/json");
      int http_feddback = http.POST(Pacote_Json);
      Serial.print("HTTP Response code: ");
      Serial.println(http_feddback);

    }
    else
    {
      Serial.println("Wifi Desconectado");
    }
    aux_time = millis();
    Pacote_Json = "";
  }
  
}

void setup() 
{
  Serial.begin(9600);

  Serial.println("");
  Serial.println("Sistema iniciado.");
  
  Wifi_init();
  Timer_Init();
  
  

}

void loop() 
{
  Config_pacote(DI0);
  Serial.println(Pacote_Json);
  //Enviar_dado();
  delay(20000);


}