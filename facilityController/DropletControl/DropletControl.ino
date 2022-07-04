#include "DropGenerator.h"

// Humidity sensor library
#include "DHT.h"

#define DHTPIN 4 // humidity pin

#define pinButton 5 // button
#define pinLight 1 // light-trigger
#define pinPhotron 12 // photron-trigger WAS: pinCam
#define pinBaumer 2 // synchro LED WAS: pinLED 

const int BAUMER_FPS = 60; // Baumer recording fps
const int LIGHT_BRIGHTENING_TIME = 3500; // light increasing time
const unsigned long MAX_RECORD_TIME = 60000; // record time WAS: MAX_FREEZING_TIME

// Humidity sensor init
DHT dht(DHTPIN, DHT22);

// droplet generating parameters
int impulseCount = 25; // count of mini impulses
int miniImpulseTime = 5; // time of mini impulse, ms
int betweenImpulseTime = 5; // time between impulses, ms
int largeImpulseTime = 12; // time of large impulse, ms

int openImpulseTime = 2000; // time of openning, ms

int baumerDelay = 1000 / (2*BAUMER_FPS); // One-half delay for recording, ms

int incomingByte = 0; // for incoming data
String testName;

char generatingValues[90];

unsigned long checkMillis; // time variable
unsigned long baumerCheckMillis; // time variable
boolean flagLED; // LED switching flag

boolean flagMeasure = false; // Provide full measurement

// humidity var, in %
float humidity;

// temperature in the chamber, *C
float chamber_tempr;

// Init dropGenerator object
DropGenerator dropGenerator;

void setup() {
  Serial.begin(9600);
  pinMode(pinButton, INPUT);
  
  pinMode(pinLight, OUTPUT);
  digitalWrite(pinLight,LOW);
  
  pinMode(pinBaumer, OUTPUT);
  digitalWrite(pinBaumer, LOW);
  
  pinMode(pinPhotron, OUTPUT);
  digitalWrite(pinPhotron, LOW);

  dht.begin();

  Serial.println("Default settings of droplet generating:");
  Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");
  Serial.print(impulseCount);
  Serial.print(",");
  Serial.print(miniImpulseTime);
  Serial.print(",");
  Serial.print(largeImpulseTime);
  Serial.print(",");
  Serial.println(betweenImpulseTime);

  
  }

void loop() {
  // open nozzle for taking syringe up - "u"
  // generate droplets by command "g"
  // turn on/off the light by command "l"
  // get humidity and ambient temperature by command "h"
  // full program by command "f". Stop command by "f"
  // change settings by command "s".
  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();

    // PROVIDE FULL PROGRAM
    if (incomingByte == 102) // letter 'f'
    { 
      // FLAG: generating & measure
      flagMeasure = true;
      Serial.readString();
    }

    // DROPLET GENERATOR CONTROL
    if (incomingByte == 103) // letter 'g'
    {
      Serial.println("Current settings of droplet generating:");
      Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");
      Serial.print(impulseCount);
      Serial.print(",");
      Serial.print(miniImpulseTime);
      Serial.print(",");
      Serial.print(largeImpulseTime);
      Serial.print(",");
      Serial.println(betweenImpulseTime);
        
      // Prepare large droplet
      Serial.print(millis());
      Serial.println(". Droplet generating start");
      dropGenerator.largeDropPreparing(impulseCount, miniImpulseTime, betweenImpulseTime);

      // Generate large droplet (with macro impulse)
      dropGenerator.oneDrop(largeImpulseTime);
      Serial.print(millis());
      Serial.println(". Droplet generating end");  
    }

    // LIGHT CONTROL
    if (incomingByte == 108) // letter 'l'
    {
      if (digitalRead(pinLight))
      {
        // Switch of the light.
        digitalWrite(pinLight,LOW);
        Serial.print(millis());
        Serial.println(". Light OFF");
      }
      else
      {
        // Switch on the light
        digitalWrite(pinLight,HIGH);
        Serial.print(millis());
        Serial.println(". Light ON");
      }
    }

    // GET HUMIDITY
    if (incomingByte == 104) // letter 'h'
    {
      // Create csv header
      Serial.println("time,chamber_thermo,humidity");
      Serial.print(millis());

      // measure humidity and chamber temperature
      humidity = dht.readHumidity();
      chamber_tempr = dht.readTemperature();

      //print humidity and thermo values
      Serial.print(",");
      Serial.print(chamber_tempr);
      Serial.print(",");
      Serial.print(humidity);
     
      Serial.println();   
    }
    
    // CHANGE DROPLET GENERATING SETTINGS
    if (incomingByte == 115) // letter 's'
    {
      // display settings
      Serial.println("Current settings of droplet generating:");
      Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");
      Serial.print(impulseCount);
      Serial.print(",");
      Serial.print(miniImpulseTime);
      Serial.print(",");
      Serial.print(largeImpulseTime);
      Serial.print(",");
      Serial.println(betweenImpulseTime);

      Serial.println();

      Serial.readString();
      Serial.println("Do you want to change them?");
      Serial.println("Write (c) for changing impulseCount");
      Serial.println("Write (m) for changing miniImpulseTime");
      Serial.println("Write (l) for changing largeImpulseTime");
      Serial.println("Write (b) for changing betweenImpulseTime");
      Serial.println("Write any other letter for cancelling settings changing.");
      while (Serial.available() == 0)
      {
        }

      //Serial.println("Write new settings, using comma as delimiter");
      //Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");       
      incomingByte = Serial.read();
      Serial.readString();
      if (incomingByte == 99) // letter 'c'
      {
        Serial.println("Write impulseCount:");
        while (Serial.available() == 0)
        {
          }
        impulseCount = Serial.readString().toInt();
      }

      if (incomingByte == 109) // letter 'm'
      {
        Serial.println("Write miniImpulseTime:");
        while (Serial.available() == 0)
        {
          }
        miniImpulseTime = Serial.readString().toInt();
      }

      if (incomingByte == 108) // letter 'l'
      {
        Serial.println("Write largeImpulseTime:");
        while (Serial.available() == 0)
        {
          }
        largeImpulseTime = Serial.readString().toInt();
      }

      if (incomingByte == 98) // letter 'b'
      {
        Serial.println("Write betweenImpulseTime:");
        while (Serial.available() == 0)
        {
          }
        betweenImpulseTime = Serial.readString().toInt();
      }

        Serial.println("New settings of droplet generating:");
        Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");
        Serial.print(impulseCount);
        Serial.print(",");
        Serial.print(miniImpulseTime);
        Serial.print(",");
        Serial.print(largeImpulseTime);
        Serial.print(",");
        Serial.println(betweenImpulseTime);
    }

    // RETURN SYRINGE UP
    if (incomingByte == 117) // letter 'u'
    {
      // Open-close nozzle
      Serial.print(millis());
      Serial.println(". Open nozzle");
      dropGenerator.oneDrop(openImpulseTime);
      Serial.print(millis());
      Serial.println(". Close nozzle");
    }
  }
  
  // START FULL-PROGRAM, if button is pressed or measure flag
  if((digitalRead(pinButton)==LOW)||flagMeasure){
    // Drop measure flag
    flagMeasure = false;
    
    // Switch of the light. In case of switching on by command.
    digitalWrite(pinLight,LOW);
    
    Serial.println();
    Serial.println("Write test name!");
    while (Serial.available() == 0)
    {
      }
    testName = Serial.readString();
    Serial.println();
    Serial.println();
    Serial.print("BEGIN TEST #");
    Serial.print(testName);
    // Switch on the light
    digitalWrite(pinLight,HIGH);
    checkMillis = millis();
    Serial.print(checkMillis);
    Serial.println(". Light ON");
    

    // GET TEMPERATURES: before drop generating
    // Create csv header
    Serial.println("time,chamber_thermo,humidity");
    // Thermo and humidity measuring, while light is getting brighter
    while (millis()-checkMillis < LIGHT_BRIGHTENING_TIME)
    {
      // measure humidity and chamber temperature
      humidity = dht.readHumidity();
      chamber_tempr = dht.readTemperature();

      //print humidity and thermo values
      Serial.print(millis());
      Serial.print(",");
      Serial.print(chamber_tempr);
      Serial.print(",");
      Serial.print(humidity);
     
      Serial.println();
      delay(500);
    }

    Serial.println("Current settings of droplet generating:");
    Serial.println("impulseCount,miniImpulseTime,largeImpulseTime,betweenImpulseTime");
    Serial.print(impulseCount);
    Serial.print(",");
    Serial.print(miniImpulseTime);
    Serial.print(",");
    Serial.print(largeImpulseTime);
    Serial.print(",");
    Serial.println(betweenImpulseTime);

    // Prepare large droplet
    Serial.print(millis());
    Serial.println(". Droplet generating start");
    dropGenerator.largeDropPreparing(impulseCount, miniImpulseTime, betweenImpulseTime);
        
    // Generate large droplet (with macro impulse)
    dropGenerator.oneDrop(largeImpulseTime);
    Serial.print(millis());
    Serial.println(". Droplet generating end");
    
    // Photron recording
    Serial.print(millis());
    Serial.println(". Photron recording start");
    digitalWrite(pinPhotron, HIGH);
    delay(1);
    digitalWrite(pinPhotron,LOW);

    // BAUMER-RECORD
    Serial.print(millis());
    Serial.print(". Baumer recording start with FPS: ");
    Serial.print(BAUMER_FPS);
    Serial.print("; one-half delay, ms: ");
    Serial.println(baumerDelay);
    while (millis()-checkMillis < MAX_RECORD_TIME)
    {
      digitalWrite(pinBaumer, HIGH);
      delay(baumerDelay);
      digitalWrite(pinBaumer, LOW);
      baumerCheckMillis = millis();
      
      // CHECK STOP COMMAND
      if (Serial.available() > 0)
      {
        incomingByte = Serial.read();
    
        // STOP COMMAND
        if (incomingByte == 102) // letter 'f'
        { 
          Serial.readString();
          break;
        }
      }
      
      while (millis()-baumerCheckMillis < baumerDelay-1)
      {
        }
    }
      
    // Switch of the light.
    digitalWrite(pinLight,LOW);
    Serial.print(millis());
    Serial.println(". Light OFF");
    Serial.print("END TEST #");
    Serial.print(testName);
   }
 }
