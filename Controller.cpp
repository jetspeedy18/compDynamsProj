class Controller {

	private:
	
		// base parameters
		double kp;
		double ki;
		double kd;
		
		// where do we want the system to good
		// I never targeted anywhere other than zero, but it is 
		// parameterized for futere exploration
		double setPoint;
		
		// some error stats to keep around
		double err;
		double err_int;
		double err_last;
		double last_err_d;
		
		
		// use names for the fuzzy logic states
		enum FS { NB, NM, NS, Z, PS, PM, PB };
		
		// trims the input to a range
		double trimf(double x, double a, double b, double c) {
			if (x <= a || x >= c) return 0.0;
			else if (x == b) return 1.0;
			else if (x < b) return (x - a) / (b - a);
			else return (c - x) / (c - b);
		}
		
		
		// to and form the named logic states
		double membership(FS set, double x) {
			switch (set) {
				case NB: return trimf(x, -1.0, -1.0, -0.5);
				case NM: return trimf(x, -1.0, -0.5, -0.2);
				case NS: return trimf(x, -0.5, -0.2, 0.0);
				case  Z: return trimf(x, -0.2,  0.0, 0.2);
				case PS: return trimf(x,  0.0,  0.2, 0.5);
				case PM: return trimf(x,  0.2,  0.5, 1.0);
				case PB: return trimf(x,  0.5,  1.0, 1.0);
			}
			return 0.0;
		}	
		double fuzzyVal(FS set) {
			switch (set) {
				case NB: return -6.0;
				case NM: return -3.6;
				case NS: return -1.8;
				case Z:  return  0.0;
				case PS: return  1.8;
				case PM: return  3.6;
				case PB: return  6.0;
			}
			return 0;
		}
		
		const int RULE_KP[7][7] = { 
		// rule tables from "SIMULATION DESIGN OF CONTROLLER FOR NETWORKED CONTROL SYSTEM" by Jinbo Hu, Haitao Zhang, and Guidang Wu, 
		// Information Engineering College, Henan University of Science and Tehnology
		// de →   NB   NM   NS   Z    PS   PM   PB   (rows = error, columns = de)
			{ PB, PB, PM, PM, PS, Z,  Z  }, // NB
			{ PB, PB, PM, PS, PS, Z,  NS }, // NM
			{ PM, PM, PM, PS, Z,  NS, NS }, // NS
			{ PM, PM, PS,  Z, NS, NM, NM }, // Z
			{ PS, PS,  Z, NS, NS, NM, NM }, // PS
			{ PS,  Z, NS, NM, NM, NM, NB }, // PM
			{ Z,   Z, NM, NM, NM, NB, NB }  // PB
		};
		const int RULE_KI[7][7] = {
			{ NB, NB, NM, NM, NS,  Z,  Z },
			{ NB, NB, NM, NS, NS,  Z,  Z },
			{ NB, NM, NS, NS,  Z, PS, PS },
			{ NM, NM, NS,  Z, PS, PM, PM },
			{ NM, NS,  Z, PS, PS, PM, PB },
			{  Z,  Z, PS, PS, PM, PB, PB },
			{  Z,  Z, PS, PM, PM, PB, PB }
		};
		const int RULE_KD[7][7] = {
			{ PS, NS, NB, NB, NB, NM, PS },
			{ PS, NS, NB, NM, NM, NS,  Z },
			{  Z, NS, NM, NM, NS, NS,  Z },
			{  Z, NS, NS, NS, NS, NS,  Z },
			{  Z,  Z,  Z,  Z,  Z,  Z,  Z },
			{ PB, NS, PS, PS, PS, PS, PB },
			{ PB, PM, PM, PM, PS, PS, PB }
		};
		

		// loop through rule tables and apply them
		double fuzzyInf( const int rul[7][7], double err, double derr){
			double num = 0, den = 0;
			for (int i = 0; i <7; i++){
				double mu = membership((FS)i, err);
				for (int j = 0; j<7;j++) {
					double mu_d = membership((FS)j, derr);
					
					double fir = std::min(mu, mu_d);
					double out = fuzzyVal((FS)rul[i][j]);
					
					num += fir*out;
					den += fir;
				}
			}
			if (den==0) return 0;
			return num/den;
		}
				
	
	public:
		// init error and base values
		Controller() {
			kp = 1;
			ki = 0;
			kd = 0;
			
			setPoint = 0;
			
			err = 0;
			err_int = 0;
			err_last = 0;
			last_err_d = 0;
		}
		
		double control(double in_y, double deltat) {
			//update err
			err = setPoint - in_y;	
			double err_d = 0;
			
			double kp_ = kp;
			double ki_ = ki;
			double kd_ = kd;
			
			// dont fuzzy on first step (devide by zero would be a problem)
			// but otherwise use this to update PID parameters for the cycle
			if(deltat != 0) {
				err_d = (err-err_last)/deltat;
				double err_df = 0.1*err_d+0.9*last_err_d;
				
				double dkp = fuzzyInf(RULE_KP, err, err_df);
				kp_ += dkp *0.05;
				double dki = fuzzyInf(RULE_KI, err, err_df);
				ki_ += dki *0.01;
				double dkd = fuzzyInf(RULE_KD, err, err_df);
				kd_ += dkd *0.5;
			}
			
			
			//do PID
			double out = 0;
			out += kp_ * err;
			out += ki_ *	err_int;
			// restrict the amount kd can change and dont do this on the first step
			if (deltat != 0) { out += kd_ * (0.1*err_d+0.9*last_err_d); }
			
			
			//save values of error for next step
			err_last = err;
			err_int += err*deltat;
			
			// cap integration error term
			if (err_int > 10) { err_int = 10; }
			else if (err_int < -10) { err_int = -10; }
			
			return out;
		}
	
	
};