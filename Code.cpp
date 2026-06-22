/*
AUTONOMOUS 4WD OBSTACLE AVOIDANCE ROBOT
Logic:
Move Forward
If Front > 40cm -> Keep Moving
If Front <= 40cm:
  Stop
  Scan Left -> Center
  Scan Right -> Center
  Compare
  L > R  -> Turn Left
  R > L  -> Turn Right
  Equal  -> Turn Right
  Move Forward Immediately
*/

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 3
#define IN4 4
#define ENB 11

#define TRIG_PIN 9
#define ECHO_PIN 10

#define SERVO_PIN 8
Servo scanner;

#define ENCODER_PIN 2
volatile unsigned long encoderPulses = 0;

const float PULSES_PER_METER = 470.0;
const float MISSION_DISTANCE = 7.0;

const int MOTOR_SPEED = 180;
const int TURN_SPEED  = 180;
const int SAFE_DISTANCE = 40;

const int SERVO_CENTER = 90;
const int SERVO_LEFT   = 170;
const int SERVO_RIGHT  = 10;

unsigned long obstacles=0, turns=0, leftTurns=0, rightTurns=0;
float distanceTravelled=0, currentSpeed=0, averageSpeed=0;

unsigned long missionStartTime;
unsigned long lastSpeedTime;
unsigned long lastPulseCount=0;
bool missionComplete=false;

void encoderISR(){ encoderPulses++; }

void moveForward(){
  analogWrite(ENA,MOTOR_SPEED);
  analogWrite(ENB,MOTOR_SPEED);

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void stopRobot(){
  analogWrite(ENA,0);
  analogWrite(ENB,0);

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

void turnLeft(){
  analogWrite(ENA,TURN_SPEED);
  analogWrite(ENB,TURN_SPEED);

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  delay(580);
  stopRobot();
}

void turnRight(){
  analogWrite(ENA,TURN_SPEED);
  analogWrite(ENB,TURN_SPEED);

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  delay(580);
  stopRobot();
}

long getDistance(){
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);

  long duration = pulseIn(ECHO_PIN,HIGH,30000);
  long distance = duration * 0.034 / 2;

  if(distance == 0) distance = 400;
  return distance;
}

int scanLeft(){
  scanner.write(SERVO_LEFT);
  delay(700);
  int d = getDistance();

  scanner.write(SERVO_CENTER);
  delay(300);
  return d;
}

int scanRight(){
  scanner.write(SERVO_RIGHT);
  delay(700);
  int d = getDistance();

  scanner.write(SERVO_CENTER);
  delay(300);
  return d;
}

void updateDistanceData(){
  noInterrupts();
  unsigned long p=encoderPulses;
  interrupts();
  distanceTravelled = p / PULSES_PER_METER;
}

void updateSpeedData(){
  unsigned long now=millis();

  if(now-lastSpeedTime>=1000){
    noInterrupts();
    unsigned long p=encoderPulses;
    interrupts();

    currentSpeed=(p-lastPulseCount)/PULSES_PER_METER;

    float elapsed=(now-missionStartTime)/1000.0;
    if(elapsed>0) averageSpeed=distanceTravelled/elapsed;

    lastPulseCount=p;
    lastSpeedTime=now;
  }
}

void startupCountdown()
{
  for (int i = 5; i >= 1; i--)
  {
    display.clearDisplay();
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(10, 8);
    display.print("Starting");

    display.setTextSize(4);
    display.setCursor(52, 24);
    display.print(i);

    int barWidth = map(i, 5, 0, 0, 120);
    display.drawRoundRect(4, 54, 120, 8, 2, SSD1306_WHITE);
    display.fillRect(6, 56, barWidth, 4, SSD1306_WHITE);

    display.display();
    delay(1000);
  }

  display.clearDisplay();
  display.drawRoundRect(0,0,128,64,4,SSD1306_WHITE);
  display.setTextSize(3);
  display.setCursor(30,20);
  display.print("GO!");
  display.display();
  delay(1000);
}

void showScanningScreen()
{
  display.clearDisplay();
  display.drawRoundRect(0,0,128,64,4,SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(15,10);
  display.print("OBSTACLE FOUND");

  display.setCursor(22,30);
  display.print("SCANNING...");

  display.setCursor(45,48);
  display.print("< O >");

  display.display();
}

void updateOLED()
{
  display.clearDisplay();

  display.fillRect(0,0,128,11,SSD1306_WHITE);

  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(34,2);
  display.print("[AUTO-BOT]");

  if ((millis()/500UL)%2==0)
    display.fillRect(122,3,4,5,SSD1306_BLACK);
  else
    display.drawRect(122,3,4,5,SSD1306_BLACK);

  display.setTextColor(SSD1306_WHITE);

  display.drawFastHLine(0,11,128,SSD1306_WHITE);
  display.drawFastVLine(64,12,29,SSD1306_WHITE);
  display.drawFastHLine(0,42,128,SSD1306_WHITE);

  display.setCursor(0,13);
  display.print("D:");
  display.print(distanceTravelled,2);
  display.print("m");

  display.setCursor(66,13);
  display.print("S:");
  display.print(currentSpeed,2);

  display.setCursor(0,23);
  display.print("A:");
  display.print(averageSpeed,2);

  display.setCursor(66,23);
  display.print("OB:");
  display.print(obstacles);

  display.setCursor(0,33);
  display.print("L:");
  display.print(leftTurns);
  display.print(" T:");
  display.print(turns);

  display.setCursor(66,33);
  display.print("R:");
  display.print(rightTurns);

  display.setCursor(0,44);
  display.print("PROGRESS");

  int pct = constrain((int)((distanceTravelled/MISSION_DISTANCE)*100.0),0,100);

  display.setCursor(98,44);
  display.print(pct);
  display.print("%");

  display.drawRoundRect(0,54,128,10,3,SSD1306_WHITE);

  int fill = constrain((int)((distanceTravelled/MISSION_DISTANCE)*122.0),0,122);

  if(fill>0)
    display.fillRect(3,56,fill,6,SSD1306_WHITE);

  display.display();
}

void finishMission(){
  missionComplete=true;
  stopRobot();

  display.clearDisplay();
  display.drawRoundRect(0,0,128,64,4,SSD1306_WHITE);

  display.fillRect(2,2,124,13,SSD1306_WHITE);

  display.setTextColor(SSD1306_BLACK);
  display.setCursor(4,4);
  display.print("= MISSION COMPLETE =");

  display.setTextColor(SSD1306_WHITE);

  display.setCursor(4,20);
  display.print("Dist:");
  display.print(distanceTravelled,2);
  display.print("m");

  display.setCursor(4,30);
  display.print("Avg:");
  display.print(averageSpeed,2);
  display.print("m/s");

  display.setCursor(4,40);
  display.print("Obs:");
  display.print(obstacles);

  display.setCursor(4,50);
  display.print("T:");
  display.print(turns);
  display.print(" L:");
  display.print(leftTurns);
  display.print(" R:");
  display.print(rightTurns);

  display.display();
}

void setup(){
  Serial.begin(9600);

  pinMode(ENA,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENB,OUTPUT);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(ENCODER_PIN,INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoderISR, RISING);

  scanner.attach(SERVO_PIN);
  scanner.write(SERVO_CENTER);

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  startupCountdown();

  missionStartTime=millis();
  lastSpeedTime=millis();
}

void loop(){

  if(missionComplete) return;

  updateDistanceData();
  updateSpeedData();

  if(distanceTravelled >= MISSION_DISTANCE){
    finishMission();
    return;
  }

  long frontDistance = getDistance();

  if(frontDistance > SAFE_DISTANCE){
    moveForward();
  }
  else{

    stopRobot();
    obstacles++;
    showScanningScreen();

    int leftDistance = scanLeft();
    int rightDistance = scanRight();

    Serial.print("L=");
    Serial.print(leftDistance);
    Serial.print(" R=");
    Serial.println(rightDistance);

    if(abs(leftDistance-rightDistance)<=5){
      turnRight();
      rightTurns++;
      turns++;
    }
    else if(leftDistance > rightDistance){
      turnLeft();
      leftTurns++;
      turns++;
    }
    else{
      turnRight();
      rightTurns++;
      turns++;
    }

    // Immediately resume forward motion
    moveForward();
  }

  updateOLED();
}
