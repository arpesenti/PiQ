#include "Robot.h"

extern int state;
extern int cruiseSpeed;
extern int rotationalCruiseSpeed;

// Half timeout of the scan check moveForward
// Acceleration speed

Robot::Robot() {

}

void Robot::init() {
	Serial.println("Robot start init");
	motion.init();
	highDistanceTop.initHighDistanceTop();
	highDistanceBottom.initHighDistanceBottom();
	proximity.initProximity();
	lineSensor.init(false);
	remote.init();
	position.calibrate(motion, false);
	feet.init();
	delay(100);
	feet.open();	
	cruiseSpeed = CRUISE_SPEED;
	rotationalCruiseSpeed = ROTATIONAL_CRUISE_SPEED;
	eggOnBoard = false;
	pinMode(PANIC_LED, OUTPUT);
	digitalWrite(PANIC_LED, LOW);
	eyes.init();
	Serial.println("Robot finish init");
}

int Robot::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void Robot::start(){
	position.init();
	// ********************************* no need to check obstacles and lines when exiting from home
	int speed = cruiseSpeed/2; 

	double distance = 0; // made distance cm
	double NewDistanceLimit = 10; // cm
	double distanceToCover = CM_OUT_OF_HOME;
	
	position.update();
	double angleToFollow = position.getOrientation();
	
	unsigned long startTime = millis();
	unsigned long elapsedTime = 0;
	
	//bool decelerated = false;
	
	motion.moveForward(speed);
	
	// Serial.println("Before while");
	while (distance < distanceToCover){
		// Serial.println("Entered while");
		Serial.print("elapsedTime : ");
		Serial.println(elapsedTime);
		position.update();		
		SoftwareServo::refresh();
		position.update();
		
		// check time out
		elapsedTime = millis() - startTime;
		position.update();
		if (elapsedTime  >  TASK_TIME_OUT){
			motion.stop();
			position.update();
			Serial.println("TASK_TIME_OUT of start");
			enterPanicState(); // panic or simply stop?***************************************
			break;
		}

		position.update();

		// check angle
		 if ( distance > NewDistanceLimit){
		// Serial.println("New distance limit");
		 	motion.stop();
		 	position.update();
		 	adjustOrientation(angleToFollow); // follow initial angle
		 	position.getOrientation(); // update actual angle to compute exact coordinates
		 	NewDistanceLimit = NewDistanceLimit + DISTANCE_FOR_ADJUSTING_ANGLE;
		 	
			motion.moveForward(speed);
		 }

		position.update();
		distance = position.getY() / MOUSE_SCALE; // it moves with orientation PI/2
		// position.update();
		// //Serial.println(position.getX());
		// position.update();
  // 	Serial.println(position.getY());
  	position.update();
		
	}
	Serial.println("Complete start correctly");
	motion.stop();
	position.update();
	position.reset(); 
	// Serial.println(position.getX());
 //  Serial.println(position.getY());
 //  Serial.println(position.getOrientation());
	
}

bool Robot::adjustOrientation(double angleToFollow){
	motion.stop();
	position.update();
	delay(10);
	position.update();
	double deltaRad;
	double currentOrientation = position.getOrientation();
	Serial.println(position.getOrientation());

	if (angleToFollow > currentOrientation + TOLERANCE_ANGLE/2){
		deltaRad = angleToFollow - currentOrientation; // positive value
		if (deltaRad <= PI)
			return rotateLeft(deltaRad);
		else
			return rotateRight(2*PI - deltaRad);
	}else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		if (deltaRad <= PI)		
			return rotateRight(deltaRad);
		else
			return rotateLeft(2*PI - deltaRad);
	}else
		return true;
		
}

// scan rotating left
bool Robot::scanForEgg() {
	Serial.println("Entered in scanForEgg");

	if (proximity.distance() < 15) {
		if (highDistanceTop.distance() > 25) {
			return true;
		}
	}

	double startAngle = position.getOrientation();
	double arriveAngle = startAngle - 2 * TOLERANCE_ANGLE;
	double arriveAngleMod = fmod(2*PI + arriveAngle, 2*PI);
	Serial.println(startAngle);
	Serial.println(arriveAngleMod);
	double distanceBottom;
	double distanceProximity;
	double currentOrientation = position.getOrientation();

	unsigned long startTime = millis();
	unsigned long taskStartTime;

	fixFeet();

	motion.rotateLeft(rotationalCruiseSpeed);
	while (! areCloseAngles(currentOrientation, arriveAngleMod, TOLERANCE_ANGLE) ){
		distanceBottom = highDistanceBottom.distance();
		if (distanceBottom < 60) {
			SoftwareServo::refresh();
			distanceBottom = highDistanceBottom.distance();
		}
		Serial.print("distanceBottom : ");		
		Serial.println(distanceBottom);
		if(distanceBottom < 60){
			motion.stop();
			Serial.println("Entered in < 60");
			double distanceTop = highDistanceTop.distance();
			if(distanceTop - distanceBottom > DISTANCE_MARGIN){
				Serial.println("Entered in found");
				// Serial.print("Bottom: ");
				// Serial.print(distanceBottom);
				// Serial.print(" Top: ");
				// Serial.println(distanceTop);
				motion.stop();
				delay(100);
				distanceBottom = highDistanceBottom.distance();
				Serial.print("Bottom: ");
				Serial.println(distanceBottom);
				taskStartTime = millis();
				// while (distanceBottom > 60 && millis() - taskStartTime < TASK_TIME_OUT/2) {
				// 	//rotateRight(2*TOLERANCE_ANGLE);
				// 	motion.rotateRight(0.7*rotationalCruiseSpeed);				
				// 	//delay(10);
				// 	distanceBottom = highDistanceBottom.distance();
				// 	Serial.println("go to right to research egg");
				// }
				if (distanceBottom > 60) {
					Serial.println("enter in try to refind egg from distance");
					
					unsigned long startTimeRefound = millis();
					bool found = false;
					
					motion.rotateRight(rotationalCruiseSpeed);
					while(millis() - startTimeRefound < 4 * m100_TIME_OUT){
						if (highDistanceBottom.distance() < 60 || proximity.distance() < 15){
							motion.stop();	
							Serial.println("refound egg");
							found = true;
							break;
						}
					}
					motion.stop();

					if (!found) {
						Serial.println("egg not refound");
						motion.rotateLeft(rotationalCruiseSpeed);
						delay(100);
						continue;
					}
				}

				motion.stop();
			
				SoftwareServo::refresh();

				// exit for time out
				if(millis() - taskStartTime > TASK_TIME_OUT/2){
					motion.stop();
					Serial.println("Entered in task - timeout");
					motion.rotateLeft(rotationalCruiseSpeed);
					delay(100);
					continue;
				}

				distanceBottom = highDistanceBottom.distance();
				distanceTop = highDistanceTop.distance();
				distanceProximity = proximity.distance();
				if(distanceProximity < 10 || distanceTop < distanceBottom + DISTANCE_MARGIN){
					Serial.print("probably fake measures : proximity distance ");
					Serial.println(distanceProximity);
					Serial.print("probably fake measures : distanceTop distance ");
					Serial.println(distanceTop);
					Serial.print("probably fake measures : distanceBottom distance ");
					Serial.println(distanceBottom);
					// probably bottom measures are not correct
					motion.rotateLeft(rotationalCruiseSpeed);
					delay(300);
					
					continue;

				}
				Serial.println("Egg found");
				return true;
			
			} else {
				Serial.println("Egg covered or obstacle");
				motion.rotateLeft(rotationalCruiseSpeed);
				delay(200);
			}
		}

		if ( millis() - startTime > SHORT_TIME_OUT){
			motion.stop();
			Serial.println("Entered in timeout");
			enterPanicState();
			break;
		}

		currentOrientation = position.getOrientation();
		Serial.print("Current orientation: ");
		Serial.println(currentOrientation);
		checkSpeedChange();
	}

	Serial.println("completed rotation for scan - no eggs found");
	motion.stop();
	fixFeet();
	if (random(0, 2) >= 1) {
		motion.rotateLeft(rotationalCruiseSpeed);
	} else {
		motion.rotateRight(rotationalCruiseSpeed);
	}
	delay(random(300, 400));
	motion.stop();
	position.clearMouseBuffer();
	fixFeet();
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
	double distanceToCover = 20;

	double deltaX = 0;
	double deltaY = 0;
	
	//bool decelerated = false;

	unsigned long startTime = millis();
	unsigned long taskStartTime = millis();

	int speed = cruiseSpeed;
	
	if(!rotateToFreeDirection()) 
		return false; // not able to find a free direction - enter in panic state

	double initialOrientation = position.getOrientation();
	Serial.print("initial orientation: ");
	Serial.println(initialOrientation);

	motion.moveForward(speed);
	while (distance < distanceToCover && millis() - startTime < SHORT_TIME_OUT/2){
		// check obstacles - DO IT ONLY WHEN YOU STOP TO UPDATE ANGLE
		
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
		if ( distance > NewDistanceLimit || millis() - taskStartTime > TASK_TIME_OUT/2){
			Serial.println("check Point");
			motion.stop();
			position.update();
			delay(100);
			position.update();
			//update angle
			position.getOrientation();
			SoftwareServo::refresh();
			NewDistanceLimit = NewDistanceLimit + DISTANCE_FOR_ADJUSTING_ANGLE;
			position.clearMouseBuffer();
						
			if (checkPanicState()) {
				enterPanicState();
				return false;
			}

			//if(!decelerated && distanceToCover - distance <= DECELERATING_DISTANCE){
			//	speed = speed / 2;
			//	decelerated = true;
			//}
		
			if(!canMoveForward()){
				delay(4000); // wait for passing robot
				if(!canMoveForward()){
					moveBackward(10);
					if (canMoveForward()) {
						// probably is an egg, scan.
						motion.rotateRight(rotationalCruiseSpeed);
						delay(500);
						motion.stop();
						return true;
					}
					else if(!rotateToFreeDirection()) 
						return false; // panic state
				}
			}	
			
			SoftwareServo::refresh();

			taskStartTime = millis();
			motion.moveForward(speed);
		}
	}


	motion.stop();
	position.update();


	if ( millis() - startTime > SHORT_TIME_OUT){
		Serial.println("exit for timeout in change position");
	}
	
	Serial.println("position changed: x, y, currentangle" );
	Serial.println(position.getX());
	Serial.println(position.getY());
	Serial.println(position.getOrientation());
	SoftwareServo::refresh();
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
	

	unsigned long startTime = millis();
	unsigned long taskStartTime;
	motion.moveForward(cruiseSpeed);
	position.update();
	while (distanceProximity > 13 && millis() - startTime < SHORT_TIME_OUT/2) {
		taskStartTime = millis();		
		position.update();
		while (millis() - taskStartTime < m100_TIME_OUT){
			position.update();
			distanceProximity = proximity.distance();
			position.update();
			//Serial.print("Distance proximity");
			//Serial.println(distanceProximity);
			//position.update();
			if(distanceProximity < 13)
				break;
		}
		motion.stop();
		position.update();
		delay(20);

		if (highDistanceTop.distance() < 20){ // some obstacles
			Serial.println("some obstacles in trying to reach egg");
			delay(4000); // waiting for passing robot
			if (highDistanceTop.distance() < 20)			
				return false;
		}

		distanceProximity = proximity.distance(); // update proximity measures
		Serial.print("Distance proximity");
		Serial.println(distanceProximity);
		if(distanceProximity < 13){
			Serial.println("exit from 1st step of reach egg < 13");			
			break;
		}
		if (!tryToRefindEggFromDistance())
			return false;
		motion.moveForward(cruiseSpeed);

	}
	motion.stop();
	position.update();
	feet.open();
	
	// check if exit for time our
	if(millis() - startTime > SHORT_TIME_OUT/2){
		Serial.println("exit for time out in 1st step of try to reach egg");		
		return false;
	}

	Serial.println("Exit < 13");
	delay(1000);
	double oldProx= proximity.distance();
	double newProx = oldProx;
	
	Serial.print("prox distance: ");	
	Serial.println(newProx);
	

	
	// 2nd step : get very close to egg

	taskStartTime = millis();
	startTime = millis();
	motion.moveForward(cruiseSpeed/2);
	position.update();

	while(distanceProximity > 4 && millis() - startTime < TASK_TIME_OUT/2){
		taskStartTime = millis();
		while (millis() - taskStartTime < m100_TIME_OUT/2){
			position.update();
			distanceProximity = proximity.distance();
			position.update();
			Serial.print("Distance proximity");
			Serial.println(distanceProximity);
			position.update();
			if(distanceProximity < 4){
				motion.stop();
				position.update();
				Serial.println("egg reached - exit from 2nd step of reach egg");
				return true;
			}
		}
		motion.stop();
		position.update();
		delay(20);
		if(!tryToRefindEgg()){
			Serial.println("egg missed");
			return false;
		}
		motion.moveForward(cruiseSpeed/2);

	}

	motion.stop();
	position.update();
	
	if(distanceProximity <= 5){
		Serial.println("egg reached - exit from 2nd step of reach egg");
		return true;
	}

	if (millis() - startTime > SHORT_TIME_OUT/2){
		Serial.println("exit for time out of 2nd step of reach egg");
		return false;
	}
}


bool Robot::catchEgg() {
	// PRECONDITION robot near the egg
	
	//double eggDistance = proximity.distance();
	// if (eggDistance > 8 || highDistanceTop.distance() < 16)
	// 	return false; //egg too far
	feet.close();
	delay(1000);
	double eggDistance = proximity.distance();
	Serial.println(eggDistance);
	if (eggDistance < 7) {// to calibrate ! *************************************************************
		eggOnBoard = true;
		rotationalCruiseSpeed += SPEED_INCREMENT_ROTATION;
		cruiseSpeed += SPEED_INCREMENT;
		motion.moveForward(cruiseSpeed);
		delay(150);
		motion.stop();
		return true; //egg on board
	} else {
		feet.open(); //egg missed
		feet.close();
		eggDistance = proximity.distance();
		if (eggDistance < 6) { // to calibrate ! *************************************************************
			eggOnBoard = true;
			rotationalCruiseSpeed += SPEED_INCREMENT_ROTATION;
			cruiseSpeed += SPEED_INCREMENT;
			motion.moveForward(cruiseSpeed);
			delay(150);
			motion.stop();
			return true; //egg on board
		} else {
			feet.open();
			return false;
		}	
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
			return rotateLeft(deltaRad);
		else
			return rotateRight(2*PI - deltaRad);
	}else if (angleToFollow < currentOrientation - TOLERANCE_ANGLE/2){
		deltaRad = currentOrientation - angleToFollow; // positive value
		if (deltaRad <= PI)		
			return rotateRight(deltaRad);
		else
			return rotateLeft(2*PI - deltaRad);
	}else {
		fixFeet();
		return true;
	}
		
	
}

int Robot::tryToApproach() {
// da considerare se incontro ostacoli...
// per ora si aggiorna costantemente l'angolo...

	int radius = 5;
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
	
	unsigned long startTime = millis();
	unsigned long taskStartTime = startTime;
	motion.moveForward(speed);
	while(distanceNew > radius && millis() - startTime < TIME_OUT){
		position.update();
		if (distanceNew < 3*radius)
			speed = cruiseSpeed/2;
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
		if ( distanceNew < NewDistanceLimit || millis()-taskStartTime > TASK_TIME_OUT/2){
			motion.stop();
			delay(10);
			position.update();
			Serial.print("Distance to home: ");
			position.update();
			Serial.println(distanceNew);
			position.update();
			
			if(!canMoveForward()){
				Serial.println("Can not move forward");
				motion.stop();
				delay(4000);
				if(canMoveForward()){
					motion.moveForward(speed);
					continue;
				}
				if (distanceNew < 100) {
					fixFeet();
					return APPROACH_NOT_FOUND_LINE;
				}
				if(!changePosition())
					return APPROACH_FAILED; // panic state
				position.update();
				distanceOld = distanceNew;
				distanceNew = sqrt(square(position.getX()) + square(position.getY()));
				vectorPositionAngle = atan2(position.getY(), position.getX());
				angleToFollow = fmod(2*PI + (vectorPositionAngle - PI), 2*PI);
				if (!adjustOrientation(angleToFollow)) {
					return APPROACH_FAILED;
				}
			} else {
				if (!adjustOrientation(angleToFollow)) {
					return APPROACH_FAILED;
				}
			}

			NewDistanceLimit = distanceNew - DISTANCE_FOR_ADJUSTING_ANGLE;
			if( distanceNew <= 3 * radius )
				speed = speed / 2;
			motion.moveForward(speed);
			taskStartTime = millis();
		}
		checkSpeedChange();
		speed = cruiseSpeed;
	}
			
	motion.stop();
	position.update();
	fixFeet();
	if (millis() - startTime > TIME_OUT) {
		return APPROACH_NOT_FOUND_LINE;
	}
	eyes.commandBlink(state);
	return APPROACH_NOT_FOUND_LINE; // OK also in case the line is found


//****************************************
}


bool Robot::searchLine() {
	// PRECONDITION: robot nearby the home

	// rotate toward PI
	double currentDirection = 0;
	if (!isOnBlueLine()) {
		Serial.println("Adjusting orientation");
		if (adjustOrientation(currentDirection) == false) {
			return false;
		}
	}

	// search line first forward, then backward
	unsigned long startTime = millis();
	motion.moveForward(cruiseSpeed);
	unsigned long partialStartTime = millis();
	while (!isOnBlueLine() && millis()-startTime < TIME_OUT) {
		if (millis() - partialStartTime > 1500) {
			motion.stop();
			if (adjustOrientation(currentDirection) == false) {
				return false;
			}
			fixFeet();
			motion.moveForward(cruiseSpeed);
			partialStartTime = millis(); 
		}
		if (!canMoveForward()) {
			motion.stop();
			delay(4000); // wait for passing robots
			if(canMoveForward()){
				motion.moveForward(cruiseSpeed);
				continue;
			}
		 	currentDirection = fmod(currentDirection + PI, 2*PI);
			if (adjustOrientation(currentDirection) == false) { // dietrofront
				return false;
			}	  
			fixFeet();
			motion.moveForward(cruiseSpeed);
		}
		checkSpeedChange();
	}
	motion.stop();
	fixFeet();
	if (isOnBlueLine()) {
		Serial.println("Found orange line");
		if (position.getOrientation() > PI/2 && position.getOrientation() < 3/2*PI) {
			motion.rotateLeft(rotationalCruiseSpeed);
		} else {
			motion.rotateRight(rotationalCruiseSpeed);
		}
		delay(300);
		motion.stop();
		fixFeet();
		// orienting to the right way of the line. Hypothesis: line is not going away from the home
		motion.moveForward(cruiseSpeed);
		unsigned long taskStartTime = millis();
		while (isOnBlueLine() && millis() - taskStartTime > TASK_TIME_OUT) {
			checkSpeedChange();
		}
		delay(100);
		motion.stop();
		fixFeet();
		if (position.getOrientation() > PI/2 && position.getOrientation() < 3/2*PI) {
			motion.rotateLeft(rotationalCruiseSpeed);
		} else {
			motion.rotateRight(rotationalCruiseSpeed);
		}
		taskStartTime = millis();
		while (!isOnBlueLine()) {
			Serial.println("Rotating to orient in the right direction");
			SoftwareServo::refresh();
			checkSpeedChange();
			if (millis() - taskStartTime > SHORT_TIME_OUT) {
				motion.stop();
				Serial.println("Timeout of refine position on line");
				enterPanicState();
				return false;
			} 
		}
		motion.stop();
		fixFeet();
		return true;
	} else if (millis()-startTime >= TIME_OUT) {
		motion.stop();
		Serial.println("Timeout of search line, LAST TRY!");
		// last try: move 3/2*PI
		if (!adjustOrientation(3/2*PI)) return false;
		motion.moveForward(cruiseSpeed);
		while (canMoveForward() && millis()-startTime < LONG_TIME_OUT) {
			checkSpeedChange();
			checkPanicState();	
		}
		motion.stop();
		currentDirection = 0;
		if (!adjustOrientation(currentDirection)) return false;
		motion.moveForward(cruiseSpeed);
		startTime = millis();
		while (!isOnBlueLine() && millis()-startTime < TIME_OUT) {
			if (millis() - partialStartTime > 1500) {
				motion.stop();
				if (!adjustOrientation(currentDirection)) return false;
				fixFeet();
				motion.moveForward(cruiseSpeed);
				partialStartTime = millis(); 
			}
			if (!canMoveForward()) {
				motion.stop();
				delay(4000); // wait for passing robots
				if(canMoveForward()){
					motion.moveForward(cruiseSpeed);
					continue;
				}
			 	currentDirection = fmod(currentDirection + PI, 2*PI);
				if (!adjustOrientation(currentDirection)) return false; // dietrofront 
				fixFeet();
				motion.moveForward(cruiseSpeed);
			}
			checkSpeedChange();
		}
		motion.stop();
		fixFeet();
		if (isOnBlueLine()) {
			Serial.println("Found orange line");
			if (position.getOrientation() > PI/2 && position.getOrientation() < 3/2*PI) {
				motion.rotateLeft(rotationalCruiseSpeed);
			} else {
				motion.rotateRight(rotationalCruiseSpeed);
			}
			delay(300);
			motion.stop();
			fixFeet();
			// orienting to the right way of the line. Hypothesis: line is not going away from the home
			motion.moveForward(cruiseSpeed);
			unsigned long taskStartTime = millis();
			while (isOnBlueLine() && millis() - taskStartTime > TASK_TIME_OUT) {
				checkSpeedChange();
			}
			delay(100);
			motion.stop();
			SoftwareServo::refresh();
			if (position.getOrientation() > PI/2 && position.getOrientation() < 3/2*PI) {
				motion.rotateLeft(rotationalCruiseSpeed);
			} else {
				motion.rotateRight(rotationalCruiseSpeed);
			}
			taskStartTime = millis();
			while (!isOnBlueLine()) {
				Serial.println("Rotating to orient in the right direction");
				SoftwareServo::refresh();
				checkSpeedChange();
				if (millis() - taskStartTime > SHORT_TIME_OUT) {
					motion.stop();
					Serial.println("Timeout of refine position on line");
					enterPanicState();
					return false;
				} 
			}
			motion.stop();
			fixFeet();
			return true;
		} else {
			enterPanicState();
			return false;
		}
	} else{
		Serial.println("this should never happen, in theory!");
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

	unsigned long startTime = millis();

	// continue until time out or not able to refind the Blue line
	motion.moveForward(cruiseSpeed);
	while(millis()-startTime < LONG_TIME_OUT ){
		while (isOnBlueLine() && highDistanceTop.distance() > 16) {
			SoftwareServo::refresh();
		}
		motion.stop();
		Serial.println("Exit from the loop");
		if (isOnBlack()) {
			Serial.println("Arrived home");
			return true;
		}
		if (!isOnBlueLine()) {
			motion.stop();
			Serial.println("not on blu line - try to refind it");
			if (!refindBlueLine()) {
				if (isOnBlack()) {
					Serial.println("Arrived home");
					return true;
				}
				enterPanicState();
				return false;
			} 
			
		} 
		if (highDistanceTop.distance() < 16) {
			if (isOnBlack()) {
				return true;
			} else {
				delay(4000);
				if (highDistanceTop.distance() < 16) {
					return false;
				}
			}
		}
		motion.moveForward(cruiseSpeed);
		
	}
	motion.stop();

	Serial.println("finished for time out in follow line to home");
	enterPanicState();
	return false;
}

bool Robot::refindBlueLine() {

	unsigned long startTime = millis();

	motion.rotateLeft(rotationalCruiseSpeed);
	Serial.println("Entered in refindLine");
	while(millis() - startTime < m500_TIME_OUT/2){
		SoftwareServo::refresh();
		if (isOnBlueLine()){
			motion.stop();	
			Serial.println("refound line");
			return true;
		}
	}
	motion.stop();
	delay(50);
	startTime = millis();
	motion.rotateRight(rotationalCruiseSpeed);
	while(millis() - startTime <  m500_TIME_OUT){
		SoftwareServo::refresh();
		if (isOnBlueLine()){
			motion.stop();	
			Serial.println("refound line");
			return true;
		}
	}
	motion.stop();
	if (isOnBlack()) {
		Serial.println("Arrived home");
		return false;
	}
	delay(50);
	startTime = millis();
	motion.rotateRight(rotationalCruiseSpeed);
	while(millis() - startTime < 1.5*m500_TIME_OUT){
		SoftwareServo::refresh();
		if (isOnBlueLine()){
			motion.stop();	
			Serial.println("refound line");
			return true;
		}
	}
	motion.stop();
	delay(50);
	startTime = millis();
	motion.rotateLeft(rotationalCruiseSpeed);
	while(millis() - startTime <  m500_TIME_OUT*3){
		SoftwareServo::refresh();
		if (isOnBlueLine()){
			motion.stop();	
			Serial.println("refound line");
			return true;
		}
	}
	motion.stop();
	Serial.println("Line not refound");
	return false;
}

bool Robot::deposit() {
	// PRECONDITION: has the home in front

	feet.open();
	rotationalCruiseSpeed -= SPEED_INCREMENT_ROTATION;
	cruiseSpeed -= SPEED_INCREMENT;
	delay(400);
	int bumpDuration = 500;
	motion.moveForward(cruiseSpeed);
	delay(bumpDuration); // small bump to the just released egg
	motion.stop();	
	motion.moveBackward(cruiseSpeed);
	delay(bumpDuration);
	motion.stop();
	eggOnBoard = false;
	return true; // could it fail?
}

bool Robot::newInit() {
	// PRECONDITION: egg released, facing the home

	moveBackward(CM_OUT_OF_HOME);
	delay(500);
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
			eyes.commandBlink(state);
			feet.open();
			rotationalCruiseSpeed -= SPEED_INCREMENT_ROTATION;
			cruiseSpeed -= SPEED_INCREMENT;
			eggOnBoard = false;
			state = EXPLORE_SCAN;
		} else if (command == SEARCH_LINE_STRATEGY) {
			motion.stop();
			eyes.commandBlink(state);
			state = COMEBACK_LINESEARCHING;
		}else if (command == REMOTE_ROTATELEFT) {
			motion.stop();
			eyes.commandBlink(state);
			motion.rotateLeft(rotationalCruiseSpeed);
			delay(500);
			motion.stop();
		} else if (command == REMOTE_ROTATERIGHT) {
			motion.stop();
			eyes.commandBlink(state);
			motion.rotateRight(rotationalCruiseSpeed);
			delay(500);
			motion.stop();
		} else if (command == REMOTE_MOVEFORWARD) {
			motion.stop();
			eyes.commandBlink(state);
			unsigned long startTime = millis();
			motion.moveForward(cruiseSpeed);
			while( millis() - startTime < 500);
				//position.update();
			motion.stop();
		} else if (command == REMOTE_MOVEBACKWARD) {
			motion.stop();
			eyes.commandBlink(state);
			unsigned long startTime = millis();
			motion.moveBackward(cruiseSpeed);
			while( millis() - startTime < 500);
				//position.update();
			motion.stop();
		} else if (command == REMOTE_STOP) {
			motion.stop();
			eyes.commandBlink(state);
		} else if (command == REMOTE_INCREASE_SPEED) {
			if (cruiseSpeed + 5 <= 100)
				cruiseSpeed += 5;
	 		if (rotationalCruiseSpeed + 5 <= 100)
				rotationalCruiseSpeed += 5;
			Serial.print("New speed: ");
			Serial.println(rotationalCruiseSpeed);
			eyes.commandBlink(state);
		} else if (command == REMOTE_DECREASE_SPEED) {
			if (cruiseSpeed - 5 > 0)
				cruiseSpeed -= 5;
			if (rotationalCruiseSpeed -5 >= 0)
				rotationalCruiseSpeed -= 5;
			Serial.print("New speed: ");
			Serial.println(rotationalCruiseSpeed);
			eyes.commandBlink(state);
		} else if (command == REMOTE_OPENFEET) {
			motion.stop();
			feet.open();
			rotationalCruiseSpeed -= SPEED_INCREMENT_ROTATION;
			cruiseSpeed -= SPEED_INCREMENT;
			eggOnBoard = false;
			eyes.commandBlink(state);
		} else if (command == REMOTE_CLOSEFEET) {
			motion.stop();
			feet.close();
			rotationalCruiseSpeed += SPEED_INCREMENT_ROTATION;
			cruiseSpeed += SPEED_INCREMENT;
			eggOnBoard = true;
			eyes.commandBlink(state);
		}
	}
	motion.stop();
	digitalWrite(PANIC_LED, LOW);
	Serial.println("Exit from panic");
}

bool Robot::escapeFromWait() {
	while (state == WAIT) {
		remote.update();
		char command = remote.strategy();
		if (command == NORMAL_STRATEGY) {
			eyes.commandBlink(state);
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
	fixFeet();
	Serial.println("_________Panic____________");
	digitalWrite(PANIC_LED, HIGH);
	state = PANIC;
}

bool Robot::rotateToFreeDirection(){
	Serial.println("enter rotateToFreeDirection");

	if(canMoveForward()) 
		return true;
	double distance = 60; // distance of obstacles
	int speed = rotationalCruiseSpeed;

	// cycles eventually decreasing distance of obstacles
	while( distance >= 20){
		unsigned long startTime = millis();
		double startAngle = position.getOrientation();
		double arriveAngle = fmod(2*PI + (startAngle - 3*TOLERANCE_ANGLE), 2*PI);
		double currentOrientation = startAngle;

		bool switchRotation = false;

		int count = 1;
		int interval = 500;
		fixFeet();		
		delay(50);

		motion.rotateLeft(rotationalCruiseSpeed); // 10% speed
		delay(100);

		Serial.print("try with distance : ");
		Serial.println(distance);

		
		Serial.print("high dist. top");
		Serial.println(highDistanceTop.distance());
		Serial.println(arriveAngle);

		while ((highDistanceTop.distance() < distance || !canMoveForward()) && 
					 !areCloseAngles(startAngle, arriveAngle, 2*TOLERANCE_ANGLE)){	
			
			//Serial.println("iteration of while");
			
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

			if (millis() - startTime > count*interval) {
				SoftwareServo::refresh();
				count += 1;
			}

			if ( millis() - startTime > SHORT_TIME_OUT ){
				if(switchRotation){	
					motion.stop();	
					position.clearMouseBuffer();
					Serial.println("time out rotate to free direction");	
					enterPanicState();
					return false;
				}else{
					switchRotation = true;
					startTime = millis();
					motion.rotateRight(rotationalCruiseSpeed);
					delay(100);
				}
			}
			currentOrientation = position.getOrientation();
			checkSpeedChange();
			if (checkPanicState()) {
				enterPanicState();
				return false;
			}
		}

		delay(200); // set a delay to continue the rotation in order to ensure that the obstacles get off the trajectory of the entire body of robot
		
		motion.stop();
		position.clearMouseBuffer();

		// exit the while loop for the first condition
		if (highDistanceTop.distance() >= distance && canMoveForward()){
			Serial.println("direction found");
			fixFeet();
			return true;
		}
		
		distance -= 20; // repeat the cycle with a smaller distance of obstacles
		fixFeet();
	}

	enterPanicState();
	return false;
	
}
	
// It doesn't need anymore
bool Robot::isOnBlack(){
  // take into consideration if need to separate check of color when in rotation and in moveForward:
	// the first doesn't need update, the second one does.

	return lineSensor.color() == 'k';
}


// rotate right : (+ angleRad)
bool Robot::rotateRight(double angleRad){
// angleRad must be between 0 and 2*PI

	Serial.println("enter rotate right");
	unsigned long startTime = millis();
	unsigned long millisFromStart = 0;

	double initialOrientation = position.getOrientation();
	// toAngle must be between 0 and 2*PI
	double toAngle = fmod(2*PI + position.getOrientation() - angleRad, 2*PI);

	Serial.print("initial orientation : ");
	Serial.println(initialOrientation);
	Serial.print("toAngle : ");
	Serial.println(toAngle);

	int count = 1;
	int interval = 300;
	fixFeet();
	delay(50);
	motion.rotateRight(rotationalCruiseSpeed);
	
	while (!areCloseAngles(position.getOrientation(),toAngle, TOLERANCE_ANGLE)){ 
		// Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);
		
		if (millisFromStart > count*interval) {
			SoftwareServo::refresh();
			count += 1;
		}

		if ( millisFromStart  > SHORT_TIME_OUT ){
			motion.stop();	
			position.clearMouseBuffer();	
			enterPanicState();
			return false;
		}
		// if (!canMoveForward())
		// 	return rotateLeft(angleRad);
		if (checkSpeedChange()) 
			motion.rotateRight(rotationalCruiseSpeed); // it's possible to increase the speed while it's rotating
	}
	
	motion.stop();
	position.clearMouseBuffer();

	// ensure to have always the feet at their correct position	after a rotation 
  // (maybe some obstacles have been hit)
	fixFeet();

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


	int count = 1;
	int interval = 300;
	fixFeet();
	delay(50);
	motion.rotateLeft(rotationalCruiseSpeed);

	while (!areCloseAngles(position.getOrientation(),toAngle, TOLERANCE_ANGLE)){ 
		// Serial.println(position.getOrientation());
		millisFromStart = millis() - startTime;
		//Serial.println(millisFromStart);

		if (millisFromStart > count*interval) {
			SoftwareServo::refresh();
			count += 1;
		}
		
		if ( millisFromStart > SHORT_TIME_OUT ){
			motion.stop();	
			position.clearMouseBuffer();		
			enterPanicState();
			return false;
		}
		// if (!canMoveForward())
		// 	return rotateRight(angleRad);
		if (checkSpeedChange()) 
			motion.rotateLeft(rotationalCruiseSpeed); // it's possible to increase the speed while it's rotating
	}
	motion.stop();
	position.clearMouseBuffer();	
	
	// ensure to have always the feet at their correct position	after a rotation 
  // (maybe some obstacles have been hit)
	fixFeet();
	Serial.println("Arriving angle");
  	Serial.println(position.getOrientation());
	return true;
}

bool Robot::canMoveForward() {
	// is there an obstacle?
	// proximity is needed to avoid collateral effects of highDistanceTop when the obstacle is too much close
	if (eggOnBoard) {
		return highDistanceTop.distance() > 25;
	} else {
		return highDistanceTop.distance() > 25 && proximity.distance() > 8 && lineSensor.color() != 'k'; // check also the black color
	}
}
	

bool Robot::moveBackward(double distanceToDo){
	double distanceToDo_mouse = distanceToDo; // cm
	int startX = position.getX();
	int startY = position.getY();
	double deltaX = 0;
	double deltaY = 0;
	double distance = 0.0;
	int speed = cruiseSpeed;
	unsigned long startTime = millis();
	motion.moveBackward(speed);
	while(distance < distanceToDo_mouse && millis()-startTime<TASK_TIME_OUT){
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

	// consider time not angles when rotating

	motion.rotateLeft(rotationalCruiseSpeed);
	unsigned long startTime = millis();
	
	while(millis() - startTime < m100_TIME_OUT){
		if (proximity.distance() < 15){
			motion.stop();	
			Serial.println("refound egg");
			return true;
		}
	}
	motion.stop();
	
	startTime = millis();
	motion.rotateRight(rotationalCruiseSpeed);
	while(millis() - startTime <  2*m100_TIME_OUT){
		if ( proximity.distance() < 15){
			motion.stop();	
			Serial.println("refound egg");
			return true;
		}
	}
	motion.stop();

	Serial.println("egg not refound");	
	return false;

}
	
bool Robot::tryToRefindEggFromDistance(){
	Serial.println("enter in try to refind egg from distance");
	// consider time not angles when rotating
	
	motion.rotateLeft(rotationalCruiseSpeed);
	unsigned long startTime = millis();
	
	while(millis() - startTime < m100_TIME_OUT){
		if (highDistanceBottom.distance() < 60 || proximity.distance() < 15){
			motion.stop();	
			Serial.println("refound egg");
			return true;
		}
	}
	motion.stop();
	
	startTime = millis();
	motion.rotateRight(rotationalCruiseSpeed);
	while(millis() - startTime < 4 * m100_TIME_OUT){
		if (highDistanceBottom.distance() < 60 || proximity.distance() < 15){
			motion.stop();	
			Serial.println("refound egg");
			return true;
		}
	}
	motion.stop();

	Serial.println("egg not refound");	
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
	//Serial.println("Color: ");
	char color = lineSensor.color();
	//Serial.println(color);
	return color == 'o';
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

bool Robot::checkSpeedChange() {
	remote.update();
	char strategy = remote.strategy();
	int step = 5;
	if (strategy == REMOTE_INCREASE_SPEED) {
		eyes.commandBlink(state);
		Serial.println("increase speed");
		if (cruiseSpeed + 5 <= 100)
			cruiseSpeed += 5;
	 	if (rotationalCruiseSpeed + 5 <= 100)
			rotationalCruiseSpeed += 5;
		return true;
	} else if (strategy == REMOTE_DECREASE_SPEED) {
		eyes.commandBlink(state);
		Serial.println("decrease speed");
		if (cruiseSpeed - 5 > 0)
			cruiseSpeed -= 5;
		if (rotationalCruiseSpeed -5 <= 0)
			rotationalCruiseSpeed -= 5;
		return true;
	}
	return false;
}

bool Robot::checkPanicState() {
	remote.update();
	char strategy = remote.strategy();
	if (strategy == PANIC_STRATEGY) {
		return true;
	} else {
		return false;
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

void Robot::fixFeet() {
	for (int i = 0; i < 10; i++) {
		SoftwareServo::refresh();
	}
}

