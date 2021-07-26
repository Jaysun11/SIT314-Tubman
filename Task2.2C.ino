//Libraries
#include <DHT.h>
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"

//Constants
#define DHTPIN 2     // what pin we're connected to for the temp sensor (2)
#define DHTTYPE DHT22   // DHT 22  
#define LOG_INTERVAL  2000 // mills between entries. 
#define SYNC_INTERVAL 2000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

#define ECHO_TO_SERIAL   1 // echo data to serial port. 1 is on, 0 is off


//Variables
float temp; //Stores temperature value

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void setup()
{
    Serial.begin(9600);

    // initialize the SD card
    initSDcard();

    // create a new file
    createFile();
    
    //Connect to RTC
    initRTC();
    
    dht.begin();

     logfile.println("datetime,temp");
     
     if(ECHO_TO_SERIAL){
        Serial.println("datetime,temp");
     }

     pinMode(13, OUTPUT);
  
}

void loop()
{

    digitalWrite(13, HIGH);
  
    DateTime now;

     // delay for the amount of time we want between readings
    delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

    //Get the time from the RTC
      now = RTC.now();
      //Print the time and date in format DD/MM/YYYY HH:MM:SS
      logfile.print(now.day(), DEC);
      logfile.print("/");
      logfile.print(now.month(), DEC);
      logfile.print("/");
      logfile.print(now.year(), DEC);
      logfile.print(" ");
      logfile.print(now.hour(), DEC);
      logfile.print(":");
      logfile.print(now.minute(), DEC);
      logfile.print(":");
      logfile.print(now.second(), DEC);
      if ( ECHO_TO_SERIAL) { //If echo to serial is on, print to serial too
        Serial.print(now.day(), DEC);
        Serial.print("/");
        Serial.print(now.month(), DEC);
        Serial.print("/");
        Serial.print(now.year(), DEC);
        Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(":");
        Serial.print(now.minute(), DEC);
        Serial.print(":");
        Serial.print(now.second(), DEC);
      }
    
    //Read data and store it to variable temp
    temp= dht.readTemperature();


    
    //Print temp and humidity values to file 
    logfile.print(", ");
    logfile.println(temp);

    if (ECHO_TO_SERIAL){ //If we want to echo to serial do it too
      Serial.print(", ");
      Serial.println(temp);
    }

      //Run temp check for AC UNIT

    if (temp > 22){
      Serial.println("Temperature is too High");
    } else if (temp < 20){
      Serial.println("Temperature is too Low");
    } else {
      Serial.println("Temperature is ideal");
    }
    
  if ((millis() - syncTime) < SYNC_INTERVAL) return; //Once the desired amount of time has passed (2 seconds)
  //Return

  //Update the time to ensure we check every 2 seconds
  syncTime = millis();

  //Ensures that any bytes written to the file are physically saved to the SD card.
  logfile.flush();
}

//This function is a halt, if there is a fatal error, pause the program indefinitely.
void error(char const *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while (1);
}

void initSDcard()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

}


void createFile()
{
  //file name must be in 8.3 format (name length at most 8 characters, follwed by a '.' and then a three character extension.
  //This loop will allow for 99 files to be stored
  char filename[] = "TEMP00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
}

void initRTC()
{
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
     if (ECHO_TO_SERIAL){
      Serial.println("RTC failed");
     }
  }
 
}
