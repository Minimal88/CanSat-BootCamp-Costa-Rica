/*INCLUDES Y DEFINITIONS MISION COMS*/
#include <SPI.h>
#include <RH_RF69.h>
#define RF69_FREQ 433.0
RH_RF69 rf69(RFM69_CS, RFM69_INT);  // Singleton instance of the radio driver
#define BEACON_VERF_CODE        67      // 'C'