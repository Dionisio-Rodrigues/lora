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
#include<Arduino_JSON.h>
#include <Security.h>

// ---- Variaveis Globais ----

long int aux_time = 0;
long int Time_envio = 5000;
//char dado =;


// ---- Instancias de Lib's ----

WiFiClient client;
HTTPClient http;

// ---- Funções ----

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
  
}

void Enviar_dado()
{
  if (millis() - aux_time > Time_envio)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      http.begin(client,API_POST);
      http.addHeader("Content-Type", "text/plain");
      int http_feddback = http.POST("Hello, World!");
      Serial.print("HTTP Response code: ");
      Serial.println(http_feddback);

    }
    else
    {
      Serial.println("Wifi Desconectado");
    }
    aux_time = millis();
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
  Enviar_dado();
}