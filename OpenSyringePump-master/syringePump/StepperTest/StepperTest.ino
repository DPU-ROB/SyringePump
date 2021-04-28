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
Adafruit_StepperMotor* motor2 = AFMS.getStepper(400, 2);
Adafruit_StepperMotor* motor1 = AFMS.getStepper(400, 1);

int state = 0;

unsigned long stepper1Steps = 0;
unsigned long stepper2Steps = 0;
unsigned long speed1;
unsigned long speed2;
char pumpNum;


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
      state = 6;
    } else {
      sendByte(63); //errorByte - tells gui to display error
      sendByte(30); //error number - tells gui what error to display
    }
    continue;
  } else if(state == 2){
     pumpNum = nextByte(1000);
     unsigned long steps = nextLong(1000);
     if(steps == -1){
      sendByte(63); //errorByte - tells gui to display error
      sendByte(31); //error - Number of steps not recieved
      state = 1;
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
        motor2->step(stepper2Steps - steps, FORWARD, SINGLE);
      } else if(steps > stepper1Steps){
        motor2->step(steps - stepper2Steps, BACKWARD, SINGLE);
      }
     } else {
       sendByte(63); //errorByte - tells gui to display error
       sendByte(32); //error number - //invalid pump number recieved
       state == 1;
       continue;
     }
     state = 3;
     continue;
  } else if(state == 3){
    while(Serial.available() < 1){
      if(millis() - startTime >= pullTime){
        state = 1;
        sendByte(69); //Status update number
        sendByte(80); //Pull complete
        break;
      }
    }
    char command = nextByte();
    if(command == -1){
      continue;
    } else if(command == 92) {
      state = 4; //go into pause state
    } else  if(command == 94) {
      sendByte(69);
      sendByte(81); //stop update status
      state = 1; //go back to initial state
    } else {
      sendByte(63);
      sendByte(33); //unexpected command number from GUI
    }
    continue;
  } else if(state == 4){ //Pause State
    if(pumpNum == 1){
      motor1->setSpeed(0); //stop motor 1
      state = 5;
    } else if(pumpNum == 2){
      motor2->setSpeed(0); //stop motor 2
      state = 5;
    } else {
      sendByte(63);
      sendByte(34); //No Pump Found Error
    }
    continue;
  } else if(state == 5){ //Wait for Resume State
    while(Serial.available() < 1);
    char bite = nextByte(1000);
    if(command == 94){
      sendByte(69);
      sendByte(81); //stop update status
      state = 1; //go back to initial state
    } else if(command == 93){ //go back to state 3
      if(pumpNum == 1){
        motor1->setSpeed(100); //resume motor 1
        state = 3;
      } else if(pumpNum == 2){
        motor2->setSpeed(100); //resume motor 2
        state = 3;
      } else {
        sendByte(63);
        sendByte(34); //No Pump Found Error
      }
      state = 3;
    } else {
      sendByte(67);
      sendByte(88); //Unexpected Command found
    }
    continue;
  } else if(state == 6){
     speed1 = nextLong(1000);
     speed2 = nextLong(1000);
     if(speed1 == -1){
      sendByte(63); //errorByte - tells gui to display error
      sendByte(35); //error - Number of steps not recieved
      state = 1;
      continue;
     }
     if(speed2 == -1){
        sendByte(63); //errorByte - tells gui to display error
        sendByte(36); //error - Number of steps not recieved
        state = 1;
        continue;
     }
     
     //set speeds for the motors
     motor1->setSpeed(speed1);
     motor2->setSpeed(speed2);

     //step the required amount of steps to get to zero
     motor1->step(stepper1Steps, FORWARD, SINGLE);
     motor2->step(stepper2Steps, FORWARD, SINGLE);
     
     state = 7;
     continue;
  } else if(state == 7){
    while(Serial.available() < 1);
    char bite = nextByte(1000);
    if(command == 94){
      sendByte(69);
      sendByte(81); //stop update status
      state = 1; //go back to initial state execution stopped
    } else if(command == 97){ //go to Pause State
      state = 8;
    } else {
      sendByte(67);
      sendByte(88); //Unexpected Command found
    }
    continue;
  } else if (state == 8){
    motor1->setSpeed(0); //Pause both Motors
    motor2->setSpeed(0);
    while(Serial.available() < 1);
    char bite = nextByte(1000);
    if(command == 94){
      sendByte(69);
      sendByte(81); //stop update status
      state = 1; //go back to initial state
    } else if(command == 93){ //go back to state 7
      sendByte(69);
      sendByte(82); //resume push by setting motor speeds back to normal
      motor1->setSpeed(speed1);
      motor2->setSpeed(speed2);
      state = 7;
    } else {
      sendByte(67);
      sendByte(88); //Unexpected Command found
    }
    continue;
  } else {
    sendByte(67);
    sendByte(98);
    sendByte(state); //error unknown state
    state = 1
  }
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
