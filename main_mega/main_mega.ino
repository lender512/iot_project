
/*  Getting_BPM_to_Monitor prints the BPM to the Serial Monitor, using the least lines of code and PulseSensor Library.
 *  Tutorial Webpage: https://pulsesensor.com/pages/getting-advanced
 *
--------Use This Sketch To------------------------------------------
1) Displays user's live and changing BPM, Beats Per Minute, in Arduino's native Serial Monitor.
2) Print: "♥  A HeartBeat Happened !" when a beat is detected, live.
2) Learn about using a PulseSensor Library "Object".
4) Blinks the builtin LED with user's Heartbeat.
--------------------------------------------------------------------*/

#define USE_ARDUINO_INTERRUPTS true // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>  // Includes the PulseSensorPlayground Library.
#include "process_and_send.h"

//  Variables
const int PulseWire = A0;    // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int endoderPin = A1;    // the number of the encoder pin
const int LED = LED_BUILTIN; // The on-board Arduino LED, close to PIN 13.
int Threshold = 300;         // Determine which Signal to "count as a beat" and which to ignore.
                             // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                             // Otherwise leave the default "550" value.

PulseSensorPlayground pulseSensor; // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

void setup()
{

  Serial.begin(9600); // For Serial Monitor

  pinMode(endoderPin, INPUT);
  // Configure the PulseSensor object, by assigning our variables to it.
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED); // auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin())
  {
    Serial.println("We created a pulseSensor Object !"); // This prints one time at Arduino power-up,  or on Arduino reset.
  }
}

void loop()
{
  
  if (pulseSensor.sawStartOfBeat())
  { 
    int const endoValu = analogRead(endoderPin);
    int const myBPM = pulseSensor.getBeatsPerMinute();
//    Serial.println(myBPM);
    unsigned long const relative_time = millis();
    add_data(0, (float)myBPM, (float)endoValu, (float) relative_time);
  }
  delay(100); // considered best practice in a simple sketch.
}
