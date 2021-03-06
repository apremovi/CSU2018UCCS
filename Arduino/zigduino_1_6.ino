// CSU Circuit Breakers UCCS Senior Design Team
// Fall 2018 
// Modifications of RFM69HCW code from Sparkfun
// to create meter simulator aka Zigduino

// Version 1.5
// 4 December 2018
//transmits volts,(blank space where amps goes),watts despite not showing
//on the serial monitor...

// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

//Timer function
//This timer also overcomes the 50 day millis() limit problem
//this setup will allow 10 timers, 0-9, 
//replace "10" with a higher or lower number if you need more or less timers
//the next two lines must be added to any program using this code
unsigned long timer[10];
byte timerState[10];

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      1   // My node ID
#define TONODEID      2   // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:

//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "CSUCIRCUITBREAKR" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED           9 // LED positive pin
#define GND           8 // LED ground pin

// Create a library object for our RFM69HCW module:

RFM69 radio;

double mwh_value = 0; //changed to same data type as dtostrf


double seed = 325; //changed to same data type as dtostrf

void setup()
{
  // Open a serial port so we can send keystrokes to the module:

  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  // Set up the indicator LED (optional):

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(GND,OUTPUT);
  digitalWrite(GND,LOW);

  // Initialize the RFM69HCW:

  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:

  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[62];
  

  // Generate the char string to be sent:
  //seed = 325
  //mwh_value=seed++;

  char sendvolt[6];
  char sendamp[6];
  char sendwatt[7];
  static int sendlength = 30;
  
 //random values are taken 1 std dev up and down plus the mean out of roughly 1000 values tested.
  int volt = random(11752, 11943); 
  char v_cstr[6];
  itoa(volt, v_cstr, 10); //last one is radix
  char volt_buffer[6];
  strcpy(volt_buffer, v_cstr);
  volt_buffer[3] = '.';
  strcpy(&volt_buffer[3 + 1], &v_cstr[3]);
  
  int amp = random(14393, 19235);
  char a_cstr[6];
  itoa(amp, a_cstr, 10);
  char amp_buffer[6];
  strcpy(amp_buffer, a_cstr);
  amp_buffer[2] = '.';
  strcpy(&amp_buffer[2 + 1], &a_cstr[2]);

  double watt = ((double) volt/100)*((double) amp/1000);
  char w_cstr[7];
  dtostrf(watt, 7, 2, w_cstr); // width, precision
   
   strcat(sendbuffer,volt_buffer);
   strcat(sendbuffer,",");
   strcat(sendbuffer,amp_buffer);
   strcat(sendbuffer,",");
   strcat(sendbuffer,w_cstr);
   Serial.println(sendbuffer);
 
  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).

  // If there is any serial input, add it to the buffer:
  
    // If the input is a carriage return, or the buffer is full:

    //if ((input == '\r') || (sendlength == 61)) // CR or buffer full
    //{
      // Send the packet!


      //Serial.print("sending to node ");
      //Serial.print(TONODEID, DEC);
      //Serial.print(", message [");
    
      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():

      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      }

      // If you don't need acknowledgements, just use send():

      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }

      sendlength = 30; // reset the packet adjusted to 8 from 0, before when it was zero the received message would be blank
      Blink(LED,10);
    //}
  

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    Blink(LED,10);
  }
   //Print the information
    // Serial.print("Current usage is ");
    // Serial.print(mwh_value);
    // Serial.print(" mWH \r");
     delay(4000); //increased delay to 4 sec
}


void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
