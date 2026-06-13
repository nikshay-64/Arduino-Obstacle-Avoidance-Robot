
#include <Servo.h>

// ======================================================
// PIN DEFINITIONS
// ======================================================
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

// ======================================================
// SETTINGS
// ======================================================
const int SPEED_NORMAL = 90;
const int SPEED_SLOW   = 75;

const int DIST_STOP = 40;
const int DIST_SLOW = 60;
const int DIST_SAFE = 80;

const int SAMPLES = 5;
const int SAMPLE_GAP = 20;
const int SERVO_SETTLE = 450;

// 1 = Right
// -1 = Left
int lastTurnDirection = 1;

// ======================================================
// ULTRASONIC FUNCTIONS
// ======================================================
float rawPing() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0)
    return 999.0;

  return duration * 0.01715;
}

float getDistance() {

  float samples[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    samples[i] = rawPing();
    delay(SAMPLE_GAP);
  }

  for (int i = 1; i < SAMPLES; i++) {

    float key = samples[i];
    int j = i - 1;

    while (j >= 0 && samples[j] > key) {
      samples[j + 1] = samples[j];
      j--;
    }

    samples[j + 1] = key;
  }

  return samples[SAMPLES / 2];
}

float scanAt(int angle) {

  scanner.write(angle);
  delay(SERVO_SETTLE);

  return getDistance();
}

// ======================================================
// MOTOR FUNCTIONS
// ======================================================
void setSpeed(int speedValue) {

  speedValue = constrain(speedValue, 0, 255);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void moveForward(int speedValue) {

  setSpeed(speedValue);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(int speedValue) {

  setSpeed(speedValue);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft(int speedValue) {

  setSpeed(speedValue);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight(int speedValue) {

  setSpeed(speedValue);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ======================================================
// OBSTACLE AVOIDANCE
// ======================================================
void avoidObstacle() {

  stopRobot();
  delay(200);

  moveBackward(SPEED_SLOW);
  delay(500);

  stopRobot();
  delay(200);

  float leftDistance   = scanAt(170);
  float centerDistance = scanAt(90);
  float rightDistance  = scanAt(10);

  scanner.write(90);
  delay(SERVO_SETTLE);

  if (centerDistance > DIST_SAFE) {

    Serial.print(centerDistance);
    Serial.print("\t\t");
    Serial.print(leftDistance);
    Serial.print("\t\t");
    Serial.print(rightDistance);
    Serial.println("\t\tMove Forward");

    moveForward(SPEED_NORMAL);
    delay(300);
    return;
  }

  if (leftDistance < DIST_STOP &&
      centerDistance < DIST_STOP &&
      rightDistance < DIST_STOP) {

    Serial.print(centerDistance);
    Serial.print("\t\t");
    Serial.print(leftDistance);
    Serial.print("\t\t");
    Serial.print(rightDistance);
    Serial.println("\t\tReverse and Re-scan");

    if (lastTurnDirection == 1) {

      turnLeft(SPEED_NORMAL);
      delay(1000);
      lastTurnDirection = -1;

    } else {

      turnRight(SPEED_NORMAL);
      delay(1000);
      lastTurnDirection = 1;
    }

    stopRobot();
    delay(200);

    return;
  }

  if (leftDistance >= centerDistance &&
      leftDistance >= rightDistance) {

    Serial.print(centerDistance);
    Serial.print("\t\t");
    Serial.print(leftDistance);
    Serial.print("\t\t");
    Serial.print(rightDistance);
    Serial.println("\t\tTurn Left");

    turnLeft(SPEED_NORMAL);
    delay(750);

    lastTurnDirection = -1;
  }

  else if (rightDistance >= centerDistance &&
           rightDistance >= leftDistance) {

    Serial.print(centerDistance);
    Serial.print("\t\t");
    Serial.print(leftDistance);
    Serial.print("\t\t");
    Serial.print(rightDistance);
    Serial.println("\t\tTurn Right");

    turnRight(SPEED_NORMAL);
    delay(750);

    lastTurnDirection = 1;
  }

  else {

    Serial.print(centerDistance);
    Serial.print("\t\t");
    Serial.print(leftDistance);
    Serial.print("\t\t");
    Serial.print(rightDistance);
    Serial.println("\t\tMove Forward");

    moveForward(SPEED_NORMAL);
    delay(300);
  }

  stopRobot();
  delay(200);
}

// ======================================================
// SETUP
// ======================================================
void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  scanner.attach(SERVO_PIN);

  scanner.write(90);
  delay(1000);

  Serial.begin(9600);

  Serial.println("Front(cm)\tLeft(cm)\tRight(cm)\tAction");
}

// ======================================================
// MAIN LOOP
// ======================================================
void loop() {

  float frontDistance = getDistance();

  if (frontDistance >= DIST_SAFE) {

    Serial.print(frontDistance);
    Serial.println("\t\t-\t\t-\t\tMove Forward");

    moveForward(SPEED_NORMAL);
  }

  else if (frontDistance > DIST_STOP) {

    Serial.print(frontDistance);
    Serial.println("\t\t-\t\t-\t\tMove Slow");

    moveForward(SPEED_SLOW);
  }

  else {

    avoidObstacle();
  }
}

