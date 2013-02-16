#ifndef Proximity_h
#define Proximity_h

class ProximitySensor {
public:
	ProximitySensor(int sensorPin);
	double distance();
private:
	int pin;
};
#endif