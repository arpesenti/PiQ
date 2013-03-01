#include "Robot.h"

extern int state;
extern int cruiseSpeed;
extern int rotationalCruiseSpeed;

Robot::Robot() {

}

void Robot::init() {
	Serial.println("Robot init");
	motion.init();
	highDistanceTop.initHighDistanceTop();
	highDistanceBottom.initHighDistanceBottom();
	proximity.initProximity();
	lineSensor.init(false);
	//remote.init();
	position.calibrate(motion, false);
	feet.init();	
	cruiseSpeed = CRUISE_SPEED;
	rotationalCruiseSpeed = ROTATIONAL_CRUISE_SPEED;
}

int Robot::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void Robot::start(){
	position.init();
	// ********************************* no need to check obstacles and lines when exiting from home
	int speed = CRUISE_SPEED/2; 

	double distance = 0; // made distance cm
	double NewDistanceLimit = 10; // cm
	double distanceToCover = 10;
	
	position.update();
	double angleToFollow = position.getOrientation();
	
	unsigned long startTime = millis();
	unsigned long elapsedTime = 0;
	
	bool decelerated = false;
	
	motion.moveForward(speed);
	
	// Serial.println("Before while");
	while (abs(distance) < distanceToCover){
		// Serial.println("Entered while");
		SoftwareServo::refresh();
		position.update();
		// check time out
		elapsedTime = millis() - startTime;
		position.update();
		if (elapsedTime  >  SHORT_TIME_OUT){
			enterPanicState();
			break;
		}
		position.update();
		// check angle
		// if ( distance > NewDistanceLimit){
		// 	// Serial.println("New distance limit");
		// 	motion.stop();
		// 	position.update();
		// 	adjustOrientation(angleToFollow); // follow initial angle
		// 	position.getOrientation(); // update actual angle to compute exact coordinates
		// 	//position.update();
		// 	NewDistanceLimit = NewDistanceLimit + DISTANCE_FOR_ADJUSTING_ANGLE;
		// 	//position.update();
		// 	if(!decelerated && distanceToCover - distance <= DECELERATING_DISTANCE){
		// 		speed = speed / 2;
		// 		decelerated = true;
		// 	}
		// 	//position.update();
		// 	motion.moveForward(speed);
		// }

		position.update();
		distance = position.getY() / MOUSE_SCALE; // it moves with orientation PI/2
		position.update();
		Serial.println(position.getX());
		position.update();
  	Serial.println(position.getY());
  	position.update();
		
	}
	Serial.println("Exit while");
	motion.stop();
	position.update();
	position.reset(); // da rimettere!!!!
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
	double arriveAngle = startAngle - 2 * TOLERANCE_ANGLE;
	double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);
	double distanceBottom;
	double currentOrientation = position.getOrientation();

	unsigned long startTime = millis();
	motion.rotateLeft(rotationalCruiseSpeed);
	while (! areCloseAngles(currentOrientation, arriveAngle, TOLERANCE_ANGLE) ){
		distanceBottom = highDistanceBottom.distance();
		Serial.println(distanceBottom);
		if(distanceBottom < 60){
			motion.stop();
			Serial.println("Entered in <80");
			double distanceTop = highDistanceTop.distance();
			if(distanceTop - distanceBottom > DISTANCE_MARGIN){
				// Serial.println("Entered in found");
				// Serial.print("Bottom: ");
				// Serial.print(distanceBottom);
				// Serial.print(" Top: ");
				// Serial.println(distanceTop);
				motion.stop();
				delay(100);
				Serial.print("Bottom: ");
				Serial.println(highDistanceBottom.distance());
				distanceBottom = highDistanceBottom.distance();
				while (distanceBottom > 60 && millis() - startTime < TIME_OUT) {
					rotateRight(TOLERANCE_ANGLE+0.01);
					delay(100);
					distanceBottom = highDistanceBottom.distance();
				}
				if(millis() - startTime < TIME_OUT){
					motion.stop();
					Serial.println("Entered in timeout");
					enterPanicState();
					return false;
				}
				return true;
			} else {
				motion.rotateLeft(rotationalCruiseSpeed);
				delay(10);
			}
		}

		if ( millis() - startTime > TIME_OUT){
			motion.stop();
			Serial.println("Entered in timeout");
			enterPanicState();
			break;
		}

		//if( isOnBlackLine()){
		//	motion.stop();
		//	rotateRight(PI/2);
		//	return false;
		//}
		currentOrientation = position.getOrientation();
		checkSpeedChange();
	}
	motion.stop();
	return false;
	
} 

// change position of a fixed distance
// return false if not able to find a free direction to move - panic state
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

	int speed = cruiseSpeed;
	
	if(!rotateToFreeDirection()) 
		return false; // not able to find a free direction - enter in panic state

	double initialOrientation = position.getOrientation();
	Serial.print("initial orientation: ");
	Serial.println(initialOrientation);

	motion.moveForward(speed);
	while (distance < distanceToCover){
		// check black line - no more needed *** FIELD IS DELIMITED WITH WALLS
		//if(isOnBlackLine()){
		//	motion.stop();
		//	position.update();
		//	moveBackward(10 * MOUSE_SCALE); // distance to do backward
		//	position.update();
		//	if(!rotateRight(PI/2)) 
		//		return false; // panic state	
		//	motion.moveForward(speed);	
			
		//}
		//position.update();

		// check obstacles - DO IT ONLY WHEN YOU STOP TO UPDATE ANGLE
		

		//if(!canMoveForward()){
		//	motion.stop();
		//	position.update();
		//	delay(4000); // wait for passing robot
		//	if(!canMoveForward()){
		//		if(!rotateToFreeDirection()) 
		//			return false; // panic state
		//	}
		//	position.update();
		//	motion.moveForward(speed);
		//}
		
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
		
			if(!canMoveForward()){
				delay(4000); // wait for passing robot
				if(!canMoveForward()){
					if(!rotateToFreeDirection()) 
						return false; // panic state
				}
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

// if it fails, you have to redo a scan of eggs
bool Robot::reachEgg() {
	double oldDistanceBottom = highDistanceBottom.distance();
	double newDistanceBottom = oldDistanceBottom;
	double distanceProximity = proximity.distance();
	bool waitForPassingRobot = false;
	double initialOrientation = position.getOrientation();
	int speed = cruiseSpeed;
	
	// 1st step : move closer such that proximity senses it
	Serial.print("Distance bottom: ");
	Serial.println(newDistanceBottom);
	Serial.print("Proximity: ");
	Serial.println(distanceProximity);
	// if (newDistanceBottom > 15 && distanceProximity > 15){
	// 	motion.moveForward(cruiseSpeed);		
	// 	while(newDistanceBottom > 15){
	// 		position.update();
	// 		if(newDistanceBottom - oldDistanceBottom > 20){ // give a bit of tolerance on measurement errors
	// 			motion.stop();
	// 			position.update();
	// 			return false;
	// 		}
	// 		oldDistanceBottom = newDistanceBottom;
	// 		position.update();
	// 		newDistanceBottom = highDistanceBottom.distance();
	// 		position.update();

	// 		// check for robot and obstacles
	// 		double distanceTop = highDistanceTop.distance();
	// 		position.update();

	// 		if(distanceTop - newDistanceBottom < DISTANCE_MARGIN){
	// 			motion.stop();
	// 			position.update();
	// 			if(!waitForPassingRobot){
	// 				waitForPassingRobot = true;
	// 				delay(3000);
	// 				continue;
	// 			}else
	// 				return false;
	// 		}
	// 	}
	// }
	// position.update();
	// motion.stop();
	// position.update();

	unsigned long startTime = millis();
	motion.moveForward(cruiseSpeed);
	position.update();
	while (distanceProximity > 13) {
		startTime = millis();
		position.update();
		while (millis() - startTime < 100)
			position.update();
		motion.stop();
		delay(20);

		if (highDistanceTop.distance() < 20) // some obstacles
			return false;

		distanceProximity = proximity.distance(); // update proximity measures
		if(distanceProximity < 13) 
			break;

		newDistanceBottom = highDistanceBottom.distance();
		if (newDistanceBottom > 60) {
			bool result = tryToRefindEggFromDistance();
			if (result == true) {
				position.getOrientation();
				position.clearMouseBuffer();
				motion.moveForward(cruiseSpeed);
				Serial.println("Refound");
				continue;
			} else {
				Serial.println("Not Refound");
				return false;
			}
		}

	}


	double oldProx= proximity.distance();
	double newProx = oldProx;
	
	Serial.print("prox distance: ");	
	Serial.println(newProx);
	

	motion.moveForward(cruiseSpeed/2);
	// 2nd step : get very close to egg
	while(newProx > 4){
		position.update();

		if (newProx >= 15 && oldProx >= 15){
			motion.stop();
			position.update();
			if(tryToRefindEgg() == false)
				return false;			
			else
				motion.moveForward(cruiseSpeed/2);
		}

		position.update();

		if(newProx > oldProx + 7){ // give a bit of tolerance on measurement errors
			motion.stop();
			position.update();
			if(tryToRefindEgg() == false)
				return false;
			else
				motion.moveForward(cruiseSpeed/2);
						
		}

		position.update();
		oldProx = newProx;
		position.update();
		newProx = proximity.distance();		
		//Serial.print("prox distance: ");	
		//Serial.println(newProx);

	}
	
	motion.stop();
	position.update();
	return true;
	
}
bool Robot::catchEgg() {
	// PRECONDITION robot near the egg
	
	// double eggDistance = proximity.distance();
	// if (eggDistance > 8)
	// 	return false; //egg too far
	// feet.close();
	// eggDistance = proximity.distance();
	// if (eggDistance > 4) 
	// 	return true; //egg on board
	// else {
	// 	feet.open(); //egg missed
	// 	return false;
	// }
	feet.open();
	// Serial.println("Open feet");
	// SoftwareServo::refresh();
	// delay(1000);
	// feet.close();
	// Serial.println("Close feet");
	SoftwareServo::refresh();
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
	int speed = cruiseSpeed;
	
	if (angleToFollow < 0)
		angleToFollow += 2*PI;
	
	//************************************** 
	double NewDistanceLimit = distanceNew - DISTANCE_FOR_ADJUSTING_ANGLE;
	
	motion.moveForward(speed);
	while(distanceNew > radius){
		position.update();
		if (distanceNew < 3*radius)
			speed = map(distanceNew, 0, 3 * radius, 0, cruiseSpeed);
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
			
			if(!canMoveForward()){
				Serial.println("Can not move forward");
				motion.stop();
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

			NewDistanceLimit = NewDistanceLimit - DISTANCE_FOR_ADJUSTING_ANGLE;
			if( distanceNew <= 3 * radius )
				speed = speed / 2;
			motion.moveForward(speed);
		}
		checkSpeedChange();
		speed = cruiseSpeed;
	}
			
	motion.stop();
	position.update();
	return APPROACH_FOUND_LINE;


//****************************************
}


bool Robot::searchLine() {
	// PRECONDITION: robot nearby the home

	// rotate toward PI
	double currentDirection = PI;
	adjustOrientation(currentDirection);

	// search line first forward, then backward
	unsigned long startTime = millis();
	motion.moveForward(cruiseSpeed);
	while (!isOnBlueLine() && millis()-startTime < TIME_OUT) {
		position.update();
		if (isOnBlackLine()) {
			motion.stop();
		 	position.update();
			adjustOrientation(fmod(currentDirection + PI, 2*PI)); // dietrofront 
		}
		checkSpeedChange();
	}
	if (isOnBlueLine()) {
		motion.stop();
		position.update();

		// orienting to the right way of the line. Hypothesis: line is not going away from the home
		int threshold = 200;
		int rightReflectance = lineSensor.rightReflectance();
		int leftReflectance = lineSensor.leftReflectance(); 
		while (leftReflectance > threshold || rightReflectance > threshold) {
			if (leftReflectance > rightReflectance) {
				motion.rotateLeft(rotationalCruiseSpeed);
			} else {
				motion.rotateRight(rotationalCruiseSpeed);
			}
			rightReflectance = lineSensor.rightReflectance();
			leftReflectance = lineSensor.leftReflectance(); 
		} 
		return true;
	} else if (millis()-startTime > TIME_OUT) {
		motion.stop();
		position.update();
		return false;
	}
}

bool Robot::followLineToHome() {
	// PRECONDITION: is on blue line, and there is no obstacle in front other then the home...

	// if (!isOnBlueLine()) {
	// 	if (!refindBlueLine()) {
	// 		enterPanicState();
	// 		return false;
	// 	}
	// }

	//*********************for test
	while(!isOnBlueLine()){ };
	int drift = 0;
	unsigned long startTime = millis();
	bool tryToFollowLine = true;
	
	// continue until time out or not able to refind the Blue line
	while(tryToFollowLine){
		while (isOnBlueLine() && /*highDistanceTop.distance() > 30 &&*/ millis()-startTime < TIME_OUT ) {
			drift = lineSensor.rightReflectance() - lineSensor.leftReflectance();
			Serial.print("Drift: ");
			Serial.println(drift);
			motion.moveForwardWithDrift(3, drift);
		}

		Serial.println("Exit from the loop");
		if (millis()-startTime > TIME_OUT) {
			Serial.println("finished for time out in follow line to home");
			motion.stop();
			enterPanicState();
			return false;
		} else if (!isOnBlueLine()) {
			motion.stop();
			Serial.println("not on blu line - try to refind it");
			if (!refindBlueLine()) {
				enterPanicState();
				return false;
			} 
			
		}
		
	}
	motion.stop();
	return true;
}

bool Robot::refindBlueLine() {
	unsigned long startTime = millis();
	while (!isOnBlueLine() && millis()-startTime < TIME_OUT ) {
		if (lineSensor.leftReflectance() - lineSensor.rightReflectance() > 80) {
			Serial.println("refind blue line - rotate left");
			rotateLeft(2 * TOLERANCE_ANGLE);
		} else if (lineSensor.rightReflectance() - lineSensor.leftReflectance() > 80) {
			Serial.println("refind blue line - rotate right");
			rotateRight(2 * TOLERANCE_ANGLE);
		} else {

			// make a complete rotation
			Serial.println("make complete rotation");
			double startAngle = position.getOrientation();
			double arriveAngle = startAngle - 2 * TOLERANCE_ANGLE;
			double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);
			double distanceBottom;
			double currentOrientation = position.getOrientation();
		
			int speed = rotationalCruiseSpeed;	
			unsigned long startTime = millis();
			
			while (arriveAngle >= 0 ? 
				(currentOrientation < arriveAngleMod || currentOrientation > startAngle):
				(currentOrientation < arriveAngleMod && currentOrientation > startAngle)){
				
				rotateLeft(2 * TOLERANCE_ANGLE);
				
				if (isOnBlueLine()){
					motion.stop();
					delay(20);
					if(isOnBlueLine()) // recheck to be sure with stop motors
						return true;
					else
						continue;	
				}
				
				if( millis() - startTime > TIME_OUT){
					motion.stop();
					return false;
				}
			}
		}
	}
	if (isOnBlueLine())
		return true;
	else 
		return false;
}

bool Robot::deposit() {
	// PRECONDITION: has the home in front

	feet.open();
	delay(400);
	int bumpDuration = 300;
	motion.moveForward(cruiseSpeed);
	delay(bumpDuration); // small bump to the just released egg
	motion.stop();	
	motion.moveBackward(cruiseSpeed);
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
		} else if (command == SEARCH_LINE_STRATEGY) {
			motion.stop();
			state = COMEBACK_LINESEARCHING;
		}else if (command == REMOTE_ROTATELEFT) {
			motion.stop();
			rotateLeft(2*TOLERANCE_ANGLE);
			position.getOrientation();
		} else if (command == REMOTE_ROTATERIGHT) {
			motion.stop();
			rotateRight(2*TOLERANCE_ANGLE);
			position.getOrientation();
		} else if (command == REMOTE_MOVEFORWARD) {
			motion.stop();
			unsigned long startTime = millis();
			motion.moveForward(cruiseSpeed);
			while( millis() - startTime < 100)
				position.update();
			motion.stop();
		} else if (command == REMOTE_MOVEBACKWARD) {
			motion.stop();
			unsigned long startTime = millis();
			motion.moveBackward(cruiseSpeed);
			while( millis() - startTime < 100)
				position.update();
			motion.stop();
		} else if (command == REMOTE_STOP) {
			motion.stop();
		} else if (command == REMOTE_INCREASE_SPEED) {
			if (cruiseSpeed + 5 <= 100)
				cruiseSpeed += 5;
	 		if (rotationalCruiseSpeed + 5 <= 100)
				rotationalCruiseSpeed += 5;
		} else if (command == REMOTE_DECREASE_SPEED) {
			if (cruiseSpeed - 5 > 0)
				cruiseSpeed -= 5;
			if (rotationalCruiseSpeed -5 <= 0)
				rotationalCruiseSpeed -= 5;
		}
	}
}

bool Robot::escapeFromWait() {
	while (state == WAIT) {
		remote.update();
		char command = remote.strategy();
		if (command == NORMAL_STRATEGY) {
			start();
			state = EXPLORE_SCAN;
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
	Serial.println("_________Panic____________");
	state = PANIC;
}

bool Robot::rotateToFreeDirection(){
	if(canMoveForward()) 
		return true;
	double distance = 80; // distance of obstacles
	int speed = rotationalCruiseSpeed;

	// cycles eventually decreasing distance of obstacles
	while( distance > 0){
		unsigned long startTime = millis();
		double startAngle = position.getOrientation();
		double arriveAngle = startAngle - TOLERANCE_ANGLE;
		double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);
		double currentOrientation = startAngle;

		bool switchRotation = false;
		int randNumber = random(2); // random number between 0 and 1;
		motion.rotateLeft(rotationalCruiseSpeed); // 10% speed
		while (highDistanceTop.distance() < distance && 
					(arriveAngle >= 0 ? 
						(currentOrientation < arriveAngleMod || currentOrientation > startAngle):
						(currentOrientation < arriveAngleMod && currentOrientation > startAngle))){	
			
			
			//if (isOnBlackLine()){
			//	motion.stop();
			//	position.clearMouseBuffer();
			//	if(!switchRotation){
			//		switchRotation = true;
			//		startTime = millis();
			//		motion.rotateRight(rotationalCruiseSpeed);
			//	}else{
			//		break;
			//	}
			//}
			
			if ( millis() - startTime > TIME_OUT ){
				if(switchRotation){	
					motion.stop();	
					position.clearMouseBuffer();	
					enterPanicState();
					return false;
				}else{
					switchRotation = true;
					startTime = millis();
					motion.rotateRight(rotationalCruiseSpeed);
				}
			}
			currentOrientation = position.getOrientation();
			checkSpeedChange();
		}
		
		motion.stop();
		position.clearMouseBuffer();

		// exit the while loop for the first condition
		if (highDistanceTop.distance() >= distance){
			return true;
		}
		
		distance -= 20; // repeat the cycle with a smaller distance of obstacles
	}

	enterPanicState();
	return false;
	
}
	
// It doesn't need anymore
bool Robot::isOnBlackLine(){
	//test
	return false;
  // take into consideration if need to separate check of color when in rotation and in moveForward:
	// the first doesn't need update, the second one does.

	//return lineSensor.color() == 'k';
}


// rotate right : (+ angleRad)
bool Robot::rotateRight(double angleRad){
// angleRad must be between 0 and 2*PI

	Serial.println("enter rotate right");
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;

	double speed = rotationalCruiseSpeed;
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
			speed = map(distanceBetweenAngles(position.getOrientation(), toAngle), 0, PI, 0, rotationalCruiseSpeed);
			if (! decelerated && millisFromStart > ACCELERATION_TIME) {
				motion.rotateRight(speed);
				decelerated = true;
			}
		}

		if (! decelerated && millisFromStart > ACCELERATION_TIME) {
			motion.rotateRight(rotationalCruiseSpeed);
			decelerated = true;
		}
		
		if ( millisFromStart  > TIME_OUT ){
			motion.stop();	
			position.clearMouseBuffer();	
			enterPanicState();
			return false;
		}
		//if (isOnBlackLine())
		//	return rotateLeft(angleRad);
		checkSpeedChange();
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
	int speed = rotationalCruiseSpeed;
	motion.rotateLeft(ACCELERATION_SPEED);

	while (!areCloseAngles(position.getOrientation(),toAngle, TOLERANCE_ANGLE)){ 
		Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		if (areCloseAngles(position.getOrientation(),toAngle, 5*TOLERANCE_ANGLE)) {
			speed = map(distanceBetweenAngles(position.getOrientation(), toAngle), 0, PI, 0, rotationalCruiseSpeed);
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
		//if (isOnBlackLine())
		//	return rotateRight(angleRad);
		checkSpeedChange();
	}
	motion.stop();
	position.clearMouseBuffer();	
	Serial.println("Arriving angle");
  	Serial.println(position.getOrientation());
	return true;
}

bool Robot::canMoveForward() {
	return highDistanceTop.distance() > 25;
}

bool Robot::moveBackward(double distanceToDo){
	double distanceToDo_mouse = distanceToDo; // cm
	int startX = position.getX();
	int startY = position.getY();
	double deltaX = 0;
	double deltaY = 0;
	double distance = 0.0;
	int speed = cruiseSpeed;
	motion.moveBackward(speed);
	while(distance < distanceToDo_mouse){
		position.update();
		deltaX = square(position.getX()-startX);
		position.update();
		deltaY = square(position.getY()-startY);
		position.update();
		distance = sqrt(deltaX + deltaY) / MOUSE_SCALE;
		checkSpeedChange();
	}
	motion.stop();
	position.update();
	return true;
	
}


//********************************* I think it can be cut *******************
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

//********************************* end of cut *****************************

bool Robot::tryToRefindEgg(){
	Serial.println("enter in try to refind egg");
	double deltaAngleRad = 0.3;

	rotateLeft(deltaAngleRad);
	//Serial.println("end rotation");
	if (proximity.distance() < 15)
		return true;
	rotateLeft(deltaAngleRad);
	//Serial.println("end rotation");
	if (proximity.distance() < 15)
		return true;
	rotateRight(3 * deltaAngleRad);
	//Serial.println("end rotation");
	if (proximity.distance() < 15)
		return true;
	rotateRight(deltaAngleRad);
	//Serial.println("end rotation");
	if (proximity.distance() < 15)
		return true;
	return false;
}

bool Robot::tryToRefindEggFromDistance(){
	Serial.println("enter in try to refind egg from distance");
	double deltaAngleRad = 2*TOLERANCE_ANGLE;

	rotateLeft(deltaAngleRad);
	//Serial.println("end rotation");
	if (highDistanceBottom.distance() < 60)
		return true;
	rotateLeft(deltaAngleRad);
	//Serial.println("end rotation");
	if (highDistanceBottom.distance() < 60)
		return true;
	rotateRight(3 * deltaAngleRad);
	//Serial.println("end rotation");
	if (highDistanceBottom.distance() < 60)
		return true;
	rotateRight(deltaAngleRad);
	//Serial.println("end rotation");
	if (highDistanceBottom.distance() < 60)
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
	//return lineSensor.color() == 'b';

	// for test
	Serial.println("Color: ");
	char color = lineSensor.color();
	Serial.println(color);
	return color == 'k';
}

void Robot::rotateToAngle(double angle) {
	double currentOrientation = position.getOrientation();
	double angleToReach = angle;
	double deltaRad = 0;
	if (angleToReach > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToReach - currentOrientation; // positive value
		if (deltaRad <= PI)
			rotateLeft(deltaRad);
		else
			rotateRight(2*PI - deltaRad);
		return;
	}else if (angleToReach < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToReach; // positive value
		if (deltaRad <= PI)		
			rotateRight(deltaRad);
		else
			rotateLeft(2*PI - deltaRad);
		return;
	}else
		return;


}

void Robot::checkSpeedChange() {
	remote.update();
	char strategy = remote.strategy();
	int step = 5;
	if (strategy == REMOTE_INCREASE_SPEED) {
		if (cruiseSpeed + 5 <= 100)
			cruiseSpeed += 5;
	 	if (rotationalCruiseSpeed + 5 <= 100)
			rotationalCruiseSpeed += 5;
	} else if (strategy == REMOTE_DECREASE_SPEED) {
		if (cruiseSpeed - 5 > 0)
			cruiseSpeed -= 5;
		if (rotationalCruiseSpeed -5 <= 0)
			rotationalCruiseSpeed -= 5;
	}
}



// method that doesn't use mouse

bool Robot::comeBackHomeSimple(){
	double currentOrientation = position.getOrientation();
	rotateToAngle(3/2*PI);
	motion.moveForward(CRUISE_SPEED);
	
	while(!isOnBlueLine()){
		if( highDistanceTop.distance() < 20){
			motion.stop();
			bool result = changePositionSimple();
			if ( result == true){
				currentOrientation = position.getOrientation();
				if( currentOrientation > 0 && currentOrientation < 3/4*PI){
					rotateToAngle(PI);
					if(changePositionSimple() == false)
						return false;
				}
				rotateToAngle(3/2*PI);
				motion.moveForward(CRUISE_SPEED);
				continue;
			}else	
				return false; // panic state
		}
	}
}

bool Robot::changePositionSimple(){
	Serial.println("enter change position simple");
	if(!rotateToFreeDirection()){
		enterPanicState();
		return false;
	}
	motion.moveForward(CRUISE_SPEED);
	delay(1000);
	motion.stop();
	return true;	
}

bool Robot::searchEggSimple(){
	double proximityDistance = proximity.distance();
	while(proximityDistance < 10){
		motion.moveForward(CRUISE_SPEED);
		if(highDistanceTop.distance() < 20){
			if(changePositionSimple() == false)
				return false; //panic state
		}

		proximityDistance = proximity.distance();

	}
}

