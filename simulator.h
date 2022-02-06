#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "configuration.h"
#include "cell_base.h"
#include "am_cell.h"
#include "av_node_2.h"
#include <fstream>
#include <sys/stat.h>
class simulator
{
private:
    int array_counter {0};
    double time_step {coarse_time_step}; // egg
    double time {0};
    std::ofstream fast_cell_data;
    std::ofstream fast_indexes;
    std::ofstream activation_times;
    std::ofstream fast_types;
    std::ofstream slow_types;
    std::ofstream slow_indexes;
    std::ofstream am_ions;
    std::ofstream slow_cell_data;
    std::ofstream fast_cell_details;
    std::ofstream slow_cell_details;
    std::ofstream test_cell_data;
    std::ofstream am_test_currents;
    std::ofstream n_test_currents;
    std::ofstream an_test_currents;
    std::ofstream nh_test_currents;
    std::ofstream clamping_data;
    std::ofstream am_test_stim_current;
    std::string dirname;
    
public:
    simulator();

    ~simulator() {};

    void delete_me() {
        delete this;
    }

    void single_cell();

    void one_dimensional();
};

#endif
