# Arduino Obstacle Avoidance Robot

## Overview

This project is a 4WD autonomous obstacle avoidance robot built using an Arduino Uno, HC-SR04 ultrasonic sensor, SG90 servo motor, and L298N motor driver.

The robot continuously scans its surroundings, detects obstacles, and automatically selects the clearest path to navigate without human intervention.

---

## Features

- Autonomous navigation
- Obstacle detection using ultrasonic sensor
- Left and right environment scanning using servo motor
- Automatic path selection
- Four-wheel drive system
- Battery-powered operation

---

## Components Used

| Component | Quantity |
|------------|------------|
| Arduino Uno | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| SG90 Servo Motor | 1 |
| L298N Motor Driver | 1 |
| TT Gear Motors | 4 |
| Robot Wheels | 4 |
| 4WD Chassis | 1 |
| 18650 Batteries | 3 |
| Jumper Wires | Multiple |

---

## Working Principle

1. The ultrasonic sensor measures the distance in front of the robot.
2. If no obstacle is detected, the robot moves forward.
3. When an obstacle is detected within a predefined threshold distance:
   - The robot stops.
   - The servo rotates the ultrasonic sensor to the left and right.
   - Distances on both sides are measured.
4. The robot compares both distances and chooses the direction with more free space.
5. The robot continues moving and repeats the process.

---

## Pin Connections

### HC-SR04

| Sensor Pin | Arduino Pin |
|------------|------------|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D10 |

### Servo Motor

| Servo Pin | Arduino Pin |
|------------|------------|
| Signal | D11 |
| VCC | 5V |
| GND | GND |

### L298N Motor Driver

| L298N Pin | Arduino Pin |
|------------|------------|
| IN1 | D2 |
| IN2 | D3 |
| IN3 | D4 |
| IN4 | D5 |

---

## Circuit Diagram

Added project photographs inside the images folder.

---

## Demonstration Video

https://www.youtube.com/watch?v=9A9Wnhvn_Kc
https://www.youtube.com/watch?v=WF7uMh7em00

---

## Future Improvements

- Bluetooth control
- Maze solving algorithm
- Room mapping
- MPU6050 integration
- Mobile application control

---

## Skills Learned

- Embedded Systems
- Arduino Programming
- Sensor Interfacing
- Motor Control
- Autonomous Navigation
- Electronics Prototyping

---

## Author

Nikshay Kumar
