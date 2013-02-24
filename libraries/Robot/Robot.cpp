#include "Robot.h"

extern int state;
extern int cruiseSpeed;

Robot::Robot() {

}

void Robot::init() {
	motion.init();
	highDistanceTop.initHighDistanceTop();
	highDistanceBottom.initHighDistanceBottom();
	proximity.initProximity();
	//lineSensor.init();
	//feet.init();
	//remote.init();
	position.calibrate(motion, false);
	cruiseSpeed = CRUISE_SPEED;
}

void Robot::start(){
	position.init();

	// ********************************* no need to check obstacles and lines when exiting from home
	int speed = 20; // 20%

	double distance = 0; // made distance cm
	double NewDistanceLimit = 10; // cm
	double distanceToCover = 50;
	
	position.update();
	double angleToFollow = position.getOrientation();
	
	unsigned long startTime = millis();
	unsigned long elapsedTime = 0;
	
	bool decelerated = false;
	
	motion.moveForward(speed);
	

	while (abs(distance) < distanceToCover){
		position.update();
		// check time out
		elapsedTime = millis() - startTime;
		position.update();
		if (elapsedTime  > TIME_OUT){
			enterPanicState();
			break;
		}
		position.update();
		// check angle
		if ( distance > NewDistanceLimit){
			motion.stop();
			position.update();
			adjustOrientation(angleToFollow); // follow initial angle
			position.getOrientation(); // update actual angle to compute exact coordinates
			position.update();
			NewDistanceLimit = NewDistanceLimit + DISTANCE_FOR_ADJUSTING_ANGLE;
			position.update();
			if(!decelerated && distanceToCover - distance <= DECELERATING_DISTANCE){
				speed = speed / 2;
				decelerated = true;
			}
			position.update();
			motion.moveForward(speed);
		}

		position.update();
		distance = position.getY() / MOUSE_SCALE; // it moves with orientation PI/2
		position.update();
		
	}
	motion.stop();
	//position.reset(); // da rimettere!!!!
	Serial.println(position.getX());
  Serial.println(position.getY());
  Serial.println(position.getOrientation());
	//*******************************

}

bool Robot::adjustOrientation(double angleToFollow){
	motion.stop();
	position.update();
	delay(10);
	double deltaRad;
	double currentOrientation = position.getOrientation();

	if (angleToFollow > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToFollow - currentOrientation; // positive value
		if (deltaRad <= PI)
			rotateLeft(deltaRad);
		else
			rotateRight(2*PI - deltaRad);
		return true;
	}else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		if (deltaRad <= PI)		
			rotateRight(deltaRad);
		else
			rotateLeft(2*PI - deltaRad);
		return true;
	}else
		return true;
		
}

// scan rotating left
bool Robot::scanForEgg() {
	double startAngle = position.getOrientation();
	double arriveAngle = startAngle - TOLERANCE_ANGLE;
	double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);
	double distanceBottom;
	
	int speed = 10;	
	unsigned long startTime = millis();
	motion.rotateLeft(speed);
	while (arriveAngle >= 0 ? 
				(position.getOrientation() < arriveAngleMod || position.getOrientation() > startAngle):
				(position.getOrientation() < arriveAngleMod && position.getOrientation() > startAngle)){
		
		distanceBottom = highDistanceBottom.distance();
		if(distanceBottom < 100){
			double distanceTop = highDistanceTop.distance();
			if(distanceTop - distanceBottom > DISTANCE_MARGIN){
				motion.stop();
				position.update();
				return true;
			}
		}

		position.update();
		if ( millis() - startTime > TIME_OUT){
			enterPanicState();
			break;
		}

		if( isOnBlackLine()){
			rotateRight(PI/2);
			return false;
		}
	}

	return false;
	
}

bool Robot::changePosition(){
	motion.stop();
	int startX = position.getX();
	int startY = position.getY();
	double distance = 0.0;
	double NewDistanceLimit = 10.0 ;
	double distanceToCover = 30;

	double deltaX = 0;
	double deltaY = 0;
	
	bool decelerated = false;

	int speed = 25;
	
	if(!rotateToFreeDirection()) 
		return false; // not able to find a free direction - enter in panic state

	double initialOrientation = position.getOrientation();
	Serial.print("initial orientation: ");
	Serial.println(initialOrientation);

	motion.moveForward(speed);
	while (distance < distanceToCover){
		// check black line
		if(isOnBlackLine()){
			motion.stop();
			position.update();
			moveBackward(10 * MOUSE_SCALE); // distance to do backward
			position.update();
			if(!rotateRight(PI/2)) 
				return false; // panic state	
			motion.moveForward(speed);	
			
		}
		position.update();
		// check obstacles
		if(!canMoveForward()){
			motion.stop();
			position.update();
			delay(4000); // wait for passing robot
			if(!canMoveForward()){
				if(!rotateToFreeDirection()) 
					return false; // panic state
			}
			position.update();
			motion.moveForward(speed);
		}
		
		// update made distance
		position.update();
		//Serial.print("X e Y  : ");
		//position.update();
		//Serial.print(position.getX());
		//Serial.print(" ");
		deltaX = square(position.getX()-startX);
		position.update();
		//Serial.println(position.getY());
		deltaY = square(position.getY()-startY);
		position.update();
		distance = sqrt(deltaX  + deltaY ) / MOUSE_SCALE;
		position.update();
		
		// update angle
		if ( distance > NewDistanceLimit){
			motion.stop();
			position.update();
			delay(100);
			position.update();
			//update angle
			position.getOrientation();
			NewDistanceLimit = NewDistanceLimit + DISTANCE_FOR_ADJUSTING_ANGLE;
			position.clearMouseBuffer();
			
	
			if(!decelerated && distanceToCover - distance <= DECELERATING_DISTANCE){
				speed = speed / 2;
				decelerated = true;
			}
			motion.moveForward(speed);
		}
	}
	
	motion.stop();
	position.update();

	Serial.println("position changed" );
	Serial.println(position.getX());
	Serial.println(position.getY());
	Serial.println(position.getOrientation());
	return true;
}


bool Robot::reachEgg() {
	double oldDistanceBottom = highDistanceBottom.distance();
	double newDistanceBottom = oldDistanceBottom;
	double distanceProximity = proximity.distance();
	bool waitForPassingRobot = false;
	double initialOrientation = position.getOrientation();

	//test**********************************************
	newDistanceBottom = 20;
	//******************************************************
	
	if (newDistanceBottom > 30 && distanceProximity > 30){
		while(newDistanceBottom < 30){
			moveForwardKeepingDirection(CRUISE_SPEED, initialOrientation);
			position.update();
			if(newDistanceBottom - oldDistanceBottom > 1){
				motion.stop();
				position.update();
				return false;
			}
			oldDistanceBottom = newDistanceBottom;
			newDistanceBottom = highDistanceBottom.distance();
			
			double distanceTop = highDistanceTop.distance();
			if(distanceTop - newDistanceBottom < DISTANCE_MARGIN){
				motion.stop();
				position.update();
				if(!waitForPassingRobot){
					waitForPassingRobot = true;
					delay(3000);
					continue;
				}else
					return false;
			}
		}
	}


	double oldProx= proximity.distance();
	double newProx = oldProx;
	
	initialOrientation = position.getOrientation();
	Serial.print("prox distance: ");	
	Serial.println(newProx);
	
	while(newProx > 5){
		moveForwardKeepingDirection(CRUISE_SPEED, initialOrientation);
		position.update();


		if (newProx >= 35 && oldProx >= 35){
			motion.stop();
			position.update();			
			return false;
		}

		if(newProx - oldProx > 1 ){
			motion.stop();
			position.update();
			if(tryToRefindEgg() == false)
				return false;
			else
				initialOrientation = position.getOrientation();
			
		}

		oldProx = newProx;
		newProx = proximity.distance();		
		Serial.print("prox distance: ");	
		Serial.println(newProx);

	}
	
	motion.stop();
	position.update();
	return true;
	
}
bool Robot::catchEgg() {
	// PRECONDITION robot near the egg
	double eggDistance = proximity.distance();
	if (eggDistance > 8)
		return false; //egg too far
	feet.close(50);
	eggDistance = proximity.distance();
	if (eggDistance > 4) 
		return true; //egg on board
	else {
		feet.open(); //egg missed
		return false;
	}
}


bool Robot::positioningTowardHome() {
	double vectorPositionAngle = atan2(position.getY(), position.getX());
	Serial.print("X e Y: ");
	Serial.println(position.getX());
	Serial.println(position.getY());

	if (vectorPositionAngle < 0)
		vectorPositionAngle += 2*PI;
	Serial.print("Vector: ");
	Serial.println(vectorPositionAngle);
	double angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
	Serial.print("Follow: ");
	Serial.println(angleToFollow);
	double currentOrientation = position.getOrientation();
	double deltaRad;

	if (angleToFollow > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToFollow - currentOrientation; // positive value
		if (deltaRad <= PI)
			rotateLeft(deltaRad);
		else
			rotateRight(2*PI - deltaRad);
		return true;
	}else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		if (deltaRad <= PI)		
			rotateRight(deltaRad);
		else
			rotateLeft(2*PI - deltaRad);
		return true;
	}else
		return true;
	
}

int Robot::tryToApproach() {
// da considerare se incontro ostacoli...
// per ora si aggiorna costantemente l'angolo...

	int radius = 2;
	double deltaX = 0;
	double deltaY = 0;
	
	double distanceNew = sqrt(square(position.getX()) + square(position.getY())) / MOUSE_SCALE;
	double distanceOld = distanceNew;
	double currentOrientation = position.getOrientation();
	double vectorPositionAngle = atan2(position.getY(), position.getX());
	double angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
	int speed = CRUISE_SPEED;
	
	if (angleToFollow < 0)
		angleToFollow += 2*PI;
	
//************************************** 
double NewDistanceLimit = distanceNew - DISTANCE_FOR_ADJUSTING_ANGLE;
	
	motion.moveForward(speed);
	while(distanceNew > radius){
		if(!canMoveForward()){
			Serial.println("Can not move forward");
			motion.stop();
			position.update();
			delay(4000);
			if(canMoveForward()){
				motion.moveForward(speed);
				continue;
			}
			
			if(!changePosition())
				return APPROACH_FAILED; //panic state
			position.update();
			distanceOld = distanceNew;
			distanceNew = sqrt(square(position.getX()) + square(position.getY()));
			vectorPositionAngle = atan2(position.getY(), position.getX());
			angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
		}
		position.update();
		if (distanceNew < 3*radius)
			speed = map(distanceNew, 0, 3 * radius, 0, CRUISE_SPEED);
		position.update();		
		if (distanceNew > distanceOld + DISTANCE_MARGIN){
			motion.stop();
			position.update();
			return APPROACH_FAILED; // need to repositioning toward home
		}
		
		position.update();
		distanceOld = distanceNew;
		position.update();
		deltaX = square(position.getX());
		position.update();
		deltaY = square(position.getY());
		position.update();
		distanceNew = sqrt(deltaX + deltaY) /	MOUSE_SCALE;
		position.update();
		vectorPositionAngle = atan2(position.getY(), position.getX());
		position.update();
		angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
		position.update();
		
		//Serial.print("Distance, vector, angle to follow: ");
		//position.update();
		//Serial.println(distance);
		//position.update();
		//Serial.println(vectorPositionAngle);
		//position.update();
		//Serial.println(angleToFollow);
		//position.update();
		//Serial.print("Postion x and y");
		//position.update();
		//Serial.println(position.getX());
		//position.update();
		//Serial.println(position.getY());
		//position.update();
		if ( distanceNew < NewDistanceLimit){
			motion.stop();
			delay(10);
			position.update();
			adjustOrientation(angleToFollow);
			NewDistanceLimit = NewDistanceLimit - DISTANCE_FOR_ADJUSTING_ANGLE;
			if( distanceNew <= 3 * radius )
				speed = speed / 2;
			motion.moveForward(speed);
		}
	}
			
	motion.stop();
	position.update();
	return APPROACH_FOUND_LINE;


//****************************************
}


bool Robot::searchLine() {
	// PRECONDITION: robot nearby the home

	// rotate toward home, PI
	rotateToAngle(PI);

	// search line in a zig zag way
	unsigned long startTime = millis();
	while (!isOnBlueLine() && millis()-startTime < TIME_OUT) {
		position.update();
		int x = position.getX();
		int y = position.getY();
		double distance = 0;
		while (canMoveForward() && !isOnBlackLine() && distance < 30*MOUSE_SCALE && millis()-startTime < TIME_OUT) {
			position.update();
			motion.moveForward(CRUISE_SPEED);
			distance = sqrt(square(x - position.getX()) + square(y - position.getY()));
			position.update();
		}
		motion.stop();
		position.update();
		rotateLeft(PI); // dietrofront 
		if (isOnBlueLine()) {
			motion.stop();
			position.update();
			return true;
		}
		if (millis()-startTime > TIME_OUT) {
			motion.stop();
			position.update();
			return false;
		}
	}
	return false;
}

bool Robot::followLineToHome() {
	// PRECONDITION: is on blue line, and there is no obstacle in front other then the home...

	if (!isOnBlueLine()) {
		return false;
	}
	int drift = 0;
	double alpha = 0.2; // constant to calibrate the amount of drift
	unsigned long startTime = millis();
	while (isOnBlueLine() && highDistanceTop.distance() > 30 && millis()-startTime < TIME_OUT ) {
		drift = lineSensor.rightReflectance() - lineSensor.leftReflectance();
		motion.moveForwardWithDrift(CRUISE_SPEED, alpha*drift);
		position.update();
		if (!isOnBlueLine() || millis()-startTime > TIME_OUT) {
			motion.stop();
			position.update();
			return false;
		}
	}
	return true;
}

bool Robot::deposit() {
	// PRECONDITION: has the home in front

	feet.open();
	int bumpDuration = 100;
	motion.moveForward(CRUISE_SPEED);
	delay(bumpDuration); // small bump to the just released egg
	motion.stop();
	motion.moveBackward(CRUISE_SPEED);
	delay(bumpDuration);
	motion.stop();
	return true; // could it fail?
}

bool Robot::newInit() {
	// PRECONDITION: egg released, facing the home

	moveBackward(30);
	rotateToAngle(PI/2);
	position.reset();
	return true; // could it fail?
}

bool Robot::escapeFromPanic() {
	while (state == PANIC) {
		remote.update();
		char command = remote.strategy();
		if (command == NORMAL_STRATEGY) {
			motion.stop();
			state = EXPLORE_SCAN;
		} else if (command == REMOTE_ROTATELEFT) {
			motion.stop();
			rotateLeft(TOLERANCE_ANGLE);
		} else if (command == REMOTE_ROTATERIGHT) {
			motion.stop();
			rotateRight(TOLERANCE_ANGLE);
		} else if (command == REMOTE_MOVEFORWARD) {
			motion.stop();
			motion.moveForward(CRUISE_SPEED);
		} else if (command == REMOTE_MOVEBACKWARD) {
			motion.stop();
			motion.moveBackward(CRUISE_SPEED);
		} else if (command == REMOTE_STOP) {
			motion.stop();
		}
	}
}

char Robot::readStrategy() {
	remote.update();
	char strategy = remote.strategy();
	if (strategy == PANIC) {
		return PANIC;
	} else {
		return NORMAL_STRATEGY;
	}
}

void Robot::enterPanicState(){
	// turn on some led indicating it's in panic
	motion.stop();
	state = PANIC;
}

bool Robot::rotateToFreeDirection(){
	if(canMoveForward()) 
		return true;
	double distance = 100; // distance of obstacles
	int speed = ROTATIONAL_CRUISE_SPEED;

	// cycles eventually decreasing distance of obstacles
	while( distance > 0){
		unsigned long startTime = millis();
		double startAngle = position.getOrientation();
		double arriveAngle = startAngle - TOLERANCE_ANGLE;
		double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);

		bool switchRotation = false;
		motion.rotateLeft(speed); // 10% speed
		while (highDistanceTop.distance() < distance && 
					(arriveAngle >= 0 ? 
						(position.getOrientation() < arriveAngleMod || position.getOrientation() > startAngle):
						(position.getOrientation() < arriveAngleMod && position.getOrientation() > startAngle))){	
			
			
			if (isOnBlackLine()){
				motion.stop();
				if(!switchRotation){
					switchRotation = true;
					startTime = millis();
					motion.rotateRight(speed);
				}else{
					break;
				}
			}
			
			if ( millis() - startTime > TIME_OUT ){
				if(switchRotation){	
					motion.stop();		
					enterPanicState();
					return false;
				}else{
					switchRotation = true;
					startTime = millis();
					motion.rotateRight(speed);
				}
			}
		}
		
		motion.stop();
		position.clearMouseBuffer();

		if (highDistanceTop.distance() >= distance){
			return true;
		}
		
		distance -= 20; // repeat the cycle with a smaller distance of obstacles
	}

	enterPanicState();
	return false;
	
}
	
bool Robot::isOnBlackLine(){
	//test
	return false;
	//return lineSensor.color() == 'k';
}


// rotate right : (+ angleRad)
bool Robot::rotateRight(double angleRad){
// angleRad must be between 0 and 2*PI

	Serial.println("enter rotate right");
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;

	double speed = ROTATIONAL_CRUISE_SPEED;
	double initialOrientation = position.getOrientation();
	// toAngle must be between 0 and 2*PI
	double toAngle = fmod(2*PI + position.getOrientation() - angleRad, 2*PI);

	Serial.print("initial orientation : ");
	Serial.println(initialOrientation);
	Serial.print("toAngle : ");
	Serial.println(toAngle);

	bool decelerated = false;
	motion.rotateRight(ACCELERATION_SPEED);
	
	while (!areCloseAngles(position.getOrientation(),toAngle, TOLERANCE_ANGLE)){ 
		Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (areCloseAngles(position.getOrientation(),toAngle, 4*TOLERANCE_ANGLE)) {
			speed = map(distanceBetweenAngles(position.getOrientation(), toAngle), 0, PI, 0, ROTATIONAL_CRUISE_SPEED);
			if (! decelerated && millisFromStart > ACCELERATION_TIME) {
				motion.rotateRight(speed);
				decelerated = true;
			}
		}

		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateRight(ROTATIONAL_CRUISE_SPEED);
			decelerated = true;
		}
		
		if ( millisFromStart  > TIME_OUT ){
			motion.stop();	
			position.clearMouseBuffer();	
			enterPanicState();
			return false;
		}
		if (isOnBlackLine())
			return rotateLeft(angleRad);
	}
	motion.stop();
	position.clearMouseBuffer();
	Serial.println("Arriving angle");
  Serial.println(position.getOrientation());
	return true;
	
}

// rotate left : (+ angleRad)
bool Robot::rotateLeft(double angleRad){
// angleRad must be between 0 and 2*PI

	Serial.println("enter rotate left");
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;
	double initialOrientation = position.getOrientation();
	
	// toAngle must be between 0 and 2*PI
	double toAngle = fmod(2*PI + position.getOrientation() + angleRad, 2*PI);
	Serial.print("initial orientation : ");
	Serial.println(initialOrientation);
	Serial.print("toAngle : ");
	Serial.println(toAngle);
	bool decelerated = false;
	int speed = ROTATIONAL_CRUISE_SPEED;
	motion.rotateLeft(ACCELERATION_SPEED);

	while (!areCloseAngles(position.getOrientation(),toAngle, TOLERANCE_ANGLE)){ 
		Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (areCloseAngles(position.getOrientation(),toAngle, 5*TOLERANCE_ANGLE)) {
			speed = map(distanceBetweenAngles(position.getOrientation(), toAngle), 0, PI, 0, ROTATIONAL_CRUISE_SPEED);
			if (! decelerated && millisFromStart > ACCELERATION_TIME) {
				motion.rotateLeft(speed);
				//Serial.println("*****************");
				decelerated = true;
			}
		}
		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateLeft(speed);
			//Serial.println("*****************");
			decelerated = true;
		}
		if ( millisFromStart > TIME_OUT ){
			motion.stop();	
			position.clearMouseBuffer();		
			enterPanicState();
			return false;
		}
		if (isOnBlackLine())
			return rotateRight(angleRad);
	}
	motion.stop();
	position.clearMouseBuffer();	
	Serial.println("Arriving angle");
  Serial.println(position.getOrientation());
	return true;
}

bool Robot::canMoveForward() {
	//return highDistanceTop.distance() > 25;
	return true;
}

bool Robot::moveBackward(double distanceToDo){
	double distanceToDo_mouse = distanceToDo * MOUSE_SCALE;
	int startX = position.getX();
	int startY = position.getY();
	double distance = 0.0;
	int speed = 20; // 20%
	motion.moveBackward(speed);
	while(distance < distanceToDo_mouse){
		position.update();
		distance = sqrt( square(position.getX()-startX) + square(position.getY()-startY));
	}
	motion.stop();
	position.update();
	return true;
	
}


// call it when angleToKeep is close enough to current orientation of robot
bool Robot::moveForwardKeepingDirection(double speed, double angleToKeep){
// angleToKeep must be between 0 and 2*PI

	double currentAngle =  position.getOrientation() ;
	
	if ( currentAngle < PI/2 && angleToKeep > 3/2*PI)
		angleToKeep -= 2*PI;
	else if ( angleToKeep < PI/2 && currentAngle > 3/2*PI)
		currentAngle -= 2*PI;
	double error = angleToKeep - currentAngle;
	// Serial.print("orientation: ");
	// Serial.println(currentAngle);
	// Serial.print("angleToKeep: ");
	// Serial.println(angleToKeep);
	// Serial.print("error: ");
	// Serial.println(error);
	
	position.update();
	motion.moveForwardWithDrift(speed, error * DRIFT_CONSTANT_MULTIPLIER);	

}


bool Robot::tryToRefindEgg(){
	Serial.println("enter in try to refind egg");
	double deltaAngleRad = 0.3;

	rotateLeft(deltaAngleRad);
	Serial.println("end rotation");
	if (proximity.distance() < 30)
		return true;
	rotateLeft(deltaAngleRad);
	Serial.println("end rotation");
	if (proximity.distance() < 30)
		return true;
	rotateRight(3 * deltaAngleRad);
	Serial.println("end rotation");
	if (proximity.distance() < 30)
		return true;
	rotateRight(deltaAngleRad);
	Serial.println("end rotation");
	if (proximity.distance() < 30)
		return true;
	return false;
	
	
}


// check if two angles in [0,2*PI] are closer than a TOLERANCE_ANGLE
bool Robot::areCloseAngles(double angle1, double angle2, double tolerance){
	if(abs(angle1 - angle2) < tolerance)
		return true;
	
	double angle1Negative = angle1 - 2*PI;
	if(abs(angle1Negative - angle2) < tolerance)
		return true;

	double angle2Negative = angle2 - 2*PI;
	if(abs(angle1 - angle2Negative) < tolerance)
		return true;
	return false;
}

double Robot::distanceBetweenAngles(double angle1, double angle2){
	double deltaAngle;
	if (angle1 > angle2)
		deltaAngle = angle1 - angle2;
	else
		deltaAngle = angle2 - angle1;
	if (deltaAngle > PI)
		return 2*PI - deltaAngle;
	else
		return deltaAngle;
}

void Robot::recalibrate() {
	position.calibrate(motion, true);
	//TODO calibrate color
}

bool Robot::isOnBlueLine() {
	return lineSensor.color() == 'b';
}

void Robot::rotateToAngle(double angle) {
	double currentOrientation = position.getOrientation();
	double angleToFollow = angle;
	double deltaRad = 0;
	if (angleToFollow > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToFollow - currentOrientation; // positive value
		rotateLeft(deltaRad);
	} else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		rotateRight(deltaRad);
	}
}

void Robot::checkSpeedChange() {
	remote.update();
	char strategy = remote.strategy();
	int step = 5;
	if (strategy == REMOTE_INCREASE_SPEED) {
		if (cruiseSpeed + 5 <= 100)
			cruiseSpeed += 5; 
	} else if (strategy == REMOTE_DECREASE_SPEED) {
		if (cruiseSpeed - 5 > 0)
			cruiseSpeed -= 5;
	}
}
