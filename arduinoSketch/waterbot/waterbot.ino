//------------------------------------------------------------------------------------------------------------------
// NAME: Waterbot
// AUTH: Ryan McCartney
// DATE: 15th December 2018
// DESC: Microsubmersible pump and servo motor platn watering system
// NOTE: GNU License
//------------------------------------------------------------------------------------------------------------------

//Pin Definitions
#define servoPin 24
#define pump 22

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
float flowrate = 10.00;

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
bool water(int plant, float quantity) {

  bool status = false;

  int dispenseTime = int((quantity / flowrate) * 1000);

  //Move servo to plant
  servo.write(plant);

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

  //Create the the character array (the buffer)
  for (int i = 0; i < dataLength; i++) {

    //If the incoming character is a comma then it must indicate the end of a variable
    if (dataChars[i] == delimiter) {
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
  }

  if (status == true) {

    status = water(plant[(commands[0].toInt())],commands[1].toInt());

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

