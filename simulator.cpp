#include "simulator.h"

simulator::simulator()
{
    array_counter = 0;
    time_step = coarse_time_step;
    time = 0;
    dirname = "Output/" + all_files_suffix;
    const char * c { dirname.c_str()};
    mkdir(c,0777);
}

void simulator::single_cell()
{
    std::ofstream test_cell_data {dirname + "/" + "test_cells" + all_files_suffix + ".txt"};
	std::ofstream am_test_currents {dirname + "/" + "am_test_currents" + all_files_suffix + ".txt"};
	std::ofstream n_test_currents {dirname + "/" + "n_test_currents" + all_files_suffix + ".txt"};
	std::ofstream an_test_currents {dirname + "/" + "an_test_currents" + all_files_suffix + ".txt"};
	std::ofstream nh_test_currents {dirname + "/" + "nh_test_currents" + all_files_suffix + ".txt"};
    std::ofstream clamping_data {dirname + "/" + "clamping_data" + all_files_suffix + ".txt"};
	std::ofstream am_test_stim_current {dirname + "/" +"am_test_stim_current" + all_files_suffix + ".txt"};
    
    cell_base** test_cells = new cell_base * [4];
    bool test_cells_allowed[4];

    //Now generate cells just to test them; am, n, an, nh
    am_cell* test_am_cell = new am_cell();
    test_cells[0] = test_am_cell;
    test_cells_allowed[0] = allow_am;

    av_node_2* test_n_cell = new av_node_2(1);
    test_cells[1] = test_n_cell;
    test_cells_allowed[1] = allow_n;

    av_node_2* test_an_cell = new av_node_2(16);
    test_cells[2] = test_an_cell;
    test_cells_allowed[2] = allow_an;

    av_node_2* test_nh_cell = new av_node_2(19);
    test_cells[3] = test_nh_cell;
    test_cells_allowed[3] = allow_nh;

    //Set i stim for test cells to be 0 when starting
    for (int i{}; i < 4; i++) {
        (*test_cells[i]).set_i_stim(0);
    }

    int test_cell_stim_counter[4];
    test_cell_stim_counter[0] = 0;
    test_cell_stim_counter[1] = 0;
    test_cell_stim_counter[2] = 0;
    test_cell_stim_counter[3] = 0;

    double clamp_voltage_current{ first_clamp_voltage };

    //Need doubles to hold the min/max for a current
    double Na_min{ 999 };
    double CaL_min{ 999 };
    double k1_min{ 999 };
    double to_max{ -999 };
    double kr_max{ -999 };
    double naca_min{ 999 };

    while (time <= sim_time) {
        if (allow_test_cells == true && clamp_multiple_voltages == false) {
                for (int i{}; i < 4; i++) {
                    if (test_cells_allowed[i] == true) {
                        if (stimulate_test_cells == false && clamp_test_cells == false) {
                            (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                            (*test_cells[i]).calc_vm(time_step, solve_method, false);
                        }
                        else if (clamp_test_cells == true) {
                            if (time > clamp_duration + clamp_peak_start_time) {
                                (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                                (*test_cells[i]).calc_vm(time_step, solve_method, false);
                            }
                            else if (time > clamp_peak_start_time) {
                                (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                                (*test_cells[i]).set_vm(clamp_peak_voltage);
                            }
                            else if (time > clamp_hold_start_time) {
                                (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                                (*test_cells[i]).set_vm(clamp_holding_voltage);
                            }
                            else {
                                (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                                (*test_cells[i]).calc_vm(time_step, solve_method, false);
                            }
                        }
                        else {
                            if (time >= first_stim_time + test_cell_stim_counter[i] * test_cell_stim_interval) {
                                (*test_cells[i]).set_i_stim(stim_current);
                                if (i == 0) {
                                    (*test_cells[i]).set_i_stim(stim_current * am_cell_unit_multiplier * am_cell_stim_multiplier);
                                }
                                if (time >= first_stim_time + test_stim_duration + (test_cell_stim_counter[i] * test_cell_stim_interval)) {
                                    (*test_cells[i]).set_i_stim(0);
                                    (test_cell_stim_counter[i]) += 1;
                                }
                            }
                            (*test_cells[i]).calc_i_all(time_step, solve_method, l);
                            (*test_cells[i]).calc_vm(time_step, solve_method, false);
                        }
                    }
                }
                if (array_counter % print_adjuster == 0) {
                    //cout << "array counter is " << array_counter << endl;
                    test_cell_data << time << "\t";
                    if (analyse_indivdial_cell == false) {
                        for (int i = 0; i < 4; i++) {
                            test_cell_data << (*test_cells[i]).get_vm() << "\t";
                        }
                    }
                    else {
                        double dvdt = -((*test_cells[individual_cell_index]).get_total_ion(false) + (*test_cells[individual_cell_index]).get_i_stim()) / (*test_cells[individual_cell_index]).get_cm();
                        test_cell_data << (*test_cells[individual_cell_index]).get_vm() << "\t" << dvdt;
                    }
                    (*test_cells[0]).print_currents(am_test_currents, time, 2);
                    (*test_cells[1]).print_currents(n_test_currents, time, 1);
                    (*test_cells[2]).print_currents(an_test_currents, time, 16);
                    (*test_cells[3]).print_currents(nh_test_currents, time, 19);
                    am_test_stim_current << time << "\t" << (*test_cells[0]).get_i_stim() << std::endl;
                    test_cell_data << std::endl;
                }
            }
        else if (allow_test_cells == true) {//Do multiple voltage clamping
                if (multiple_clamp_type == 0) {
                    if (time > clamp_duration + clamp_peak_start_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, 0);
                        (*test_cells[individual_cell_index]).calc_vm(time_step, solve_method, false);
                    }
                    else if (time > clamp_peak_start_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, 0);
                        (*test_cells[individual_cell_index]).set_vm(clamp_voltage_current);
                    }
                    else if (time > clamp_hold_start_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, 0);
                        (*test_cells[individual_cell_index]).set_vm(clamp_holding_voltage);
                    }
                    else {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, 0);
                        (*test_cells[individual_cell_index]).calc_vm(time_step, solve_method, false);
                    }
                    //Now check for currents being mins
                    if ((*test_cells[individual_cell_index]).get_i_na() < Na_min && time >= clamp_hold_start_time) {
                        Na_min = (*test_cells[individual_cell_index]).get_i_na();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_na() < CaL_min && time >= clamp_hold_start_time) {
                        CaL_min = (*test_cells[individual_cell_index]).get_i_cal();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_k1() < k1_min && time >= clamp_hold_start_time) {
                        k1_min = (*test_cells[individual_cell_index]).get_i_k1();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_to() > to_max && time >= clamp_hold_start_time) {
                        to_max = (*test_cells[individual_cell_index]).get_i_to();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_kr() > kr_max && time >= clamp_hold_start_time) {
                        kr_max = (*test_cells[individual_cell_index]).get_i_kr();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_naca() < naca_min && time >= clamp_hold_start_time) {
                        naca_min = (*test_cells[individual_cell_index]).get_i_naca();
                    }
                }
                else if (multiple_clamp_type == 1) {
                    if (time > triangle_return_to_hold_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, l);
                        (*test_cells[individual_cell_index]).set_vm(clamp_holding_voltage);
                    }
                    else if (time > triangle_peak_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, l);
                        double vm_to_set = triangle_peak_voltage + (clamp_holding_voltage - triangle_peak_voltage) * (time - triangle_peak_time) / (triangle_return_to_hold_time - triangle_peak_time);
                        (*test_cells[individual_cell_index]).set_vm(vm_to_set);
                        test_cell_data << time << "\t" << (*test_cells[individual_cell_index]).get_vm() << "\t" << (*test_cells[individual_cell_index]).get_i_kr() << std::endl;
                    }
                    else if (time > clamp_peak_start_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, l);
                        double vm_to_set = clamp_holding_voltage + (triangle_peak_voltage - clamp_holding_voltage) * (time - clamp_peak_start_time) / (triangle_peak_time - clamp_peak_start_time);
                        (*test_cells[individual_cell_index]).set_vm(vm_to_set);
                    }
                    else if (time > clamp_hold_start_time) {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, l);
                        (*test_cells[individual_cell_index]).set_vm(clamp_holding_voltage);
                    }
                    else {
                        (*test_cells[individual_cell_index]).calc_i_all(time_step, solve_method, l);
                        (*test_cells[individual_cell_index]).calc_vm(time_step, solve_method, false);
                    }
                    //Now check for currents being mins
                    if ((*test_cells[individual_cell_index]).get_i_na() < Na_min && time >= clamp_hold_start_time) {
                        Na_min = (*test_cells[individual_cell_index]).get_i_na();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_na() < CaL_min && time >= clamp_hold_start_time) {
                        CaL_min = (*test_cells[individual_cell_index]).get_i_cal();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_k1() < k1_min && time >= clamp_hold_start_time) {
                        k1_min = (*test_cells[individual_cell_index]).get_i_k1();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_to() > to_max && time >= clamp_hold_start_time) {
                        to_max = (*test_cells[individual_cell_index]).get_i_to();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_kr() > kr_max && time >= clamp_hold_start_time) {
                        kr_max = (*test_cells[individual_cell_index]).get_i_kr();
                    }
                    if ((*test_cells[individual_cell_index]).get_i_naca() < naca_min && time >= clamp_hold_start_time) {
                        naca_min = (*test_cells[individual_cell_index]).get_i_naca();
                    }
                }
                //Check to see if time needs to be reset
                if (time >= sim_time - coarse_time_step && clamp_voltage_current <= last_clamp_voltage) {
                    //Print to the file
                    clamping_data << clamp_voltage_current << "\t" << Na_min << "\t" << CaL_min << "\t" << k1_min << "\t" << to_max << "\t" << kr_max << "\t" << naca_min << std::endl;
                    //Reset parameters
                    Na_min = 999;
                    CaL_min = 999;
                    k1_min = 999;
                    to_max = -999;
                    kr_max = -999;
                    naca_min = 999;
                    time = 0;
                    std::cout << "\nCompleted simulation for " << clamp_voltage_current * 1E3 << " mV, resetting parameters and moving on to next voltage..." << std::endl;
                    clamp_voltage_current += clamp_voltage_step;
                    (*test_cells[individual_cell_index]).reset();
                }
            }
        array_counter++;
        time += time_step;
    }
    test_cell_data.close();
    am_test_currents.close();
    

	//Now export any parameters if necessary
	std::cout << "\nNow exporting test cells for initialisation..." << std::endl;
	for (int i{}; i < 4; i++) {
		test_cells[i]->export_cell(i);
	}
	std::cout << "\nExport complete!" << std::endl;
    delete_me();
}

void simulator::one_dimensional()
{
    std::ofstream fast_cell_data {dirname + "/" + "fast_cells" + all_files_suffix + ".txt"};
	std::ofstream fast_indexes {dirname +"/" +"fast_indexes" + all_files_suffix + ".txt"};
	std::ofstream activation_times{dirname +"/" +"activation_times" + all_files_suffix + ".txt"};
	std::ofstream fast_types {dirname +"/" +"fast_types" + all_files_suffix + ".txt"};
	std::ofstream slow_types {dirname +"/" +"slow_types" + all_files_suffix + ".txt"};
	std::ofstream slow_indexes {dirname +"/" +"slow_indexes" + all_files_suffix + ".txt"};
	std::ofstream am_ions {dirname +"/" +"am_ions" + all_files_suffix + ".txt"};
	std::ofstream slow_cell_data {dirname +"/" +"slow_cells" + all_files_suffix + ".txt"};
	std::ofstream fast_cell_details {dirname +"/" +"fast_cell_details" + all_files_suffix + ".txt"};
	std::ofstream slow_cell_details {dirname +"/" +"slow_cell_details" + all_files_suffix + ".txt"};

    std::cout << "Beginning cell initialisation..." << std::endl;
	//Initialise array of cells, slow and fast pathway
	cell_base** fast_cells = new cell_base * [N_fast];
	cell_base** slow_cells = new cell_base * [N_slow];
	std::cout << "\nCell initialisation complete!" << std::endl;
	
	//Now generate the cells in the slow pathway
	for (int cell_number = 0; cell_number < N_slow; cell_number++) {
		if (cell_number < 75) {
			if (import_am == true) {
				am_cell* cell = new am_cell(0);
				(*cell).set_coupling_conductance((600E-9 - (100E-9 / (1 + exp((cell_number - 38.5) / -5))))); //jacks:500
				(*cell).set_g_na(.57);//NEW, removed atm
				slow_cells[cell_number] = cell;
			}
			else {
				am_cell* cell = new am_cell();
				(*cell).set_coupling_conductance((600E-9 - (100E-9 / (1 + exp((cell_number - 38.5) / -5))))); //jacks:500
				(*cell).set_g_na(.57);//NEW, removed atm
				slow_cells[cell_number] = cell;
			}
		}
		else {
			if (import_n == true) {
				av_node_2* cell = new av_node_2(1, 1);
				(*cell).set_coupling_conductance(500E-9); //jacks:300
				slow_cells[cell_number] = cell;
			}
			else {
				av_node_2* cell = new av_node_2(1);
				(*cell).set_coupling_conductance(500E-9); //jacks:300
				slow_cells[cell_number] = cell;
			}

		}
	}
	for (int i{}; i < N_slow; i++) {
		slow_cell_details << (*slow_cells[i]).get_cell_type() << " \t " << (*slow_cells[i]).get_cm() << " \t " << (*slow_cells[i]).get_g_na() << " \t " << (*slow_cells[i]).get_coupling_conductance() << std::endl;
	}

	//Now generate cells in the fast pathway
	for (int cell_number = 0; cell_number < N_fast; cell_number++) {
		if (cell_number < 125) {
			if (import_am == true) {
				am_cell* cell = new am_cell(0);
				if (cell_number < 75) {
					(*cell).set_coupling_conductance(550E-9);
				}
				else {
					(*cell).set_coupling_conductance((550E-9 - (200E-9 / (1 + exp((cell_number - 124.) / -5))))); //jacks:120.5
				}
				fast_cells[cell_number] = cell;
			}
			else {
				am_cell* cell = new am_cell();
				if (cell_number < 75) {
					(*cell).set_coupling_conductance(550E-9);
				}
				else {
					(*cell).set_coupling_conductance((550E-9 - (200E-9 / (1 + exp((cell_number - 124.) / -5))))); //jacks:120.5
				}
				fast_cells[cell_number] = cell;
			}

		}
		else if (cell_number < 200) {
			if (import_an == true) {
				av_node_2* cell = new av_node_2(16, 2);
				(*cell).set_coupling_conductance(400E-9);//500
				fast_cells[cell_number] = cell;
			}
			else {
				av_node_2* cell = new av_node_2(16);
				(*cell).set_coupling_conductance(400E-9);//500
				fast_cells[cell_number] = cell;
			}

		}
		else if (cell_number >= 200 && cell_number < 225) {
			if (import_n == true) {
				av_node_2* cell = new av_node_2(1, 1);
				(*cell).set_coupling_conductance(300E-9);
				(*cell).set_g_na(3.2E-13 / (1 + exp((cell_number + 25.0 - 238.0) / 2.5))); //jacks: 5 //CHANGE FROM 5E-13 TO 2.53E-9// 3.2 to 4
				fast_cells[cell_number] = cell;
			}
			else {
				av_node_2* cell = new av_node_2(1);
				(*cell).set_coupling_conductance(300E-9);
				(*cell).set_g_na(3.2E-13 / (1 + exp((cell_number + 25.0 - 238.0) / 2.5))); //jacks: 5//CHANGE FROM 5E-13 TO 2.53E-9 //3.2 to 4
				fast_cells[cell_number] = cell;
			}

		}
		else if (cell_number >= 225 && cell_number < 250) {
			if (import_n == true) {
				av_node_2* cell = new av_node_2(1, 1);
				if (cell_number < 250) {
					(*cell).set_coupling_conductance(300E-9);
					(*cell).set_g_na(5E-13 / (1 + exp((cell_number + 25.0 - 263.0) / -2.5))); //jacks orig:5//CHANGE FROM 5E-13 TO 2.53E-9
				}
				else {
					(*cell).set_coupling_conductance(500E-9); //jacks orig: 500
					//(*cell).set_g_na(3.2E-13);//NEW
				}
				fast_cells[cell_number] = cell;
			}
			else {
				av_node_2* cell = new av_node_2(1);
				if (cell_number < 250) {
					(*cell).set_coupling_conductance(300E-9);
					(*cell).set_g_na(5E-13 / (1 + exp((cell_number + 25.0 - 263.0) / -2.5))); //jacks orig:5//CHANGE FROM 5E-13 TO 2.53E-9
				}
				else {
					(*cell).set_coupling_conductance(500E-9); //jacks orig: 500
				}
				fast_cells[cell_number] = cell;
			}

		}
		else {
			if (import_nh == true) {
				av_node_2* cell = new av_node_2(19, 3);
				if (cell_number < 250) {
					(*cell).set_coupling_conductance(300E-9);//300
					(*cell).set_g_na(4E-13 / (1 + exp((cell_number + 25.0 - 263.0) / -2.5))); //jacks orig:5//CHANGE FROM 5E-13 TO 2.53E-9
				}
				else {
					(*cell).set_coupling_conductance(400E-9); //jacks orig: 500
					(*cell).set_g_na(5E-13);//NEW
				}
				fast_cells[cell_number] = cell;
			}
			else {
				av_node_2* cell = new av_node_2(19);
				if (cell_number < 250) {
					(*cell).set_coupling_conductance(300E-9);
					(*cell).set_g_na(4E-13 / (1 + exp((cell_number + 25.0 - 263.0) / -2.5))); //jacks orig:5//CHANGE FROM 5E-13 TO 2.53E-9
				}
				else {
					(*cell).set_coupling_conductance(400E-9); //jacks orig: 500
					(*cell).set_g_na(5E-13);//NEW
				}
				fast_cells[cell_number] = cell;
			}

		}
	}
	for (int i{}; i < N_fast; i++) {
		fast_cell_details << (*fast_cells[i]).get_cell_type() << " \t " << (*fast_cells[i]).get_cm() << " \t " << (*fast_cells[i]).get_g_na() << " \t " << (*fast_cells[i]).get_coupling_conductance() << std::endl;
	}

	// std::vector<float> firstam{};
	// std::vector<float> lastam{};
	// std::vector<float> middleslow{};
	// std::vector<float> middlefast{};
	// std::vector<float> firstbund{};
	// std::vector<float> endbund{};

    while (time <= sim_time) {
        bool i_bna_zero = false;
        double this_stim{ 0 };
        
        //Control stim time
        for (int noStim = 0; noStim < number_stims; noStim++) {
            if (time > (first_stim_time + noStim * stim_interval) && time < (first_stim_time + noStim * stim_interval) + stim_time) {
               // std::cout << "Stimulating at " << time << " s..." << std::endl;
               this_stim = stim_current;
            }
        }
        
        //Loop over fast pathway
        for (int i = 0; i < N_fast; i++) {
            (*fast_cells[i]).set_i_stim(0.0);
            (*fast_cells[i]).calc_i_all(time_step, solve_method, l);
            bool stim = false;

            if (i == 0) {
                double cell_current = (*fast_cells[i]).get_coupling_conductance() * ((*fast_cells[i]).get_vm() - (*fast_cells[i + 1]).get_vm());
                if ((*fast_cells[i]).get_cell_type() == 2) {
                    (*fast_cells[i]).set_i_stim(this_stim * am_cell_stim_multiplier * am_cell_unit_multiplier);
                }
                else {
                    (*fast_cells[i]).set_i_stim(this_stim);
                }
            }
            else if (i == 74) {
                // Due to three nearest neighbours
                double cell_current =
                    (*fast_cells[i]).get_coupling_conductance() * (3 * (*fast_cells[i]).get_vm() - (*fast_cells[i - 1]).get_vm() - (*fast_cells[i + 1]).get_vm() - (*slow_cells[0]).get_vm());
                cell_current +=
                    (((*fast_cells[i + 1]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() - (*fast_cells[i + 1]).get_vm())) / 4;
                cell_current +=
                    (((*fast_cells[i + 1]).get_coupling_conductance() - (*slow_cells[0]).get_coupling_conductance()) * ((*slow_cells[0]).get_vm() - (*fast_cells[i + 1]).get_vm())) / 4;
                cell_current +=
                    (((*slow_cells[0]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() - (*slow_cells[0]).get_vm())) / 4;
                if ((*fast_cells[i]).get_cell_type() == 2) {
                    //Due to different AMcell units
                    (*fast_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*fast_cells[i]).set_i_stim(cell_current);
                }
            }
            else if (i == 224) {
                double cell_current =
                    (*fast_cells[i]).get_coupling_conductance() * (3 * (*fast_cells[i]).get_vm() - (*fast_cells[i - 1]).get_vm() - (*fast_cells[i + 1]).get_vm() - (*slow_cells[N_slow - 1]).get_vm());
                cell_current +=
                    ((*fast_cells[i + 1]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() + (*fast_cells[i + 1]).get_vm()) / 4;
                cell_current +=
                    ((*fast_cells[i + 1]).get_coupling_conductance() - (*slow_cells[N_slow - 1]).get_coupling_conductance()) * ((*slow_cells[N_slow - 1]).get_vm() + (*fast_cells[i + 1]).get_vm()) / 4;
                cell_current +=
                    ((*slow_cells[N_slow - 1]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() + (*slow_cells[N_slow - 1]).get_vm()) / 4;
                if ((*fast_cells[i]).get_cell_type() == 2) {
                    //Due to different AMcell units
                    (*fast_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*fast_cells[i]).set_i_stim(cell_current);
                }
            }
            else if (i != 0 && i != 224 && i != (N_fast - 1) && i != 74) {
                double cell_current =
                    (*fast_cells[i]).get_coupling_conductance() * (2 * (*fast_cells[i]).get_vm() - (*fast_cells[i - 1]).get_vm() - (*fast_cells[i + 1]).get_vm());
                cell_current +=
                    ((*fast_cells[i + 1]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() - (*fast_cells[i + 1]).get_vm()) / 4;
                if ((*fast_cells[i]).get_cell_type() == 2) {
                    (*fast_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*fast_cells[i]).set_i_stim(cell_current);
                }
            }
            else if (i == N_fast - 1) {
                //Ghost cell with same vm as i
                double cell_current =
                    (*fast_cells[i]).get_coupling_conductance() * ((*fast_cells[i]).get_vm() - (*fast_cells[i - 1]).get_vm());
                cell_current +=
                    ((*fast_cells[i]).get_coupling_conductance() - (*fast_cells[i - 1]).get_coupling_conductance()) * ((*fast_cells[i - 1]).get_vm() - (*fast_cells[i]).get_vm()) / 4;
                if ((*fast_cells[i]).get_cell_type() == 2) {
                    (*fast_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*fast_cells[i]).set_i_stim(cell_current);
                }
            }
        }

        //Loop over slow pathway
        for (int i = 0; i < N_slow; i++) {
            (*slow_cells[i]).set_i_stim(0);
            (*slow_cells[i]).calc_i_all(time_step, solve_method, l);
            if (i == 0) {
                double cell_current = (*slow_cells[i]).get_coupling_conductance();
                cell_current *= (2 * (*slow_cells[i]).get_vm() - (*fast_cells[74]).get_vm() - (*slow_cells[i + 1]).get_vm());
                cell_current +=
                    ((*slow_cells[i + 1]).get_coupling_conductance() - (*fast_cells[74]).get_coupling_conductance()) * ((*fast_cells[74]).get_vm() - (*slow_cells[i + 1]).get_vm()) / 4;
                if ((*slow_cells[i]).get_cell_type() == 2) {
                    (*slow_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*slow_cells[i]).set_i_stim(cell_current);
                }
            }
            else if (i == (N_slow - 1)) {
                double cell_current =
                    (*slow_cells[i]).get_coupling_conductance() * (2 * (*slow_cells[i]).get_vm() - (*slow_cells[i - 1]).get_vm() - (*fast_cells[224]).get_vm());
                cell_current +=
                    ((*fast_cells[224]).get_coupling_conductance() - (*slow_cells[i - 1]).get_coupling_conductance()) * ((*slow_cells[i - 1]).get_vm() - (*fast_cells[224]).get_vm()) / 4;
                double test = cell_current;
                if ((*slow_cells[i]).get_cell_type() == 2) {
                    (*slow_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*slow_cells[i]).set_i_stim(cell_current);
                }
            }
            else {
                double cell_current =
                    (*slow_cells[i]).get_coupling_conductance() * (2 * (*slow_cells[i]).get_vm() - (*slow_cells[i - 1]).get_vm() - (*slow_cells[i + 1]).get_vm());
                cell_current +=
                    ((*slow_cells[i + 1]).get_coupling_conductance() - (*slow_cells[i - 1]).get_coupling_conductance()) * ((*slow_cells[i - 1]).get_vm() - (*slow_cells[i + 1]).get_vm()) / 4;
                if (cell_current != 0) {
                    double test = 0;
                }
                if ((*slow_cells[i]).get_cell_type() == 2) {
                    (*slow_cells[i]).set_i_stim(cell_current * am_cell_unit_multiplier);
                }
                else {
                    (*slow_cells[i]).set_i_stim(cell_current);
                }
            }
        }

        //Calculate action potentials
        for (int i = 0; i < N_fast; i++) {
            (*fast_cells[i]).calc_vm(time_step, solve_method, i_bna_zero);
        }
        for (int i = 0; i < N_slow; i++) {
            (*slow_cells[i]).calc_vm(time_step, solve_method, i_bna_zero);
        }

        //Print data
        if (array_counter % print_adjuster == 0) {
            fast_cell_data << time << "\t";
            for (int i = 0; i < N_fast; i++) {
                fast_indexes << i << "\t";
                fast_types << (*fast_cells[i]).get_cell_type() << "\t";
                double AP = (*fast_cells[i]).get_vm();
                fast_cell_data << AP << "\t";
            }
            slow_cell_data << time << "\t";
            for (int i = 0; i < N_slow; i++) {
                slow_indexes << i << "\t";
                slow_types << (*slow_cells[i]).get_cell_type() << "\t";
                double AP = (*slow_cells[i]).get_vm();
                slow_cell_data << AP << "\t";
            }
            fast_cell_data << std::endl;
            slow_cell_data << std::endl;
        }

        // if (array_counter % (print_adjuster/100) == 0) {
        // 	//Activations
        // 	if ((*fast_cells[1]).get_vm() >= activationPoint) {
        // 		firstam.push_back(time);
        // 	}
        // 	if ((*fast_cells[74]).get_vm() >= activationPoint) {
        // 		lastam.push_back(time);
        // 	}
        // 	if ((*fast_cells[174]).get_vm() >= activationPoint) {
        // 		middlefast.push_back(time);
        // 	}
        // 	if ((*fast_cells[249]).get_vm() >= activationPoint) {
        // 		firstbund.push_back(time);
        // 	}
        // 	if ((*fast_cells[324]).get_vm() >= activationPoint) {
        // 		endbund.push_back(time);
        // 	}
        // 	if ((*slow_cells[149]).get_vm() >= activationPoint) {
        // 		middleslow.push_back(time);
        // 	}
        // }
        array_counter++;
        double percent = (time / sim_time)*100;
        for (double j = 0; j <= 100; j++) {
            if (j > percent - 0.000005 && j < percent + 0.000005){
                std::cout << j << "% complete" <<std::endl;
            }
        }
        time += time_step;
    }
    // for (auto it : firstam) {
    // activation_times << it << "\t";
    // }
    // activation_times << "\n";
    // for (auto it : lastam) {
    // activation_times << it << "\t";
    // }
    // activation_times << "\n";
    // for (auto it : middlefast) {
    // activation_times << it << "\t";
    // }
    // activation_times << "\n";;
    // for (auto it : middleslow) {
    // activation_times << it << "\t";
    // }
    // activation_times << "\n";
    // for (auto it : firstbund) {
    // activation_times << it << "\t";
    // }
    // activation_times << "\n";
    // for (auto it :endbund) {
    // activation_times << it << "\t";
    // }
    // activation_times.close();
}
