#include <Arduino.h>
#include <PID_v1.h>

#define pressurePin A0 // Get input from pressure sensor
#define motorPin 3 // control pump motor speed
#define dirPin 6
#define valvePin1 2 // control forward valve
#define valvePin2 4 // control return valve

double motorSpeed = 100; // 0 to 255
int heartRate = 0; // Controls the speed of the valve actuation (Typically 50-120)
double pressure = 0; // pressure read from transducer in kPa
double targetPressure = 0; //target pressure the system should be at in kPa. (Typically 5-25kPa)
String data = "";

PID pressurePID(&pressure, &motorSpeed, &targetPressure, 30, 5, 1, DIRECT);

void setMotorSpeed(){
  pressure = (analogRead(pressurePin)/409.5)*6.89476; // 0 - 4095. (0 - 10 PSI)
  pressurePID.Compute();
  if(heartRate <= 0){
    analogWrite(motorPin, 0);
  }
  else{
    analogWrite(motorPin, (int)motorSpeed);
  }
}

void heartBeat(){ //turns the valves on and off to simulate one heart beat
  if(heartRate <= 0){return;}
  int delayTime = (60000)/(heartRate*2); // half the time of a single heartbeat in ms
  digitalWrite(valvePin1, HIGH);
  //delay(delayTime);
  unsigned long startTime = millis();
  while(millis()-startTime < delayTime){
    setMotorSpeed();
    printPID();
  }
  digitalWrite(valvePin1, LOW);
  digitalWrite(valvePin2, HIGH);
  //delay(delayTime);
  startTime = millis();
  while(millis()-startTime < delayTime){
    setMotorSpeed();
    printPID();
  }
  digitalWrite(valvePin2, LOW);
} 

void processString(){
  if (data.substring(0, 5) == "BEAT "){
    heartRate = data.substring(5, data.length()).toInt();
    Serial.print("Heart Rate set to ");
    Serial.print(heartRate);
    Serial.println(" BPM.");
  }
  else if (data.substring(0, 5) == "PRSR "){
    targetPressure = data.substring(5, data.length()).toDouble();
    Serial.print("Target Pressure set to ");
    Serial.print(targetPressure);
    Serial.println(" kPa.");
  }
  else if (data.substring(0, 5) == "FILL "){
    int fillTime = data.substring(5, data.length()).toInt();
    digitalWrite(valvePin1, HIGH);
    digitalWrite(valvePin2, HIGH);
    analogWrite(motorPin, 255);
    delay(fillTime*1000);
    digitalWrite(valvePin1, LOW);
    digitalWrite(valvePin2, LOW);
  }
}

void getData(){
  //Serial.println("data recieved");
  data = "";
  while(Serial.available() > 0){
    char c = Serial.read();
    if(c != '\n'){
      data += c;
    }
    delay(2);
  }
  Serial.println(data);
}

void printPID(){
  Serial.print("Current Pressure: ");
  Serial.print(pressure);
  Serial.print(", Target Pressure: ");
  Serial.print(targetPressure);
  Serial.print(", Motor Speed:");
  Serial.println(motorSpeed);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pressurePin, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(valvePin1, OUTPUT);
  pinMode(valvePin2, OUTPUT);
  digitalWrite(dirPin, LOW);
  Serial.begin(115200);
  pressurePID.SetOutputLimits(0, 255);
  pressurePID.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:
  heartBeat();
  if(Serial.available()){
    getData();
    processString();
  }
  setMotorSpeed();
  printPID();

}
