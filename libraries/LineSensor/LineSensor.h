#ifndef Line_h
#define Line_h

class LineSensor {
public:
	LineSensor(int leftReflectancePin, int rightReflectancePin);
	char color();
	int leftReflectance();
	int rightReflectance();
	int centerReflectance();
private:
	
};

#endif