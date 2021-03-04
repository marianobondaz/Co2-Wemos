#include <Arduino.h>

int a_index                           = 0;
uint16_t eco2, etvoc, errstat, raw, interval_read;
int values_eco2[10];                   
bool stateAirGood                     = true;
int values10Length = sizeof(values_eco2) / sizeof(int);
bool gReverseDirection = false;

SimpleTimer      Timer1;
SimpleTimer      Timer2;
SimpleTimer      Timer3;

CCS811           ccs811(D3);              // nWAKE on D3

// Forward declarations of an array of cpt-city gradient palettes, and 
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );


