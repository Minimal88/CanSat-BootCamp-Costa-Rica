// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>
#define __AVR_ATmega328P__

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 433.0

#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
#endif


// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission
void setup() 
{
  /*INICIACILIZACION DE PUERTO SERIAL*/ //TODO: quitar antes del lanzamiento
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
  
  /*INICIALIZACION DE PINES RADIO*/
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  /*RESET MANAL DEL RADIO*/
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);  
  /*CONFIGURACION DEL RADIO*/
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed"); //TODO: quitar antes del lanzamiento
    while (1);
  }
  Serial.println("RFM69 radio init OK!");   //TODO: quitar antes del lanzamiento  
  if (!rf69.setFrequency(RF69_FREQ)) {      //Configuracion de la frecuencia
    Serial.println("setFrequency failed");  //TODO: quitar antes del lanzamiento
  }
  rf69.setTxPower(20, true);                //Configuracion de la potencia 
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);               //Configuracion de la encriptacion  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");  //TODO: quitar antes del lanzamient
}



void loop() {
  delay(1000);  // Wait 1 second between transmits, could also 'sleep' here!

  char radiopacket[20] = "Hello World #";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  
  // Send a message!
  rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
  Serial.println("sended entrying to wait");
  rf69.waitPacketSent();
  Serial.println("waited done!");
  // Now wait for a reply
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(100))  { 
    // Should be a reply message for us now   
    if (rf69.recv(buf, &len)) {
      Serial.print("Got a reply: ");
      Serial.println((char*)buf);      
    } else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No reply, is another RFM69 listening?");
  }
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
