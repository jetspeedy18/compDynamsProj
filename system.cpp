

class System {
private:
	double u_;
	double A;
	double B;
	
	double x;

public:
	
	System() {	System(1,1,0);	}
	
	System(double a, double b, double x0){
		A = a;
		B = b;
		x = x0;
		u_ = 0;
	}
	
	void setInput(double u) {
		u_ = u;
	}
	
	double getStates(double x) {
		return A*x*B*u_;
	}		
	
	
	// simulates step of system
	void tickSimu() {
		//x. = ax + bu_
		double xd = getStates(x);
		// solve the ode
		double xnew = 0; //solution
		
		
	}
	
};