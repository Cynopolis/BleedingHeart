#include <Arduino.h>
#include <PID_v1.h>

#define pressurePin A0 // Get input from pressure sensor
#define motorPin 3 // control pump motor speed
#define dirPin 6
#define valvePin1 2 // control forward valve
#define valvePin2 4 // control return valve

double motorSpeed = 0; // 0 to 255
int heartRate = 60; // Controls the speed of the valve actuation (Typically 50-120)
double pressure = 0; // pressure read from transducer in PSI
double targetPressure = 1.5; //target pressure the system should be at in PSI. (Typically 0.5-5PSI)
String data = "";
double runningPressure[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

PID pressurePID(&pressure, &motorSpeed, &targetPressure, 6.5, 0.8, 0.03, DIRECT);

double getPressure(){
  //pressure = map(analogRead(pressurePin), 0, 1023, 0, 80)-54; // 0 - 1023. (0 - 80 PSI)
  double sum = 0;
  for(int i = 0; i < 9; i++){
    sum += runningPressure[i];
    runningPressure[i] = runningPressure[i+1];
  }
  runningPressure[9] = map(analogRead(pressurePin), 0, 1023, 0, 80)-7;
  sum += runningPressure[9];
  pressure = sum/15;
  return pressure;
}

void setMotorSpeed(int motorSpeed){
  if(heartRate <= 0){
    analogWrite(motorPin, 0);
  }
  else{
    analogWrite(motorPin, motorSpeed);
  }
  printPID();
}

void heartBeat(){ //turns the valves on and off to simulate one heart beat
  if(heartRate <= 0){return;}
  double systolic = targetPressure*2;
  double diastolic = targetPressure;
  int delayTime = (60000)/(heartRate*2); // half the time of a single heartbeat in ms
  
  digitalWrite(valvePin1, HIGH);
  targetPressure = systolic;
  unsigned long startTime = millis();
  while(millis()-startTime < delayTime){
    getPressure();
    pressurePID.Compute();
    setMotorSpeed((int)motorSpeed);
  }
  digitalWrite(valvePin1, LOW);
  digitalWrite(valvePin2, HIGH);
  targetPressure = diastolic;
  //delay(delayTime);
  startTime = millis();
  while(millis()-startTime < delayTime){
    getPressure();
    pressurePID.Compute();
    setMotorSpeed((int)motorSpeed);
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
    Serial.println(" PSI.");
  }
  else if (data.substring(0, 5) == "FILL "){
    int fillTime = data.substring(5, data.length()).toInt();
    digitalWrite(valvePin1, HIGH);
    digitalWrite(valvePin2, HIGH);
    analogWrite(motorPin, 0);
    //double foo = targetPressure;
    //targetPressure = 10000;
    unsigned long startTime = millis();
    while(millis()-startTime < fillTime*1000){
      setMotorSpeed(255);
    }
    digitalWrite(valvePin1, LOW);
    digitalWrite(valvePin2, LOW);
    //targetPressure = foo;
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
  pressurePID.SetSampleTime(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  heartBeat();
  if(Serial.available()){
    getData();
    processString();
  }
  getPressure();
  pressurePID.Compute();
  printPID();

}
