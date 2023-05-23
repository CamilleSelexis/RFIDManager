//Sketch to control the Magnets using the portenta in the electronics box, send the order by ethernet to the portenta
#include "Arduino.h"
#include "PCF8575.h"
#include "MFRC522_I2C.h"

#define _TIMERINTERRUPT_LOGLEVEL_     4

#include "Portenta_H7_TimerInterrupt.h"

#include <Wire.h>

#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <math.h>
#include <stdio.h>

using namespace rtos;

#include <stdint.h>

#define DEBUG 0
#define RST_PIN 13 //reset Pin for the portenta

volatile bool toggle0 = true;
volatile bool toggle1 = true;

#define TIMER0_INTERVAL_MS        10000
#define TIMER1_INTERVAL_MS        10000

// Init timer TIM15
Portenta_H7_Timer ITimer0(TIM15);
// Init  timer TIM16
Portenta_H7_Timer ITimer1(TIM16);

//reset function -- Call it to reset the arduino
void resetFunc(void) {
  unsigned long *registerAddr;
  registerAddr = (unsigned long *)0xE000ED0C; //Writes to the AIRCR register of the stm32h747 to software restet the arduino
  //It is a 32 bit register set bit 2 to request a reset and write 0x05FA to enable the write
  //See Arm® v7-M Architecture Reference Manual for more information
  *registerAddr = (unsigned long) 0x05FA0304;
}
const int LON = LOW; // Voltage level is inverted for the LED
const int LOFF = HIGH;
const int baud = 115200;
int addresses[128];
int cc = 0; //Current Chip - Selects the RFID chip
bool pcf = false; //bool to know if a pcf chip is connected
bool rfid = false; //bool to know if a rfid chip is connected
int nRFID = 0; //Number of RFID chip
// Uses default I2C pins -> SDA     SCL
//                Mega        20      21
//                Nano        23      24
//                Portenta    11      12

// Set i2c address
int adr_pcf = 0x20;
PCF8575 pcf8575(adr_pcf);
//The portenta seems to have an I2C at adr 24 ??
//PCF8575 pcf8575(0x24);

 //Ethernet related ---------------------
byte mac[] = {0xDE, 0x03, 0x33, 0x13, 0x59, 0x99};  //Mac adress

IPAddress ip(192,168,1,81);   //Adresse IP

EthernetServer server = EthernetServer(80);  // (port 80 is default for HTTP) 52 is the number of the lab

void setup()
{
  Serial.begin(baud);
  //while(!Serial);//Wait for the user to open the serial terminal
  digitalWrite(LEDG,LON);
  pinMode(RST_PIN,OUTPUT);
  digitalWrite(RST_PIN,LOW);
  delay(100);
  digitalWrite(RST_PIN,HIGH);
  //--------------------------I2C BUS INITIALIZATION-----------------------------------------------
  Wire.begin();//Start the I2C communications;
  int nDevices;
  nDevices = scan_i2c(addresses);
  Serial.print("nDevices = ");Serial.println(nDevices);
  //MFRC522_I2C mfrc522[nDevices] = {0}; //Create the array to contain the instances of MFRC522
  for(int i = 0; i<nDevices; i++){ //Create instances of pcf and rfid depending on what was found on i2c bus
    if(addresses[i] == adr_pcf){
      Serial.println("PCF8575 connected to the I2C Bus");
      Serial.println("Initialize the pcf8575");
      init_pcf8575(adr_pcf); //init the pcf8575 with its adr
      pcf = true;
    }
    else if(addresses[i] == 0x24){
      Serial.println("0x24 adress used");
    }
  }
  //----------------------ETHERNET INITIALIZATION------------------------------------------------
  Serial.println("Initialize the ethernet connection");
  //Ethernet setup
  Ethernet.begin(mac,ip);  //Start the Ethernet coms

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1500); // do nothing, no point running without Ethernet hardware
      Serial.println("No ethernet shield connected");
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }


  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print("Ethernet server connected. Server is at ");
  Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
  digitalWrite(LEDB,LON);

}

void loop()
{
  digitalWrite(LEDB,HIGH);
  delay(100);
   // listen for incoming clients
  EthernetClient client = server.available();
  if (client) { //Check that server.available returned a client
    Serial.println("Client connected");
    while(client.connected()) { //Stay in the loop for as long as the client wants
      if(client.available()){ //Returns true if there is still data to read
        //byte data = client.read(); //read the first byte of data
        /*String commandLine = "";
        while(client.available())
          commandLine += client.read();*/
        byte data[5];
        byte index;
        data[0] = client.read();
        //SSR CONTROL ----------------------------------------------------------------------------------------
        if(data[0] == 'P'){ //Check that first letter is P for PCF -> PXE or PXD PCF 1 Enable or PCF 1 Disable
          //Command for the PCF8575
          if(pcf){//Check that a pcf8575 is connected to the I2C bus
            data[1] = client.read(); //Second char is a number and represents which SSR should be used
            index = data[1]-'0';
            if(index>= 0 && index<8){
              data[2] = client.read();
              if(data[2] == 'E'){ // E for enable
                enable_SSR(index);
                Serial.print("Enable SSR");Serial.println(index);
                client.write(data,3);
                client.print("Enabled the SSR ");client.print(index);
                client.flush();
                client.stop();
              }
              else if(data[2] == 'D'){
                disable_SSR(index);
                client.write(data,3);
                Serial.print("Disable SSR");Serial.println(index);
                client.print("Disabled the SSR ");client.print(index);
                client.flush();
                client.stop();
              }
            }
            else{
              client.println("SSR should be 0-7");
              client.flush();
              client.stop();
            }
          }
          else{
            client.println("No PCF connected");
            client.flush();
            client.stop();
          }
        }
        //SSR CONTROL Done----------------------------------------------------------------------------------------
        //Fake RFID Reading
        if(data[0] == 'N'){ //N for NFC
          data[1] = client.read();
          data[2] = client.read();
          data[3] = client.read();
          Serial.print("Received ");
          for(int i = 0;i<5;i++){
            Serial.print(data[i]);
          }
          Serial.println("");
          byte index = (data[2]-'0')*10 + data[3]-'0';
          char return_data[10] = "SLX01-";
          String ret_data = "SLX01-XXXXXXX";
          if (random(10)<7){
            ret_data = "EmptySlotXXXX";
          }
          if(data[1] == 'R'){ //Reading
            ret_data[8]= data[2]+48;
            ret_data[9]= data[3]+48;
            //client.write(return_data,10);
            client.print(ret_data);
//            client.write(data,4);
            client.print("EOT");
            Serial.println("Reading RFID");
          }
          else if (data[1] == 'W'){ //Writing
            //return_data = "SLX01-WR ";
            return_data[8]= data[2];
            return_data[9]= data[3];
            client.write(return_data,10);
            //client.write(data,4);
            client.print("EOT");
            Serial.println("Writing RFID");
          }
          else{
            client.print("EOT");
          }
          Serial.println("Fake RFID Reading/Writing");
          client.flush();
          client.stop();
        }
        else if(data[0] == 'R'){ //Check that the first letter = B for boot
          //Reset the portenta
          Serial.println("I will reset, wish me luck");
          client.print("The portenta will reset, please wait a few seconds, (this is not a true reset)");
          client.flush();
          client.stop();
          //reset();
          resetFunc();
        }
        else if(data[0] == 'S'){ //Give status of the portenta
          Serial.println("Status cmd received");
          client.print("I have ");client.print(pcf ? 1 : 0);client.print(" pcf connected");
          client.print("I have ");client.print(nRFID);client.print(" RFID chip conected");
        }
        else{
          client.print("Me not understand");
          client.stop();       
        }
        client.stop();
        Serial.println("client disconnected");
        Serial.println("----------------------");
      }
      //client.print("End");
    }
  }
  //client.print("End");
  client.stop();
  digitalWrite(LEDB,LOW);
  delay(100);
}
