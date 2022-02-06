/**
    AVN Model main.cpp
    6/12/2021 Fully rewritten by Kaylum McCann and Egle Rackaityte
    Code built on the work of Noah Philips and Gemma Lamb
*/

#include "simulator.h"

int main() {
    double intervals [5] = {1, 0.5, 0.333, 0.25, 999}; # Interval between simulations (999 being no stimulations)
    for (int i = 0; i<sizeof(intervals); i++) {
        stim_interval = intervals[i];
        if (stim_interval == 999){
            first_stim_time = 100;
        }
        else {
            first_stim_time = 0.1;
        }
        
        # Run through every Ach level
        for (int j = 0; j < 6; j++){
            l = j;
            all_files_suffix = "_" + std::to_string(stim_interval) + "_" + std::to_string(SKF) + "_" + std::to_string(l);
            simulator test_1;
            test_1.one_dimensional();
		std::cout << all_files_suffix << std::endl;
        }
        l = 0;
        SKF = true;
        all_files_suffix = "_" + std::to_string(stim_interval) + "_" + std::to_string(SKF) + "_" + std::to_string(l);
        simulator test_1;
        test_1.one_dimensional();
    }

}
