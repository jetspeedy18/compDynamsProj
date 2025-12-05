#include<iostream>

class System {
private:

	// INPUT
	double u_;
	
	// A MATRIX
	double a11, a12;
	double a21, a22;
		
	// B MATRIX
	double b1, b2;
	
	// CURRENT X STATES
	double x1, x2;
	

public:
	
	// create a system with some blank parameters
	System() {	System(0,1,1,1,0,1,0);	}
	
	// cerate a system with specified parameters
	// assumes a simple 2-state system, can easily be expanded to include more
	System(double a11_, double a12_, double a21_, double a22_, double b1_, double b2_, double x0){
		a11 = a11_;
		a12 = a12_;
		a21 = a21_;
		a22 = a22_;
		b1 = b1_;
		b2 = b2_;
		
		x1 = x0;
		x2 = 0;
		
		u_ = 0;
	}
	
	// added the input to apply in the next timestep
	// this makes inputs function as dirac functions applied on the timesteps
	void setInput(double u) {
		u_ += u;
	}
	
	
	// change the system params on the fly
	// since we assume x1 is pos and x2 is velocity we only need to update the bottom row of the a matrix
	void edita21(double a) { a21 = a; }
	void edita22(double a) { a22 = a; }
	
	// called each timestep
	void update(double t) {
		// update system
		tickSimu(t);
		// clear the inputs
		u_ = 0;
	}
	
	// get position out
	double gety() {	return x1; }
	
	// get veclocity out
	double getyd() { return x2; }
	
	// simulates step of system
	void tickSimu(double dt) {
		
		// just use euler steps - for what im doing this should be enough
		double dx1 = a11 * x1 + a12 * x2 + b1*u_;
		double dx2 = a21 * x1 + a22 * x2 + b2*u_;

		// Forward Euler update
		x1 += dt * dx1;
		x2 += dt * dx2;

	
	}
	
};