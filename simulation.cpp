#include <iostream>
#ifdef _WIN32 
	#include <windows.h>
#endif
#include <chrono>
#include "system.cpp"

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

#define TICK_SPEED 60 //Hz to limit loop to
#define DISTURBANCE_CHANCE 250 //what are the odds of getting a distrubance this cycle

#define NANOS_PER_LOOP 1000000000/TICK_SPEED

int main(){
	std::cout << "starting simulation" << std::endl;
	
	// load system parameters
	/// TODO
	// create system
	
	System model = System();
	
	auto past = std::chrono::high_resolution_clock::now();
	auto start = past;
	std::vector<double> t, y;
	
	bool runing = true;
	while (runing) {
		// main simultation loop
		
		
		if ((rand()%DISTURBANCE_CHANCE)==3) {
			
			// Dirac function aplplied as input
			
			std::cout << "Dirac" << std::endl;
			model.disturb();
			
		} 
				
			
		// get pid values
		// TODO// run model
		model.update();
		//save values
		double t_ = (past-start)/1000000000; //units [seconds]
		t.push_back(t_);
		y.push_back(model.gety());
		
		//lets do some graphing - example of continous matplotlibcpp located at
		//https://github.com/lava/matplotlib-cpp/blob/master/examples/animation.cpp
		plt::clf();
		plt::plot(t,y);
		// todo standardaize limits and 
		
		

		
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
		
		past = now;
		
	}
	
	
	std::cout << "finished simulation" << std::endl;
	return 0;
	
}