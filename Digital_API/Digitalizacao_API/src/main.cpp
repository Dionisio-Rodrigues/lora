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
#include <TimeLib.h>

#define SSID       "Talita 2.4G"
#define password   "91272520"
#define API_POST   "http://52.201.215.36:5000/postIST"
#define API_GET    "http://52.201.215.36:5000/getIST"
#define DI0         10
#define DI1         11

// ---- Variaveis Globais ----

long int aux_time           = 0;
long int Time_envio         = 10000;
String                      mac_device;
boolean                     Sinal;
String                      data;
String                      Pacote_Json;
DynamicJsonDocument         jsonConfig(1024);

// ---- Instancias de Lib's ----

WiFiClient client;
HTTPClient http;

// ---- Funções ----

void Config_pacote(int Pino_Leitura)
{ 

  


  jsonConfig  ["mac"]           = mac_device;
  jsonConfig  ["identificador"] = Pino_Leitura;
  jsonConfig  ["valor"]         = String(digitalRead(DI0));
  jsonConfig  ["data"]          = data; 

  serializeJson(jsonConfig,Pacote_Json);
}

void Wifi_init()
{
  WiFi.begin(SSID,password);
  
  Serial.println("Conectando");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado na Rede: ");
  Serial.println(SSID);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Mac do Dispositivo: ");
  Serial.println(WiFi.macAddress());
  mac_device = WiFi.macAddress();

  
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
  
  

}

void loop() 
{
  Config_pacote(DI0);
  Enviar_dado();
  delay(20000);
   

}