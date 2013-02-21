#include "Robot.h"

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
}

void Robot::start(){
	position.init();
	int speed = 25; // 25%
	motion.moveForward(speed);
	unsigned long startTime = millis();
	while (abs(position.getY() / MOUSE_SCALE - 30) ){
		position.update();
		if ( millis() - startTime > TIME_OUT){
			enterPanicState();
			break;
		}

	}
	motion.stop();
	position.reset();
	Serial.println(position.getX());
  Serial.println(position.getY());
  Serial.println(position.getOrientation());
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
	int startX = position.getX();
	int startY = position.getY();
	double distance = 0.0;
	
	int speed = 25;
	
	if(!rotateToFreeDirection()) 
		return false; // not able to find a free direction - enter in panic state

	motion.moveForward(speed);
	while (distance < 50.0 * MOUSE_SCALE){
		if(isOnBlackLine()){
			motion.stop();
			position.update();
			moveBackward(10 * MOUSE_SCALE); // distance to do backward
			position.update();
			if(!rotateRight(PI/2)) 
				return false; // panic state	
			motion.moveForward(speed);	
			
		}
		if(!canMoveForward()){
			motion.stop();
			position.update();
			delay(4000); // wait for passing robot
			if(!canMoveForward()){
				if(!rotateToFreeDirection()) 
					return false; // panic state
			}
			motion.moveForward(speed);
		}
	
		position.update();
		distance = sqrt( square(position.getX()-startX) + square(position.getY()-startY));
	}
	
	motion.stop();
	position.update();

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
			moveForwardKeepingDirection(initialOrientation);
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
		moveForwardKeepingDirection(initialOrientation);
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

}


bool Robot::positioningTowardHome() {
	double vectorPositionAngle = atan2(position.getY(), position.getX());
	if (vectorPositionAngle < 0)
		vectorPositionAngle += 2*PI;
	double angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
	position.update();
	double currentOrientation = position.getOrientation();
	double deltaRad;

	// always rotate right to reach the angleToFollow
	if (angleToFollow > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToFollow - currentOrientation; // positive value
		rotateLeft(deltaRad);
		return true;
	}else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		rotateRight(deltaRad);
		return true;
	}else
		return true;
	
}

int Robot::tryToApproach() {
// da considerare se incontro ostacoli...
// per ora si aggiorna costantemente l'angolo...

	int radius = 10 * MOUSE_SCALE;
	position.update();
	double distance = sqrt(square(position.getX()) + square(position.getY()));
	
	double currentOrientation = position.getOrientation();
	double vectorPositionAngle = atan2(position.getY(), position.getX());
	double angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);

	if (angleToFollow < 0)
		angleToFollow += 2*PI;

	while(distance > radius){
		if(!canMoveForward()){
			motion.stop();
			delay(4000);
			if(canMoveForward())
				continue;
			
			if(!changePosition())
				return -1; //panic state
			position.update();
			distance = sqrt(square(position.getX()) + square(position.getY()));
			vectorPositionAngle = atan2(position.getY(), position.getX());
			angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
		}

		moveForwardKeepingDirection(angleToFollow);
		position.update();
		distance = sqrt(square(position.getX()) + square(position.getY()));
		vectorPositionAngle = atan2(position.getY(), position.getX());
		angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
		
	}
			
	motion.stop();
	position.update();
	return distance;
	
}


bool Robot::searchLine() {

}
bool Robot::followLineToHome() {

}
bool Robot::deposit() {

}
bool Robot::newInit() {

}
bool Robot::escapeFromPanic() {
	
}


void Robot::enterPanicState(){
	// TODO : does not see here the global variable state

	// turn on some led indicating it's in panic
	//state = PANIC;
}

bool Robot::rotateToFreeDirection(){
	double distance = 100; // distance of obstacles
	int speed = CRUISE_SPEED;

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
			position.update();
			
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
		position.update();

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
	position.update();
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;

	// toAngle must be between 0 and 2*PI
	double toAngle = fmod(2*PI + position.getOrientation() - angleRad, 2*PI);
	bool decelerated = false;
	motion.rotateRight(ACCELERATION_SPEED);
	
	while (!areCloseAngles(position.getOrientation(),toAngle)){ 
		position.update();
		Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateRight(CRUISE_SPEED);
			//Serial.println("*****************");
			decelerated = true;
		}
		position.update();
		if ( millisFromStart  > TIME_OUT ){
			motion.stop();		
			enterPanicState();
			return false;
		}
		if (isOnBlackLine())
			return rotateLeft(angleRad);
	}
	motion.stop();
	position.update();
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
	position.update();
	double initialOrientation = position.getOrientation();
	
	// toAngle must be between 0 and 2*PI
	double toAngle = fmod(2*PI + position.getOrientation() + angleRad, 2*PI);
	Serial.print("initial orientation : ");
	Serial.println(initialOrientation);
	Serial.print("toAngle : ");
	Serial.println(toAngle);
	bool decelerated = false;
	motion.rotateLeft(ACCELERATION_SPEED);

	while (!areCloseAngles(position.getOrientation(),toAngle)){ 
		position.update();
		Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateLeft(CRUISE_SPEED);
			//Serial.println("*****************");
			decelerated = true;
		}
		position.update();
		if ( millisFromStart > TIME_OUT ){
			motion.stop();		
			enterPanicState();
			return false;
		}
		if (isOnBlackLine())
			return rotateRight(angleRad);
	}
	motion.stop();
	position.update();
	Serial.println("Arriving angle");
  Serial.println(position.getOrientation());
	return true;
}

bool Robot::canMoveForward() {
	return highDistanceTop.distance() > 25;
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
bool Robot::moveForwardKeepingDirection(double angleToKeep){
// angleToKeep must be between 0 and 2*PI

	double currentAngle =  position.getOrientation() ;
	
	if ( currentAngle < PI/2 && angleToKeep > 3/2*PI)
		angleToKeep -= 2*PI;
	else if ( angleToKeep < PI/2 && currentAngle > 3/2*PI)
		currentAngle -= 2*PI;
	double error = angleToKeep - currentAngle;
	Serial.print("orientation: ");
	Serial.println(currentAngle);
	Serial.print("angleToKeep: ");
	Serial.println(angleToKeep);
	Serial.print("error: ");
	Serial.println(error);
	
	position.update();
	motion.moveForwardWithDrift(5, error * 50);	

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
bool Robot::areCloseAngles(double angle1, double angle2){
	if(abs(angle1 - angle2) < TOLERANCE_ANGLE)
		return true;
	
	double angle1Negative = angle1 - 2*PI;
	if(abs(angle1Negative - angle2) < TOLERANCE_ANGLE)
		return true;

	double angle2Negative = angle2 - 2*PI;
	if(abs(angle1 - angle2Negative) < TOLERANCE_ANGLE)
		return true;

}
