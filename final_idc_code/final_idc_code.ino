//everything counter clockwise

#include <Servo.h> // Servo library
#include <Wire.h> // Whisker Wire library
#include "Adafruit_TCS34725.h" // Arduino RGB library

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const int PIN_BLUE = 44; // initializes Arduino RGB
const int PIN_RED = 45;
const int PIN_GREEN = 46;
int count = 1; // counter used for identifying continent, ensures color is only sensed once
int finalObject = 0; // 0 or 1 state of object

Servo servoLeft; // initializes servos
Servo servoRight;

#define rightQTI 53 // initializes QTI sensors
#define midQTI 51
#define leftQTI 49

//for XBee
#define Rx 17 // DOUT to pin 17
#define Rx 16 // DIN to pin 16

const int TEAM_NUMBER = 36; // used for XBee
int commsNums[40]; //array of team values
unsigned long myTime; //tracks time 

void setup() {
  pinMode(2,OUTPUT); // red LED
  pinMode(7, OUTPUT); // green LED
  digitalWrite(2,LOW); // red and green start LOW
  digitalWrite(7,LOW);

  //for XBee
  Serial.begin(9600); // begins serial connection between our Arduino and another device
  Serial2.begin(9600); // begins serial connections between other devices and our Arduino
  pinMode(5, OUTPUT); // LED representing when signal sent - yellow
  digitalWrite(5, LOW); // set sentLED to low
  delay(500);
  
  Serial3.begin(9600); // opens LCD
  Serial3.write(12); // LCD - clears monitor

  pinMode(PIN_RED,   OUTPUT); // sets Arduino RGB as an output
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  
  servoLeft.attach(11); // attaches servos
  servoRight.attach(12);

  for (int i = 0; i < 40; i++) commsNums[i] = -1;
  //created array for XBee
  
  delay(1000); // delays for 1s

  //Place bot on hash mark, moves forward for .1 seconds to begin line following
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
  delay(300);
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
}

void right(){ // bot turns to the right
  servoLeft.writeMicroseconds(1540); 
  servoRight.writeMicroseconds(1540);
}

void left(){ // bot turns to the left
  servoRight.writeMicroseconds(1460);
  servoLeft.writeMicroseconds(1460);
}

void goForward(){ // bot moves forwards
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
}

void orientCorrect(){ // finds object in circles
  // 45 degree turn
  servoLeft.writeMicroseconds(1300); // moves backwards
  servoRight.writeMicroseconds(1700);
  delay(1000);
  servoLeft.writeMicroseconds(1460); // turns
  servoRight.writeMicroseconds(1460);
  delay(500);
  servoLeft.writeMicroseconds(1700); // moves forwards
  servoRight.writeMicroseconds(1300);
  delay(500);
  servoLeft.writeMicroseconds(1500); // stops
  servoRight.writeMicroseconds(1500);
  while(count < 10){ // uses counter as true/false
      showColor(); // calls function to find color of continent
      count = count + 1; // sets count to different value, makes while loop false
    }
    servoLeft.writeMicroseconds(1700); // moves forwards
  servoRight.writeMicroseconds(1300);
  delay(500);
  servoLeft.writeMicroseconds(1500); // stops
  servoRight.writeMicroseconds(1500);
    delay(500);
  servoRight.detach(); // stops
  servoLeft.detach();
}


void showBlue(){ // Arduino RGB shows blue
  analogWrite(PIN_RED, 255);
  analogWrite(PIN_BLUE, 0);
  analogWrite(PIN_GREEN, 255);
  }

void showRed(){ // Arduino RGB shows red
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_BLUE, 255);
  analogWrite(PIN_GREEN, 255);
  }

void showGreen(){ // Arduino RGB shows green
  analogWrite(PIN_RED, 255);
  analogWrite(PIN_BLUE, 255);
  analogWrite(PIN_GREEN, 0);
  }

void showYellow(){ // Arduino RGB shows yellow
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_BLUE, 255);
  analogWrite(PIN_GREEN, 0);
  }

void showPurple(){ // Arduino RGB shows purple
  analogWrite(PIN_RED, 175);
  analogWrite(PIN_BLUE, 175);
  analogWrite(PIN_GREEN, 255);
  }

void showWhite(){ //Arduino RGB shows white
  analogWrite(PIN_RED, 0); // for Grey continent
  analogWrite(PIN_BLUE, 0);
  analogWrite(PIN_GREEN, 0);
  }

long RCTime(int sensorIn){ // reads value from QTI (charge transfer infrared) sensor
  long duration = 0;
  pinMode(sensorIn, OUTPUT);
  digitalWrite(sensorIn, HIGH);
  delay(1);
  pinMode(sensorIn, INPUT);
  pinMode(sensorIn, LOW);

  while(digitalRead(sensorIn)){
    duration++;
  }
  return duration;
}

//for XBee - sends our signal to other bots and saves our value into array
void sendSignal(bool passed){
  int n = TEAM_NUMBER << 1; // multiplies by 2 (shifts bits to left)
  n += passed ? 1 : 0; //adds a 1 or 0
  commsNums[TEAM_NUMBER] = passed ? 1 : 0;
  char c = (char) n; // character printed relates to char int value
  Serial2.print(c); // H correlates to 0 (72), I correlates to 1 (73)
}

void showColor(){ //shows color on Arduino RGB
  delay(50);
  uint16_t clear, red, green, blue;
  delay(60);  // takes 60ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  float total = red + green + blue; // calculates ratio between red, green, and blue
  float redT = red/total;
  float blueT = blue/total;
  float greenT = green/total;

  Serial.print("C:\t"); Serial.print(clear); //prints values to serial  monitor
  Serial.print("\tR:\t"); Serial.print(redT);
  Serial.print("\tG:\t"); Serial.print(greenT);
  Serial.print("\tB:\t"); Serial.println(blueT);

  if(redT > 0.6 && redT < 0.7 && greenT > 0.13 && greenT < 0.23 && blueT > 0.13 && blueT < 0.23) { 
    showRed(); //sees red continent
    } else if(redT > 0.17 && redT < 0.27 && greenT > 0.27 && greenT < 0.37 && blueT > 0.38 && blueT < 0.48) {
      showBlue(); //sees blue continent
      } else if(redT > 0.27 && redT < 0.37 && greenT > 0.31 && greenT < 0.41 && blueT > 0.27 && blueT < 0.37) {
        showWhite(); //sees grey continent
        } else if(redT > 0.25 && redT < 0.35 && greenT > 0.41 && greenT < 0.51 && blueT > 0.19 && blueT < 0.29) {
        showGreen(); //sees green continent
        } else if(redT > 0.35 && redT < 0.45 && greenT > 0.35 && greenT < 0.45 && blueT > 0.15 && blueT < 0.25) {
        showYellow(); //sees yellow continent
        } else if(redT > 0.37 && redT < 0.47 && greenT > 0.2 && greenT < 0.3 && blueT > 0.28 && blueT < 0.38) {
        showPurple(); //sees purple continent
        }
}

void readObject(){ // senses if whisker wire circuit is complete, turns on green or red LED
  for(int i = 0; i < 30; i++){
    delay(400); // delays to allow time for robot to hit
    if(digitalRead(9) == HIGH){ // if circuit completes
    digitalWrite(7, HIGH); // green LED on
    digitalWrite(2, LOW); // red LED off
  }
  else { // if circuit does not complete
    digitalWrite(7, LOW); // green LED off
    digitalWrite(2, HIGH); // red LED on
  }
  delay(10);
  }
}

//Associates grand sum with abbreviation
void nextChalls(int sum){
  if(sum == 0){
      Serial3.print(" MNC");
      } else if(sum == 1) {
        Serial3.print(" AHI");
        } else if(sum == 2) {
        Serial3.print(" EBM");
          } else if(sum == 3) {
          Serial3.print(" EBR");
            } else if(sum == 4) {
              Serial3.print(" ETSD");
              } else if(sum == 5) {
                Serial3.print(" REB");
                } else if(sum == 6) {
                  Serial3.print(" APL");
                  } else{
                    Serial3.print("ETSD"); //DEFAULT case in the event that a sum less than 0 but greater than six is achieved
                  }

}

//converts character to 0 or 1 and places into correct index in array
void updateArray(int inc){
  int state = (int) inc % 2;  //casts character to int and mods by 2
      int team_index = (int) inc / 2; //divides by two to find team number
      commsNums[team_index] = state; //inserts 0 or 1 state into team number index of array
}


//calculates current total
 int currentTotal(){
  int currTot = 0;
  for(int i=31; i<=36; i++){ //loops through team indices of array
    if(commsNums[i] >= 0){ //if a state has been received
      currTot = currTot + commsNums[i]; //add state to current total
    }
  }
  return currTot;
 }

void loop() {
  boolean signalBool = false; // boolean for 0 or 1 state
  
  int rightValue = RCTime(rightQTI); // quantifying raw data from QTI
  int leftValue = RCTime(leftQTI);
  int midValue = RCTime(midQTI);

  //start line following
  if(rightValue < 90 && leftValue < 90 && midValue > 90){ // middle QTI reads black
    goForward();
  } else if (rightValue > 90 && leftValue > 90 && midValue > 90){ 
    
    // all three QTI reads black at hash mark
    servoRight.writeMicroseconds(1500); // servos stop
    servoLeft.writeMicroseconds(1500);
    delay(1000); 
    orientCorrect(); // moves robot to find object 
    delay(100);
    readObject(); // finds if object is there
    delay(300);
    if(digitalRead(7) == HIGH){ // when sees object
      finalObject = 1; //reads metal
      signalBool = true; //stores I
        }
    else if(digitalRead(2) == HIGH){ // sees plastic
      finalObject = 0; //reads plastic
      signalBool = false; //stores H
      } 



    int loopCount = 0; // counting iterations through the loop
    myTime = millis(); //records length of time program has been running for
    //while array is not full and time is less than three minutes
    while((commsNums[31] < 0 || commsNums[32] < 0 || commsNums[33] < 0 || commsNums[34] < 0 || commsNums[35] < 0 || commsNums[36] < 0) && ((myTime/1000) < 400)) { 
    if (Serial2.available()){
      char incoming = Serial2.read(); //Read character
      updateArray(incoming);
      myTime = millis(); //update length of program every iteration of loop
    }

    if(loopCount % 20 == 0){ //sends signal every 50 iterations through the loop to prevent spam
      sendSignal(signalBool);
    }
    
    Serial3.write(12); //prints current team sum
    Serial3.print("Current:");
    Serial3.print(currentTotal());
    
    loopCount++;
    delay(50);
    }

    Serial3.write(12);
    Serial3.print("Final: "); //prints final team sum
    Serial3.print(currentTotal());
    Serial3.print(" ");
    nextChalls(currentTotal());
 }    

 else if (leftValue > 90){ // left QTI reads black
    left(); // turns left
  } else if (rightValue > 90) { // right QTI reads black
    right(); // turns right
  } 
   
}
