#include <Arduino.h>
#include <FreqCount.h>

#define GATE_INTERVAL 1000000 // milliseconds
#define SAMPLE_SIZE 200 // wanna get 200 periods each cycle

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
FreqCount.begin(GateInterval);
- Begin frequency counting. 
- GateInterval is the time in milliseconds for each measurement. 
- Using 1000 provides direct frequency output without mulitplying or dividing by a scale factor.

FreqCount.available();
- Returns true when a new measurement is available.
- Only a single measurement is buffered, so it must be read before the next gating interval.

FreqCount.read();
- Returns the most recent measurement, an unsigned long containing
- the number of rising edges seen within the gating interval. There is no delay 
- between gating intervals, so for example a 1000.5 Hz input (and perfectly accurate crystal osciallator) will alternate between 1000 and 1001 when used with a 1 second gate interval.

FreqCount.end();
Stop frequency counting. PWM (analogWrite) functionality may be used again.

FreqCount measures the number of cycles that occur during a fixed "gate interval" time. 
This works well for relatively high frequencies, because many cycles are likely to be counted during 
gate interval. At lower frequencies, very few cycles are counted, giving limited resolution.

FreqMeasure measures the elapsed time during a single cycle. This works well for relatively low frequencies, 
because a substantial time elapses. At higher frequencies, the short time can only be measured at the processor's 
clock speed, which results in limited resolution.

*/

void measure();
void update_output();
double calcualte_deposition_rate();
double calculate_deposition_thickness();

void setup() {
  Serial.begin(57600);
  
  delay(2000);
  analogWriteFrequency(8, 50000000);  // test jumper 11 to 25
  analogWrite(8, 128);
  
  // measures the number of cycles that occur during a fixed "gate interval" time
  FreqCount.begin(1000000);  //Time in microseconds
}

void loop() {
  if (FreqCount.available()) {
    unsigned long count = FreqCount.read();
    Serial.println(count);
  }
}