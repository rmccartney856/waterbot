//------------------------------------------------------------------------------------------------------------------
// NAME: waterbot.ino
// AUTH: Ryan McCartney
// DATE: 15th December 2018
// DESC: Microsubmersible pump and servo motor plant watering system
// NOTE: GNU General Public License (GPL)
//------------------------------------------------------------------------------------------------------------------

//Pin Definitions
#define servoPin 2
#define pump 3

//Receive Data Variables
String inputString = "";
bool stringComplete = false;

//Parse Commands
const int variables = 3;
String commands[variables] = "";

//Servo
#include <Servo.h>
Servo servo;

//Watering positions
int plant[] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180};

//Pump flowrate for quantity measurment (ml/s)
float flowrate = 10.00; //in ml per second
float capacity = 2000; //in ml
int spinup = 500; //in ms

//------------------------------------------------------------------------------------------------------------------
//Setup Code initialising waterbot on powerup
//------------------------------------------------------------------------------------------------------------------
void setup() {

  //Serial Communications Setup
  Serial.begin(9600);
  //Reserve Memory Sapce for Incomming Bytes
  inputString.reserve(200);

  //Initialises Servo Motor
  servo.attach(servoPin);

  //Initialise the Pump as an output
  pinMode(pump, OUTPUT);

  //Move to first plant positions
  servo.write(plant[0]);

  //Turn off the pump
  digitalWrite(pump, LOW);
}

//------------------------------------------------------------------------------------------------------------------
//Plant watering function
//------------------------------------------------------------------------------------------------------------------
bool water(int plant, int quantity) {

  bool status = false;

  unsigned long dispenseTime = (quantity / flowrate) * 1000;
  dispenseTime = dispenseTime + spinup;
  
  Serial.print("Dispensing Time = ");
  Serial.print(dispenseTime);
  Serial.println("ms");

  //Move servo to plant
  servo.write(plant);

  delay(500);

  //Turn on pump to begin watering
  digitalWrite(pump, HIGH);

  delay(dispenseTime);

  //Turn off the pump after watering is finished
  digitalWrite(pump, LOW);

  return status;
}

//------------------------------------------------------------------------------------------------------------------
//When Serial input is made this function parses the contents
//------------------------------------------------------------------------------------------------------------------
bool csvParse(String data) {

  bool status = false;
  char delimiter = ',';
  char newline = '\r';
  int variable = 0;

  // Length (with one extra character for the null terminator)
  int dataLength = data.length() + 1;

  // Prepare the character array (the buffer)
  char dataChars[dataLength];

  // Copy it over
  data.toCharArray(dataChars, dataLength);

  //Clear existing commands
  memset(commands, NULL, sizeof(commands));

  //Create the the character array (the buffer)
  for (int i = 0; i < dataLength; i++) {

    //If the incoming character is a comma then it must indicate the end of a variable
    if (dataChars[i] == delimiter) {

      //Some Debug Prints
      /*
        Serial.print("Variable Command number ");
        Serial.print(variable);
        Serial.print(" = ");
        Serial.println(commands[variable]);
      */

      variable++;
      status = true;
    }
    //Otherwise add the character to the command
    else {
      commands[variable] += dataChars[i];
    }
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------
//Main program loop
//------------------------------------------------------------------------------------------------------------------
void loop() {

  bool status = false;

  if (stringComplete == true) {

    status = csvParse(inputString);

    //Reset Serial
    inputString = "";
    stringComplete = false;


    if (status == true) {

      //Commands for special statements
      switch ((commands[2].toInt())) {
        case 0:
          Serial.println("STATUS: Dispensing water");
          status = water(plant[(commands[0].toInt())], commands[1].toInt());
          break;

        case 1:
          Serial.println("STATUS: Sweeping");

          for (int pos = plant[0]; pos <= plant[9]; pos += 1) {
            // in steps of 1 degree
            servo.write(pos);
            delay(5);
          }

          for (int pos = plant[9]; pos >= plant[0]; pos -= 1) {
            servo.write(pos);
            delay(5);
          }
          break;

        case 2:
          Serial.println("STATUS: Emptying Water Tank");
          break;
      }

    }
  }


}

//------------------------------------------------------------------------------------------------------------------
//Rertieve Serial Data
//------------------------------------------------------------------------------------------------------------------
void serialEvent() {

  while (Serial.available()) {

    //Gets the next byte
    char inputChar = (char)Serial.read();

    //Add latest char to string
    inputString += inputChar;

    if (inputChar == '\n') {
      stringComplete = true;
    }
  }
}

