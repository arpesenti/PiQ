#include "Robot.h"

Robot::Robot() {

}

void Robot::init() {
	motion.init();
	//highDistanceTop.initHighDistanceTop();
	//highDistanceBottom.initHighDistanceBottom();
	//proximity.initProximity();
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
	while (distance < 50.0){
		if(isOnBlackLine()){
			motion.stop();
			position.update();
			moveBackward(20);
			position.update();
			if(!rotateRight(PI/2)) {
				return false; // panic state	
				motion.moveForward(speed);	
			}
		}
		if(!canMoveForward()){
			motion.stop();
			position.update();
			if(!rotateToFreeDirection()) 
				return false; // panic state
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

}
bool Robot::catchEgg() {

}
bool Robot::positioningTowardHome() {

}
int Robot::tryToApproach() {

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

	//state = PANIC;
}

bool Robot::rotateToFreeDirection(){
	double distance = 100;
	int speed = 10;

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
		
		distance -= 20;
	}

	enterPanicState();
	return false;
	
}
	
bool Robot::isOnBlackLine(){
	return lineSensor.color() == 'k';
}

bool Robot::rotateRight(double angleRad){
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;
	double toAngle = fmod(2*PI + position.getOrientation() - angleRad, 2*PI);
	bool decelerated = false;
	motion.rotateRight(10);
	while (abs(position.getOrientation() - toAngle) > TOLERANCE_ANGLE){ 
		position.update();
		//Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateRight(-10);
			//Serial.println("*****************");
			decelerated = true;
		}
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
	//Serial.println("Arriving angle");
    //Serial.println(position.getOrientation());
	return true;
	
}

bool Robot::rotateLeft(double angleRad){
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;
	double toAngle = fmod(2*PI + position.getOrientation() + angleRad, 2*PI);
	bool decelerated = false;
	motion.rotateLeft(10);
	while (abs(position.getOrientation() - toAngle) > TOLERANCE_ANGLE){ 
		position.update();
		//Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateRight(-10);
			//Serial.println("*****************");
			decelerated = true;
		}
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
	//Serial.println("Arriving angle");
    //Serial.println(position.getOrientation());
	return true;
}

bool Robot::canMoveForward() {
	return highDistanceTop.distance() > 30;
}

bool Robot::moveBackward(double distanceToDo){
	double distanceToDo_mouse = distanceToDo * MOUSE_SCALE;
	int startX = position.getX();
	int startY = position.getY();
	double distance = 0.0;
	int speed = 20;
	motion.moveBackward(speed);
	while(distance < distanceToDo_mouse){
		position.update();
		distance = sqrt( square(position.getX()-startX) + square(position.getY()-startY));
	}
	motion.stop();
	position.update();
	return true;
	
}


