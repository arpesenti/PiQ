#include <Robot.h>
#include <Position.h>
#include <Motion.h>
#include <ProximitySensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Servo.h>
#include <IRremote.h>
#include <ADJDS311.h>

// States
#define WAIT 0
#define EXPLORE 1
#define EXPLORE_SCAN 2
#define EXPLORE_REACHEGG 3
#define CATCH 4
#define COMEBACK 5
#define COMEBACK_POSITIONING 6
#define COMEBACK_APPROACHING 7
#define COMEBACK_LINEFOLLOWING 8
#define COMEBACK_LINESEARCHING 9
#define DEPOSIT 10
#define PANIC 11

// Robot
Robot robot;

void setup() {
  
}

void loop() {
  
}
