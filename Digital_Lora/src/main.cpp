#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
//#include <SSD1306.h>


// --- Definicoes De Pinos ---
#define     PIN_STATUS      36 //DIO-1
#define     PIN_CONT        37 //DIO-2
#define     SDA             4
#define     SCL             15
#define     Reset           16
#define     Adress          0x03
#define     flag            2
#define     Lora_SS         18
#define     Lora_Reset      14
#define     Lora_DIO0       26
#define     Lora_DIO1       33
#define     Lora_DIO2       32


// --- Inicialização de Lib's ---

//SSD1306 display(0x03,4,15);


// --- Variáveis Auxiliares ---
static const PROGMEM u1_t NWKSKEY[16] = {0x24, 0x4B, 0x68, 0x63, 0x35, 0xBB, 0xC1, 0x7D, 0x01, 0x6B, 0x8A, 0xF0, 0x27, 0x29, 0xC9, 0xF6}; //ok 
static const PROGMEM u1_t APPSKEY[16] = {0x9B, 0xEB, 0xE8, 0x66, 0x5B, 0x5D, 0xE7, 0x66, 0x4A, 0xAB, 0xF4, 0x85, 0x58, 0xAF, 0x39, 0x3E};//ok 
static const u4_t DEVADDR = 0x260A6612; //ok

int teste = 0;

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t mydata[] = "teste" ; //Dado a ser enviado
static osjob_t sendjob; //

const unsigned TX_INTERVAL = 10;

bool status_atual = 1, status_ultimo = 1, state_atual_contangem = 1, state_ultimo_contangem = 1;
unsigned long cont = 0;

// --- Mapeamento de Hardware ---

const lmic_pinmap lmic_pins = {
  .nss = Lora_SS,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = Lora_Reset,
  .dio = {Lora_DIO0, Lora_DIO1, Lora_DIO2}
};

// --- Funcoes Auxiliares ---
void onEvent(ev_t ev){
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK) Serial.println(F("ACK recebido!"));
            if (LMIC.dataLen){
              Serial.print(F("Recebido: "));
              Serial.println(LMIC.dataLen);
              Serial.println(F("Bytes do PAYLOAD"));
            }
            // Schedule next transmission
           // os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            //data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}

void do_send(osjob_t* j)
{
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, Pacote ainda não enviado"));
  } else
  {
    LMIC_setTxData2(1,mydata, sizeof(mydata)-1, 0);
    Serial.println(F("Pacote na Fila"));

  }
  
}

void setup()
 {
  Serial.begin(9600);
  Serial.println("Iniciando...");

  pinMode(PIN_STATUS, INPUT);
  pinMode(PIN_CONT, INPUT);

  os_init();
  LMIC_reset();

  #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
  #else

    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);

  #endif

   #if defined(CFG_eu868)
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    
    #elif defined(CFG_us915)
    LMIC_selectSubBand(1);

    #endif
    //Disable link check validation
    LMIC_setLinkCheckMode(0);
    LMIC.dn2Dr = DR_SF9;
    //Set data rate and transmit power (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);
    do_send(&sendjob);
}

void loop() 
{
      unsigned long now;
    now = millis();
    if ((now & 512) != 0) {
      digitalWrite(flag, HIGH);
    }
    else {
      digitalWrite(flag, LOW);
    }

    os_runloop_once();
}