#ifndef Robot_h
#define Robot_h

#include <Arduino.h>
#include <Position.h>
#include <Motion.h>
#include <DistanceSensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>
#include <Eyes.h>

// States
#define WAIT 0
#define EXPLORE 1
#define EXPLORE_SCAN 2
#define EXPLORE_CHANGE_POSITION 21
#define EXPLORE_REACHEGG 3
#define CATCH 4
#define COMEBACK 5
#define COMEBACK_POSITIONING 6
#define COMEBACK_APPROACHING 7
#define COMEBACK_LINEFOLLOWING 8
#define COMEBACK_LINESEARCHING 9
#define DEPOSIT 10
#define PANIC 11


#define MOUSE_SCALE 85
#define TIME_OUT 30000
#define LONG_TIME_OUT 45000
#define SHORT_TIME_OUT 10000
#define TASK_TIME_OUT 2500
#define m500_TIME_OUT 500
#define m100_TIME_OUT 100
#define TOLERANCE_ANGLE 0.15
#define DISTANCE_MARGIN 20
#define DRIFT_CONSTANT_MULTIPLIER 100




// Approach results
// TODO move this constants in robot
#define APPROACH_FOUND_LINE 1
#define APPROACH_NOT_FOUND_LINE 2
#define APPROACH_FAILED 3

#define SPEED_INCREMENT 10
#define SPEED_INCREMENT_ROTATION 20
#define CRUISE_SPEED 40
#define ROTATIONAL_CRUISE_SPEED 50

#define DISTANCE_FOR_ADJUSTING_ANGLE 10
#define DECELERATING_DISTANCE 11

#define CM_OUT_OF_HOME 30


class Robot {
public:
	Robot();
	void init();
	void start();
	bool scanForEgg();
	bool changePosition();
	bool reachEgg();
	bool catchEgg();
	bool positioningTowardHome();
	int tryToApproach();
	bool searchLine();
	bool followLineToHome();
	bool deposit();
	bool newInit();
	bool escapeFromPanic();
	bool escapeFromWait();
	// temporary for test
	bool rotateRight(double angleRad);
	bool rotateLeft(double angleRad);
	void recalibrate();
	bool rotateToFreeDirection();
	bool comeBackHomeSimple();
	bool changePositionSimple();
	bool searchEggSimple();
	bool checkPanicState();
private:
	Position position;
	Motion motion;
	DistanceSensor highDistanceTop;
	DistanceSensor highDistanceBottom;
	DistanceSensor proximity;
	LineSensor lineSensor;
	Feet feet;	
	Remote remote;
	Eyes eyes;
	bool eggOnBoard;
	bool adjustOrientation(double angleToFollow);
	void enterPanicState();
  	
 	bool moveBackward(double distanceToDo);
  	bool canMoveForward();
  
  	bool tryToRefindEgg();
  	bool isOnBlack();
	bool moveForwardKeepingDirection(double speed, double orientation);
	bool areCloseAngles(double angle1, double angle2, double tolerance);
	double distanceBetweenAngles(double angle1, double angle2);
	char readStrategy();
	bool isOnBlueLine();
	void rotateToAngle(double angle);
	bool checkSpeedChange();
	bool refindBlueLine();
	bool tryToRefindEggFromDistance();
	int freeRam();
	void fixFeet();
};

#endif
