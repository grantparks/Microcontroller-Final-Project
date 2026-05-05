# Microcontroller-Final-Project
Hand gesture controlled Arduino car using an MPU9255 accelerometer.
# Hand Gesture Controlled Arduino Car

## Overview
This project uses an MPU9255 accelerometer mounted on a glove to control an Arduino car with hand motion. Tilting the hand forward, backward, left, or right sends movement commands to the car.

## Features
- Hand gesture control
- Forward, backward, left, and right movement
- Dead zone to prevent unwanted motion
- Wired communication after Bluetooth was unreliable
- Motor speed control using Arduino PWM

## Hardware
- Arduino Uno
- MPU9255 accelerometer
- Motor driver shield
- DC motors
- Mecanum wheels
- 20 AWG stranded wire
- Breadboard and jumper wires
- Battery pack

## Repository Contents
- `code/car_control.ino` main Arduino car control code
- `code/accelerometer_test.ino` accelerometer test code with zero button and LED indicator
- `report/` final project report PDF
- `images/` project setup photos and flowchart

## How It Works
1. The accelerometer reads hand tilt.
2. The Arduino converts tilt into angle values.
3. A dead zone prevents small accidental movements.
4. The car moves forward, backward, left, or right based on tilt direction.
5. A wired connection was used because Bluetooth was unreliable during testing.

## Authors
- Grant Parks
- Harrison Costa
