#include <stdio.h>
#include <stdlib.h>
#include "digital_pins.h"
#include "coms.h"
#include "payload.h"



/*DEFINICION DE VARIABLES GLOBALES*/
const char *cansatName = "CanSatCR";
int camara_status = 0;      //Estado de la camara> 0:No graba, 1:Graba
DHT dht(DHTPIN, DHTTYPE);   //Inincializa el sensor de temperatura y humedad
uint32_t timer = millis();  //Para GPS
int16_t packetnum = 0;      //Contador de paquetes enviados
float h,t,f,hif,hic;        //Variables para temperatura y humedad



void toggle_video(bool state) {
  //Inserte su codigo aqui  2
}

void obtener_datos_sensor(){
  //Inserte su codigo aqui 1
}



void setup()
{
  /*INICIACILIZACION DE PUERTO SERIAL*/ //TODO: quitar antes del lanzamiento
  Serial.begin(115200);  

  /*CONFIGURACION DEL RADIO*/
  pinMode(RFM69_RST, OUTPUT);               //Definicion del pin de reset
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(RFM69_RST, HIGH); delay(10); //Reset Manual del radio
  digitalWrite(RFM69_RST, LOW); delay(10);
  if (!rf69.init()) {                       //Inicializacion del radio
    Serial.println("RFM69 radio init failed"); //TODO: quitar antes del lanzamiento
    while (1);
  }
  Serial.println("RFM69 radio init OK!");   //TODO: quitar antes del lanzamiento
  if (!rf69.setFrequency(RF69_FREQ)) {      //Configuracion de la frecuencia
    Serial.println("setFrequency failed");  //TODO: quitar antes del lanzamiento
  }
  rf69.setTxPower(20, true);                //Configuracion de la potencia
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);               //Configuracion de la encriptacion
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");  //TODO: quitar antes del lanzamient


  /*CONFIGURACION DEL SENSOR DE TEMP Y HUMEDAD*/
  dht.begin();    //TODO: Manejar error


  /*CONFIGURACION DE LA CAMRA*/
  pinMode(trig, OUTPUT);
  digitalWrite(trig, HIGH);


  /*CONFIGURACION DEL GPS*/  
  Serial.println("Cansat Mission Software!");
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);   //Modo de NMEA a RMC:minimo recomendado  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);      //Tasa de actualización a 1 Hz
  GPS.sendCommand(PGCMD_ANTENNA);                 //Configuracion de antena interna
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);// Ask for firmware version
}







void loop() {
  /*OBTENCION DATOS MISION TEMP Y HUMEDAD*/
  obtener_datos_sensor();


  /*OBTENCION DATOS MISION GPS*/  
  char c = GPS.read();        //Lee los datos del GPS
  if (GPSECHO)
    if (c) Serial.print(c);   //TODO: quitar antes del lanzamiento
  if (GPS.newNMEAreceived()) {  //Recibio un dato    
    if (!GPS.parse(GPS.lastNMEA()))
      return; 
  }
  
  if (timer > millis()) timer = millis(); //Resetea el contador de tiempo si ya se paso del limite


  
  if (millis() - timer > 3000) {  //Cada 3 segundos envía los datos por COMS
    timer = millis();                     //Resetea el contador de tiempo
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.print(GPS.milliseconds);
    Serial.print(" -- Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    float latitude, longitude, altitude;
    if (GPS.fix) {
      latitude = GPS.latitudeDegrees;      
      longitude = GPS.longitudeDegrees;
      altitude = GPS.altitude;
      Serial.print("Lat,Long: ");
      Serial.print(latitude, 6); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(longitude, 6); Serial.println(GPS.lon);      
      Serial.print("Altitude: "); Serial.println(altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }



    /*ENVIAR TELEMETRIA Y DATOS MISION*/    
    char radiopacket[50];
    char *fhic, *fhif;
    
    char longitudeStr[10]="";      //Variable auxiliar GPS Latitude
    dtostrf(longitude,2,6,longitudeStr);
    
    char latitudeStr[10] ="";       //Variable auxiliar GPS Longitud
    dtostrf(latitude,2,6,latitudeStr);
        
    sprintf(radiopacket, "%s#%i,%i,%i,%i,%i,%s,%s,%i,%i,%i,", cansatName, packetnum++, (int) h, (int) t, (int) hic, (int) hif, latitudeStr, longitudeStr, (int) altitude, camara_status, (int) rf69.lastRssi());

    Serial.println(radiopacket);                      //TODO: quitar antes del lanzamiento

    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));     //Envia el beacon por RF
        
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf69.waitAvailableTimeout(500))  {
      if (rf69.recv(buf, &len)) {       // Revisa si hay respuesta
        Serial.print("Reply: ");        //TODO: quitar antes del lanzamiento
        Serial.println((char*)buf);     //TODO: quitar antes del lanzamiento
        Serial.print("RSSI: ");         //TODO: quitar antes del lanzamiento
        Serial.println(rf69.lastRssi(), DEC);     //TODO: quitar antes del lanzamiento

        //Inserte su codigo aqui 3

      } else {
        //Serial.println("Receive failed");   //TODO: quitar antes del lanzamiento
      }
    } else {
      //Serial.println("No reply!!");         //TODO: quitar antes del lanzamiento
    }

    free(radiopacket);
  }
}


