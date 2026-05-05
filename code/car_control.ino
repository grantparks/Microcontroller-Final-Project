#include <MPU9255.h>  
#include <Wire.h>    


// Motor driver pins
const int PWM2A = 11;     // Motor 1 speed
const int PWM2B = 3;      // Motor 2 speed
const int PWM0A = 6;      // Motor 3 speed
const int PWM0B = 5;      // Motor 4 speed
const int DIR_CLK = 4;    // Shift register clock
const int DIR_EN = 7;     // Motor driver enable
const int DATA = 8;       // Shift register data
const int DIR_LATCH = 12; // Shift register latch


// Direction  sent to the shift register
const int Move_Forward = 39;   // Drive forward
const int Move_Backward = 216;  // Drive backward
const int Left_Rotate = 106;  // Spin left in place
const int Right_Rotate = 149;  // Spin right in place
const int Stop = 0;    // Stop all motors


// Accelerometer variables
MPU9255 mpu;
float Ax, Ay, Az;
float zeroffsetFB = 0;  // Forward/backward calibration offset
float zeroffsetLR = 0;  // Left/right calibration offset


int baseSpeed = 110;  // Motor speed out of 225


// Sends speed and direction to all 4 motors
void Motor(int Dir, int s1, int s2, int s3, int s4) {
  analogWrite(PWM2A, s1);
  analogWrite(PWM2B, s2);
  analogWrite(PWM0A, s3);
  analogWrite(PWM0B, s4);
  // Send direction byte to shift register
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);
  digitalWrite(DIR_LATCH, HIGH);
}


void setup() {
  Serial.begin(115200);


  // Check MPU9255 is connected on I2C
  Wire.begin();
  Wire.beginTransmission(0x68);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("MPU found at 0x68");
  } else {
    Serial.println("MPU NOT found - check wiring");
  }


  // Initialise MPU9255
  if (mpu.init()) {
    Serial.println("MPU failed");
  } else {
    Serial.println("MPU ready");
  }
  mpu.set_acc_scale(scale_2g);  // Set to ±2G range


  // Set motor driver pins as outputs
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM0B, OUTPUT);
}


void loop() {
  // Read accelerometer and convert to G force
  mpu.read_acc();
  Ax = (float)mpu.ax / 16384.0;
  Ay = (float)mpu.ay / 16384.0;
  Az = (float)mpu.az / 16384.0;


  // Calculate tilt angles in degrees
  // Axes remapped to match VCC-forward mounting orientation
  float angleFB = atan2(-Ay, Az) * (180.0 / PI) - zeroffsetFB;
  float angleLR = atan2(-Ax, Az) * (180.0 / PI) - zeroffsetLR;


  Serial.print("FB: "); Serial.print(angleFB);
  Serial.print(" | LR: "); Serial.println(angleLR);


  int threshold  = 25;          // Ignore tilts smaller than 25 degrees
  int speedLeft  = baseSpeed;
  int speedRight = baseSpeed;


  if (angleFB > threshold) {
    // Tilt forward — drive forward
    Serial.println("FORWARD");
    if (angleLR > threshold) {
      Serial.println("TURN RIGHT");
      speedRight = baseSpeed * 0.5;  // Slow right side to curve
    }
    else if (angleLR < -threshold) {
      Serial.println("TURN LEFT");
      speedLeft = baseSpeed * 0.5;   // Slow left side to curve
    }
    Motor(Move_Forward, speedLeft, speedRight, speedLeft, speedRight);
  }
  else if (angleFB < -threshold) {
    // Tilt backward — drive backward
    Serial.println("BACKWARD");
    if (angleLR > threshold) {
      Serial.println("TURN RIGHT");
      speedRight = baseSpeed * 0.5;
    }
    else if (angleLR < -threshold) {
      Serial.println("TURN LEFT");
      speedLeft = baseSpeed * 0.5;
    }
    Motor(Move_Backward, speedLeft, speedRight, speedLeft, speedRight);
  }
  else if (angleLR > threshold) {
    // Tilt right only — rotate right in place
    Serial.println("ROTATE RIGHT");
    Motor(Right_Rotate, baseSpeed, baseSpeed, baseSpeed, baseSpeed);
  }
  else if (angleLR < -threshold) {
    // Tilt left only — rotate left in place
    Serial.println("ROTATE LEFT");
    Motor(Left_Rotate, baseSpeed, baseSpeed, baseSpeed, baseSpeed);
  }
  else {
    // No tilt detected — stop
    Serial.println("STOP");
    Motor(Stop, 0, 0, 0, 0);
  }


  delay(100);  // Update 10 times per second
