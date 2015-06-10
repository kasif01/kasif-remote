
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

// Used to control whether this node is sending or receiving
bool role = 1;

void setup() {
  Serial.begin(9600);

  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);

  // Open a writing and reading pipe on each radio, with opposite addresses

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);


  // Start the radio listening for data
  radio.startListening();
}

int command = 0;
void loop() {

  sendCommand(command++);

  // Try again 1s later
  delay(1000);

}

void sendCommand(int command) {
  radio.stopListening();

  Serial.println(F("Now sending"));

  if (!radio.write( &command, sizeof(int) )) {
    Serial.println(F("failed"));
  }

  radio.startListening();                                    // Now, continue listening

  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

  while ( ! radio.available() ) {                            // While nothing is received
    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  }

  if ( timeout ) {                                            // Describe the results
    Serial.println(F("Failed, response timed out."));
  } else {
    int response;                                 // Grab the response, compare, and send to debugging spew
    radio.read( &response, sizeof(int) );

    if (command == response) {
      Serial.println(F("Ack received"));
    } else {
      Serial.println(F("NACK received"));
    }
  }
}