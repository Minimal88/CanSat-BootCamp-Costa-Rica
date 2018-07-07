// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

/*INCLUDES Y DEFINITIONS MISION COMS*/
#include <SPI.h>
#include <RH_RF69.h>
#define __AVR_ATmega328P__
#define RF69_FREQ 433.0
#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // Definicion de pin
  #define RFM69_CS      4  // Definicion de pin
  #define RFM69_RST     2  // Definicion de pin
#endif
RH_RF69 rf69(RFM69_CS, RFM69_INT);  // Singleton instance of the radio driver
#define BEACON_VERF_CODE        67      // 'C'


/*INCLUDES Y DEFINITIONS MISION GPS*/
#include <Adafruit_GPS.h> // what's the name of the hardware serial port?
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);// Connect to the GPS on the hardware port
#define GPSECHO false
uint32_t timer = millis();


/*INCLUDES Y DEFINITIONS MISION TEMP Y HUMEDAD*/
#include "DHT.h"
#define DHTPIN 7          // Definicion de pin
#define DHTTYPE DHT11     // Modelo del chip: DHT 11
DHT dht(DHTPIN, DHTTYPE); // Inincializa el sensor de temperatura y humedad


/*DEFINITIONS CAMARA*/
int trig = 6;     //Definicion de pin





char *ftoa(char *a, float f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000}; 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}


void camera_video(bool state) {
  if (state){ //Si TRUE prende el video    
    digitalWrite(trig, LOW);   
  }else{
    digitalWrite(trig, HIGH);    
  }
  //delay(30000);   //Delay a modificar para cambiar a modo de foto o modo de caotura de video            
}



int16_t packetnum = 0;  // packet counter, we increment per xmission
void setup() 
{
  /*INICIACILIZACION DE PUERTO SERIAL*/ //TODO: quitar antes del lanzamiento
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
  
  /*CONFIGURACION DEL RADIO*/
  
  pinMode(RFM69_RST, OUTPUT);               //Definicion del pin de reset
  digitalWrite(RFM69_RST, LOW);     
  digitalWrite(RFM69_RST, HIGH);delay(10);  //Reset Manual del radio
  digitalWrite(RFM69_RST, LOW);delay(10);  
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
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);               //Configuracion de la encriptacion  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");  //TODO: quitar antes del lanzamient


  /*CONFIGURACION DEL SENSOR DE TEMP Y HUMEDAD*/
  dht.begin();

  /*CONFIGURACION DE LA CAMRA*/
  
  pinMode(trig, OUTPUT);         
  digitalWrite(trig, HIGH); 

  
  /*CONFIGURACION DEL GPS*/
  //Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);  
  GPSSerial.println(PMTK_Q_RELEASE);// Ask for firmware version


  
  
}











void loop() {
  //delay(10);  // Wait 1 second between transmits, could also 'sleep' here!

  /*OBTENCION DATOS MISION TEMP Y HUMEDAD*/
  
  float h = dht.readHumidity(); // Read temperature as Celsius (the default)  
  float t = dht.readTemperature(); // Read temperature as Fahrenheit (isFahrenheit = true)  
  float f = dht.readTemperature(true);  
  
  if (isnan(h) || isnan(t) || isnan(f)) { // Check if any reads failed and exit early (to try again).
    Serial.println("Failed to read from DHT sensor!");
    return;
  }  
  float hif = dht.computeHeatIndex(f, h); // Compute heat index in Fahrenheit (the default)  
  float hic = dht.computeHeatIndex(t, h, false);  // Compute heat index in Celsius (isFahreheit = false)
 
  /*Serial.print("H: ");
  Serial.print(h);
  Serial.print("\t T: ");
  Serial.print(t);
  Serial.print(" \t HI: ");
  Serial.print(hic);
  Serial.print(" \t HT: ");
  Serial.println(hif);*/




  /*OBTENCION DATOS MISION GPS*/
  
  //receive_gps();
  char c = GPS.read();  // read data from the GPS 
  // if you want to debug, this is a good time to do it!
  
  if (GPSECHO)    
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  //Serial.println("sdf1");
  if (GPS.newNMEAreceived()) {
    //Serial.println("sdf2");
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
  
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
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
    float latitude, longitude;
    if (GPS.fix) {
      latitude = GPS.latitudeDegrees;
      longitude = GPS.longitudeDegrees;
      Serial.print("Lat,Long: ");
      Serial.print(latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(longitude, 4); Serial.println(GPS.lon);
      //Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      //Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }

    

    /*ENVIAR TELEMETRIA Y DATOS MISION*/    
    char *cansatName= "CanSatCR";
    char radiopacket[25];
    char *fhic,*fhif;
    //ftoa(fhic, hic, 1);
    int latDec = latitude;
    int longDec = longitude;
    float tmpLat = latitude - latDec;
    float tmpLong = longitude - longDec;
    int latFrac = trunc(tmpLat * 10000);
    int longFrac = abs(trunc(tmpLong * 10000));
    
    sprintf(radiopacket,"%s#%i,%i,%i,%i,%i,%i.%i,%i.%i",cansatName,packetnum++,(int) h,(int) t,(int) hic,(int) hif,latDec,latFrac,longDec,longFrac);
    
  
    //char *ftoa(char *a, double f, int precision)
  
    
  
    //Serial.println("\nSending beacon: #,h,t,hic,hif");    //TODO: quitar antes del lanzamiento  
    Serial.println(radiopacket);                      //TODO: quitar antes del lanzamiento    
    
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));     //Envia el beacon por RF
    //Serial.println("Waiting for acknowledge...");        //TODO: quitar antes del lanzamiento  
    //rf69.waitPacketSent();              

    
    
    
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf69.waitAvailableTimeout(500))  {     
      if (rf69.recv(buf, &len)) {       // Revisa si hay respuesta
        //Serial.print("Reply: ");        //TODO: quitar antes del lanzamiento  
        //Serial.println((char*)buf);     //TODO: quitar antes del lanzamiento
        //Serial.print("RSSI: ");         //TODO: quitar antes del lanzamiento  
        //Serial.println(rf69.lastRssi(), DEC);     //TODO: quitar antes del lanzamiento  
  
        if(buf[0]!=BEACON_VERF_CODE){
          //Serial.println("Not a CMD!");        //TODO: quitar antes del lanzamiento          
          camera_video(false); //TODO: agregar apagar
        } 
        else{
          //Serial.println("Verified CMD!");        //TODO: quitar antes del lanzamiento  
          //Serial.println("Turning ON video mission");        //TODO: quitar antes del lanzamiento  
          camera_video(true); //TODO: agregar apagar  //TODO: Verificar bien cual comando es (CMD decode)
        }
  
        
      } else {
        //Serial.println("Receive failed");   //TODO: quitar antes del lanzamiento  
      }
    } else {
      //Serial.println("No reply!!");         //TODO: quitar antes del lanzamiento  
    }  


    free(radiopacket);
    




    
  }//end of if from 2 seconds
  

  



  
  




  

  


}


