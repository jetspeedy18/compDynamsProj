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
	
	System() {	System(0,1,1,1,0,1,0);	}
	
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
	
	void setInput(double u) {
		u_ += u;
	}
	
	void edita21(double a) { a21 = a; }
	void edita22(double a) { a22 = a; }
	
//	double getStateOneDot() {return a11*x1+a12*x2+b1*u_; }
//	double getStateTwoDot() {return a21*x1+a22*x2+b2*u_; }		

	
	void update(double t) {
		// update system
		tickSimu(t);
		// clear the inputs
		u_ = 0;
	}
	
	double gety() {
		return x1;
	}
	
	double getyd() {
		return x2;
	}
	
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