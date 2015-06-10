#include <SPI.h>
#include "RF24.h"
 
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

enum Commands { 
  FORWARD=8, 
  BACKWARD=2, 
  LEFT=4, 
  RIGHT=6, 
  CLAW_FW=18, 
  CLAW_BW=12, 
  WRIST_FW=28,
  WRIST_BW=22,
  ELBOW_FW=38,
  ELBOW_BW=32,
  RIM_FW=48,
  RIM_BW=42
}; 

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
 
 
void loop() { 
  sendCommand(FORWARD);
 
  delay(1000); 
  sendCommand(BACKWARD);
  
  delay(1000);
  sendCommand(99);
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
      Serial.print(F("Ack received : "));
      Serial.println(response);
    } else {
      Serial.print(F("NACK received :"));
      Serial.println(response);
    }
  }
}
