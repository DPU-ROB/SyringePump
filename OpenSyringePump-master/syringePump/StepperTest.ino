/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/


#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #1 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(400, 2);
int fw;
int state = 0;
int steps;
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(100);  // 100 rpm   
}

void loop() {
//  Serial.println("Single coil steps");
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    int sub_state = Serial.read();
    if(sub_state != 10){
      state = sub_state;
    }
  }

  if (state == 70){
    myMotor->step(steps, FORWARD, SINGLE);
    steps++; 
  }
  else if (state == 66){
    myMotor->step(steps, BACKWARD, SINGLE); 
    steps--;
  }
  else{
    Serial.println(steps);
  }
//  myMotor->step(2750, FORWARD, SINGLE); 
//
//  Serial.println("Double coil steps");
//  myMotor->step(50, FORWARD, DOUBLE); 
//  myMotor->step(50, BACKWARD, DOUBLE);
//  
//  Serial.println("Interleave coil steps");
//  myMotor->step(50, FORWARD, INTERLEAVE); 
//  myMotor->step(50, BACKWARD, INTERLEAVE); 
//  
//  Serial.println("Microstep steps");
//  myMotor->step(50, FORWARD, MICROSTEP); 
//  myMotor->step(50, BACKWARD, MICROSTEP);
}
