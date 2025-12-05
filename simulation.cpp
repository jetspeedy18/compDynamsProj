#include <iostream>
#include <fstream>
#ifdef _WIN32 
	#include <windows.h>
#endif
#include <chrono>
#include<vector>
#include "system.cpp"
#include "Controller.cpp"


// lets define some compiler constants - these parameterize the compilation
#define TICK_SPEED 60 //Hz to limit loop to
#define DISTURBANCE_CHANCE 500 //what are the odds of getting a distrubance this cycle

#define NANOS_PER_LOOP 1000000000/TICK_SPEED
#define DIRAC 10

//using namespace std;

int main(){
	std::cout << "starting simulation" << std::endl;
	
	// load system parameters
	// create system
	
	// controlled system with params
	System model(0,1,-2,-3,0,1,0);
	
	// uncontrolled systemn with params for comparison
	System model_unc(0,1,-2,-3,0,1,0);
	
	// define the controlleer
	// this uses fuzzy logic autotuning within itself.
	Controller cont;
	
	// keep track of time
	auto past = std::chrono::high_resolution_clock::now();
	auto start = past;
	double deltat = std::chrono::duration<double>( (past-start) ).count();
	
	//keep track of data
	std::vector<double> t, y;
	
	
	// set up loop vars
	bool runing = true;
	std::ofstream out;
	out.open("plots.csv"); // dump output data to this file
	
	while (runing) {
		// main simultation loop
		// on windows `q` will exit the loop, otherwise you need to kill the program
		// runs the simulation in real time with timesteps taken matching its update cycle
		// this means that if we to try to have things change at a specific time, we can 
		// easily convert that to number of loops
		
		
		// disturbances are added randommly and at a random dirrection
		// do this to both models
		if ((rand()%DISTURBANCE_CHANCE)==3) {
			
			// Dirac function aplplied as input
			
			model.setInput(DIRAC);
			model_unc.setInput(DIRAC);
			
		} else if ((rand()%DISTURBANCE_CHANCE)==4) {
			
			// negative dirac funtion applied
			model.setInput(-DIRAC);
			model_unc.setInput(-DIRAC);
		}
		
		
		// for the controlled system we get the control input, and feed that into the system
		// this is only for the controlled system
		double c = cont.control(model.gety(), deltat);
		model.setInput(c);
			
			
		// update the models based on its inputs
		// do this to both models
		model.update(deltat);
		model_unc.update(deltat);
		
		//save values - keep a bunhc in memory to be used or not later
		double t_ = std::chrono::duration<double>( (past-start) ).count(); //units [seconds]
		t.push_back(t_);
		double y_ = model.gety();
		double yd_ = model.getyd();
		y.push_back(y_);
		
		// and for comparison
		double y__ = model_unc.gety();
		double yd__ = model_unc.getyd();
		
		
		// next section is for chnaging model during runtime
		// uncomment to have the model parameters be a function of runtime
		/*
		model.edita21(t_*0.5-9.0);
		model_unc.edita21(t_*0.5-9.0);
		
		
		model.edita22(t_*0.5-9.0);
		model_unc.edita22(t_*0.5-9.0);
		*/
		
		
		// key commands (WINDOWS IMPLEMENTED ONLY RN)
		// other OS these inputs will not work - will just skip this
		#ifdef _WIN32
			// hex reference for windws.h keys used below
			// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
			
			// EXIT loop --> exits on q press
			SHORT exit = GetKeyState(0x51);
			if (exit & 0x8000) {
				runing = false;
			}
		#endif
		
		
		// loop time delay ... regulate loop speed
		// for real time simulation
		// get time and wait the necessary amoutn to hit required step time
		auto now = std::chrono::high_resolution_clock::now();
		bool didWait = false;
		while (std::chrono::duration_cast<std::chrono::nanoseconds>(now-past).count()<NANOS_PER_LOOP){
			now = std::chrono::high_resolution_clock::now();
			didWait = true;
		}
		
		// lets just make sure our computer is running at real time
		// this line should NOT be printed, otherwise loop parameters are too agressive
		if (!didWait) std::cout << "WAIT UUUPPPP..." << std::endl;
		
		// save old time data
		deltat = std::chrono::duration<double>( (now-past) ).count();;
		past = now;
		
		//save the data to file
		out << t_ << "," << y_ << ","  << y__ << "\n";
	}
	
	//close file
	out.close();
	
	
	std::cout << "finished simulation" << std::endl;
	return 0;
	
}