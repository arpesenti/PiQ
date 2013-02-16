#ifndef Motion_h
#define Motion_h

class Motion {
public:
	Motion();
	void moveForward(int speed);
	void moveBackward(int speed);
	void stop();
	void rotate(double angle, int speed);
	void penguinWalking();
private:
	
};

#endif