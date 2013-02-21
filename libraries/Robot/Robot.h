#ifndef Robot_h
#define Robot_h

#include <Arduino.h>
#include <Position.h>
#include <Motion.h>
#include <DistanceSensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>

#define MOUSE_SCALE 85
#define TIME_OUT 30000
#define TOLERANCE_ANGLE 0.1
#define DISTANCE_MARGIN 15
#define ACCELERATION_TIME 50



#define ACCELERATION_SPEED 20
#define CRUISE_SPEED 10

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
	// temporary
	bool rotateRight(double angleRad);
private:
	Position position;
	Motion motion;
	DistanceSensor highDistanceTop;
	DistanceSensor highDistanceBottom;
	DistanceSensor proximity;
	LineSensor lineSensor;
	Feet feet;
	Remote remote;
	void enterPanicState();
  bool rotateToFreeDirection();
  bool moveBackward(double distanceToDo);
  bool canMoveForward();
  bool rotateLeft(double angleRad);
  bool tryToRefindEgg();
  bool isOnBlackLine();
	bool moveForwardKeepingDirection(double orientation);
	bool areCloseAngles(double angle1, double angle2);

};

#endif
