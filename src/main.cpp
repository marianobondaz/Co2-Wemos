/*
  ccs811basic.ino - Demo sketch printing results of the CCS811 digital gas sensor for monitoring indoor air quality from ams.
  Created by Maarten Pennings 2017 Dec 11

  Calidad de aire, buzzer, O-ring led indicador de estado del aire, alarma de ventilacion
  efectos difirentes 
  HSV animation rotary


  el color verde indica que esta en codidiociones el ambiente, si esta rojo indica que hay que ventilar,
  se puede usar un efecto como la velocidad para indicar
  la cantidad de etvoc y el color para eco2

  *conectar con MQTT , grafana , firebase, nodered


  las lecturas mejoran despues de 20min, segun el sensor


*/

#include <Arduino.h>
#include <SimpleTimer.h>
#include <SPI.h>
#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library
#include "FastLED.h"
#include "QuickMedianLib.h"



#define DATA_PIN    D7
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    24
#define BRIGHTNESS  100

CRGB leds[NUM_LEDS];

// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
#define SECONDS_PER_PALETTE           10

#define LEVEL_BAJO                    1000
#define LEVEL_MEDIUM                  1500
//#define LEVEL_ALTO                    5000

#define INTERVAL_READ_CCS811          1000
#define INTERVAL_CALCULE              10000
#define INTERVAL_SHOW                 20

#define COOLING                       55
#define SPARKING                      120

#include <init.h>
#include <function.cpp>

///////////////////////////////////////
void checkAirQuality()
{
  int result = QuickMedian<int>::GetMedian(values_eco2, values10Length); 
  
  Serial.print("result Quicmedia :");
  Serial.println(result);
  
  if (result < LEVEL_BAJO)
  {  
    stateAirGood = true;         
  }
  else if (result > LEVEL_BAJO && result < LEVEL_MEDIUM)
  {
    stateAirGood = false;
  }
  else if (result > LEVEL_MEDIUM )
  {
    stateAirGood = false;
    //ACTIVAR BUZZER
  }
  
  
  
}
void readCcs811()
{
  // Read  
  ccs811.read(&eco2,&etvoc,&errstat,&raw); 

  int elements = (sizeof(values_eco2)/sizeof(values_eco2[0]));
  Serial.print("elementos :");
  Serial.println(elements);


  if (a_index >= elements -1)
  {
    a_index = 0;
    
  }else
  {
    values_eco2 [a_index] = eco2; 
    a_index ++;
  }

  Serial.print("index: ");Serial.print(a_index);Serial.print("------->");Serial.println(values_eco2[a_index]);
  Serial.println("elementos: " + elements);

  // Print measurement results based on status
  if( errstat==CCS811_ERRSTAT_OK ) { 
    Serial.print("CCS811: ");
    Serial.print("eco2=");  Serial.print(eco2);     Serial.print(" ppm  ");
    Serial.print("etvoc="); Serial.print(etvoc);    Serial.print(" ppb  ");
    //Serial.print("raw6=");  Serial.print(raw/1024); Serial.print(" uA  "); 
    //Serial.print("raw10="); Serial.print(raw%1024); Serial.print(" ADC  ");
   // Serial.print("R="); Serial.print((1650*1000L/1023)*(raw%1024)/(raw/1024)); Serial.print(" ohm");
    Serial.println();
  } else if( errstat==CCS811_ERRSTAT_OK_NODATA ) {
    Serial.println("CCS811: waiting for (new) data");
  } else if( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
    Serial.println("CCS811: I2C error");
  } else {
    Serial.print("CCS811: errstat="); Serial.print(errstat,HEX); 
    Serial.print("="); Serial.println( ccs811.errstat_str(errstat) ); 
  }

}
void checkState(){
   if (stateAirGood)
  {
    EVERY_N_MILLISECONDS(40) {
      gTargetPalette = gGradientPalettes[ 1];
      nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
    }
    colorwaves( leds, NUM_LEDS, gCurrentPalette);
    //Serial.println(gCurrentPaletteNumber);  
  }
  else if (stateAirGood == false)
  {    
   EVERY_N_MILLISECONDS(40) {
      gTargetPalette = gGradientPalettes[ 4 ];
      nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
    }
    colorwaves( leds, NUM_LEDS, gCurrentPalette);
    //Serial.println(gCurrentPaletteNumber);  
  }
  
  FastLED.show();
  //FastLED.delay(20);
}
///////////////////////////////////////

#include <setup.cpp>
void setup() {
  
  // Enable serial
  Serial.begin(9600);
  // Enable I2C
  Wire.begin();  
  //setup de los modulos
  setupCcs811(); 
  setupFastLed();
  setupTimers();
  
}


void loop() {
  
  Timer1.run();
  Timer2.run();
  Timer3.run();// posiblemente usado para lalmar al las funciones fastled cada 20ms
     
}
