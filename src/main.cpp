#include <SPI.h>
#include <vector>
#include <Arduino.h>
#include <FreqCount.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // using my own, so no parts needed

#define ARRAY_SIZE 100 // size of array we wanna keep track of
#define F_DENSE 2.33f // film density (g/cm^3) => figure this out
#define Q_DENSE 2.648f // desntiy (g/cm^3)
#define P_AREA 1.53938f // piezo-active area (cm^2)
#define SHEER_MOD 2.847e11 // Sheer modulus in g*cm^(-1)*s^(-2)
#define RES_FREQ 5.000e6 // fundamental mode(Hz)
#define GATE_INTERVAL 100 // (milliseconds)

/* FreqCount - Example with serial output
 * http://www.pjrc.com/teensy/td_libs_FreqCount.html
 *
 * This example code is in the public domain.
 *
 * For the Teensy 4.0 pin 9 is used to measure the freq
 * FreqCount.begin time is in microseconds vs milliseconds
 * for other boards.
 * As a test the sketch is setup to output 50Mhz on pin 11 sopen
 * add a jumper from pin 8 to pin 9.
 */

/*
FreqCount.begin(GateInterval); Begin frequency counting. 
- GateInterval is the time in milliseconds for each measurement. 
- Using 1000 provides direct frequency output without mulitplying or dividing by a scale factor.

FreqCount.available();
- Returns true when a new measurement is available.
- Only a single measurement is buffered, so it must be read before the next gating interval.

FreqCount.read();
- Returns the most recent measurement, an unsigned long containing
- the number of rising edges seen within the gating interval. There is no delay 
- between gating intervals, so for example a 1000.5 Hz input 
- (and perfectly accurate crystal osciallator) will alternate between 1000 and 1001 
- when used with a 1 second gate interval.

FreqCount measures the number of cycles that occur during a fixed "gate interval" time. 
This works well for relatively high frequencies, because many cycles are likely to be counted during 
gate interval. At lower frequencies, very few cycles are counted, giving limited resolution.

*/

uint32_t prev = 0;

// (change in mass) * constant = (change in freq)
double constant;
double prev_thick = 0;
double total_thickness = 0;

std::vector<double> thick_array;

void measure();
void update_output();
double calculate_deposition_rate(double dth, double time);
double calculate_deposition_thickness(double df);

void setup() {
  thick_array.resize(ARRAY_SIZE, 0);
  constant = (-2.0f*RES_FREQ*RES_FREQ)/(P_AREA*pow(Q_DENSE*SHEER_MOD, 0.5f));
  Serial.begin(57600);
  delay(2000);
  
  // measures the number of cycles that occur during a fixed "gate interval" time
  FreqCount.begin(GATE_INTERVAL);  //Time in microseconds
}

void loop() {
  if (FreqCount.available()) {

    uint32_t freq = FreqCount.read();
    Serial.println(freq);

    double dfreq = prev - freq;
    double thick = calculate_deposition_thickness(dfreq);
    
    thick_array.erase(thick_array.begin());
    thick_array.push_back(thick);

    double dthick = thick - prev_thick;
    total_thickness += dthick;

    double rate = calculate_deposition_rate(dthick, 1000);
    prev = freq;


    Serial.println(thick);
    delay(100);
  }
}

double calculate_deposition_thickness(double df){

  // norm_freq = nom freq shift of that mode divided by its mode number 
  double norm_freq = 1.0f * df / 1.0f; // I think it's already normalized..
  double mass_change = norm_freq*constant;
  double thickness = mass_change * 1.0f / P_AREA * 0.01f / F_DENSE;
  return thickness;
}

double calculate_deposition_rate(double dth, double time){
  return 1.0f * dth / time;
}