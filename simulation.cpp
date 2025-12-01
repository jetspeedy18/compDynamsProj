#include <iostream>
#include <fstream>
#ifdef _WIN32 
	#include <windows.h>
#endif
#include <chrono>
#include<vector>
#include "system.cpp"
#include "Controller.cpp"

//#include "matplotlibcpp.h"

//namespace plt = matplotlibcpp;

#define TICK_SPEED 60 //Hz to limit loop to
#define DISTURBANCE_CHANCE 500 //what are the odds of getting a distrubance this cycle

#define NANOS_PER_LOOP 1000000000/TICK_SPEED
#define DIRAC 10

//using namespace std;

int main(){
	std::cout << "starting simulation" << std::endl;
	
	// load system parameters
	/// TODO
	// create system
	
	System model(0,1,-2,-3,0,1,0);
	
	System model_unc(0,1,-2,-3,0,1,0);
	
	Controller cont;
	
	auto past = std::chrono::high_resolution_clock::now();
	auto start = past;
	double deltat = std::chrono::duration<double>( (past-start) ).count();
	
	std::vector<double> t, y;
	
	bool runing = true;
	
	std::ofstream out;
	out.open("plots.csv");
	
	while (runing) {
		// main simultation loop
		
		
		if ((rand()%DISTURBANCE_CHANCE)==3) {
			
			// Dirac function aplplied as input
			
			model.setInput(DIRAC);
			model_unc.setInput(DIRAC);
			
		} else if ((rand()%DISTURBANCE_CHANCE)==4) {
			
			// negative dirac funtion applied
			model.setInput(-DIRAC);
			model_unc.setInput(-DIRAC);
		}
			
		double c = cont.control(model.gety(), deltat);
		model.setInput(c);
			
			
		// get pid values
		// TODO// run model
		model.update(deltat);
		model_unc.update(deltat);
		//save values
		double t_ = std::chrono::duration<double>( (past-start) ).count(); //units [seconds]
		t.push_back(t_);
		double y_ = model.gety();
		double yd_ = model.getyd();
		y.push_back(y_);
		
		
		
		double y__ = model_unc.gety();
		double yd__ = model_unc.getyd();
		
		model.edita21(t_*0.5-9.0);
		model_unc.edita21(t_*0.5-9.0);
		model.edita22(t_*0.5-9.0);
		model_unc.edita22(t_*0.5-9.0);
		
		
		// key commands (WINDOWS IMPLEMENTED ONLY RN)
		// other OS these inputs will not work
		#ifdef _WIN32
			// hex reference for windws.h keys used below
			// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
			
			// exit loop --> exits on q press
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
		
		if (!didWait) std::cout << "WAIT UUUPPPP..." << std::endl;
		
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