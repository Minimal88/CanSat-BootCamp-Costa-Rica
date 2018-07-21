/*INCLUDES Y DEFINITIONS MISION GPS*/
#include <Adafruit_GPS.h> // what's the name of the hardware serial port?
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);// Connect to the GPS on the hardware port
#define GPSECHO false

/*INCLUDES Y DEFINITIONS MISION TEMP Y HUMEDAD*/
#include "DHT.h"
#define DHTTYPE DHT11     // Modelo del chip: DHT 11

