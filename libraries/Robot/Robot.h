#ifndef Robot_h
#define Robot_h

#include <Arduino.h>
#include <Position.h>
#include <Motion.h>
#include <DistanceSensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>

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
#define TOLERANCE_ANGLE 0.1
#define DISTANCE_MARGIN 15
#define ACCELERATION_TIME 50
#define DRIFT_CONSTANT_MULTIPLIER 100




// Approach results
// TODO move this constants in robot
#define APPROACH_FOUND_LINE 1
#define APPROACH_NOT_FOUND_LINE 2
#define APPROACH_FAILED 3

#define ACCELERATION_SPEED 5
#define CRUISE_SPEED 20
#define ROTATIONAL_CRUISE_SPEED 2

#define DISTANCE_FOR_ADJUSTING_ANGLE 10
#define DECELERATING_DISTANCE 11


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
	// temporary for test
	bool rotateRight(double angleRad);
	bool rotateLeft(double angleRad);
	void recalibrate();
private:
	Position position;
	Motion motion;
	DistanceSensor highDistanceTop;
	DistanceSensor highDistanceBottom;
	DistanceSensor proximity;
	LineSensor lineSensor;
	Feet feet;
	Remote remote;
	bool adjustOrientation(double angleToFollow);
	void enterPanicState();
  	bool rotateToFreeDirection();
 	bool moveBackward(double distanceToDo);
  	bool canMoveForward();
  
  	bool tryToRefindEgg();
  	bool isOnBlackLine();
	bool moveForwardKeepingDirection(double speed, double orientation);
	bool areCloseAngles(double angle1, double angle2, double tolerance);
	double distanceBetweenAngles(double angle1, double angle2);
	char readStrategy();
	bool isOnBlueLine();
	void rotateToAngle(double angle);
	void checkSpeedChange();

};

#endif
