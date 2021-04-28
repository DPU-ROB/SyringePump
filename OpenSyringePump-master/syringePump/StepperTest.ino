/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

// 400x/391 cc/minutes 
// x is the rev per minutes

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #1 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(400, 2);
int state = 0;

unsigned long stepper1Steps = 0;
unsigned long stepper2Steps = 0;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(100);  // 100 rpm   
}

void loop() {
//  Serial.println("Single coil steps");

  if(state == 0){ //Initial Program State
    while(Serial.available() <  1);
    char ack = nextByte(1000);
    if(ack == 42){
      sendByte(43);
      ack = nextByte(1000);
      if(ack == 44){
         state = 1;   
      }
    }
    continue;
  } else if (state == 1) { //Main Program State
    while(Serial.available() < 1);
    char bite = nextByte(1000);
    if(bite == 90){
      state = 2;
    } else if(bite == 91){
      state = 4;
    } else {
      sendByte(63); //errorByte - tells gui to display error
      sendByte(30); //error number - tells gui what error to display
    }
    continue;
  } else if(state == 2){
     char pumpNum = nextByte(1000);
     unsigned long steps = nextLong(1000);
     if(steps == -1){
      sendByte(63); //errorByte - tells gui to display error
      sendByte(31); //error - Number of steps not recieved
      state == 1;
      continue;
     }
     if(pumpNum == 1){
      if(steps < stepper1Steps){
        motor1->step(stepper1Steps - steps, FORWARD, SINGLE);
      } else if(steps > stepper1Steps){
        motor1->step(steps - stepper1Steps, BACKWARD, SINGLE);
      }
     } else if(pumpNum == 2){
      if(steps < stepper2Steps){
        motor1->step(stepper2Steps - steps, FORWARD, SINGLE);
      } else if(steps > stepper1Steps){
        motor1->step(steps - stepper2Steps, BACKWARD, SINGLE);
      }
     } else {
       sendByte(63); //errorByte - tells gui to display error
       sendByte(32); //error number - //invalid pump number recieved
       state == 1;
       continue;
     }
     state == 3;
     continue;
  } else if(state == 3){
    while(millis() >= targetTime){
      
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

unsigned long nextLong(int del){
  unsigned long startTime = millis();
  while(Serial.available() < 4){
    if(millis() - startTime >= del){
      return -1;
    }
  }
  unsigned long val = 0;
  val |= (Serial.read() & 0xff) << 24;
  val |= (Serial.read() & 0xff) << 16;
  val |= (Serial.read() & 0xff) << 8;
  val |= (Serial.read() & 0xff);
  return val;
}

void sendLong(long val){
  Serial.write((val >> 24) & 0xff);
  Serial.write((val >> 16) & 0xff);
  Serial.write((val >> 8) & 0xff);
  Serial.write(val & 0xff);
}

void sendByte(char bite){
  Serial.write(bite);
}

char nextByte(int del){
  unsigned long startTime = millis();
  while(Serial.available() < 1){
    if(millis() - startTime >= del){
      return -1;
    }
  }
  return Serial.read();
}
