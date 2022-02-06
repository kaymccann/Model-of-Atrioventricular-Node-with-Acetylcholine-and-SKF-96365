/*--------------------------------------------------------------------
-------- -
AVN Model Configuration File
21 / 03 / 2021	Created by Noah PHIPPS and Gemma LAMB
--------------------------------------------------------------------
------ */
#include"configuration.h"

//Parameters that need to be configured for the simulation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GENERAL PARAMETERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double sim_time = 10;//Time for whole simulation //50 for N, 6 for NH //11.5 for AN
int number_stims = 2000;//Use 20 as default, number of stimulations in the simulation

bool allow_main_simulation{ true }; //Allow the main simulation to run
bool allow_test_cells{ false }; //Allow the individual uncoupled test cells to run

double stim_time = 0.001; //Duration of the stimulations
double stim_current = (-1.2E-9);//-1.2E-9
double first_stim_time = 0.1;//Time of the first stimulation
double stim_interval = 0.4;//Interval between stimulations

bool SKF = false;//Apply SKF to 1D pathway
int l = 0;//Controls ACh concentration; usually set via command line 
//0 for 0, 1 for 10-6, 2 for 10-7, 3 for .5*10-7, 4 for .3*10-7, 5 for 10-8

std::string import_file_prefix{ "import_test_" };//Prefix for import files for initialising simulation
std::string all_files_suffix{ "_" + std::to_string(stim_interval) + "_" + std::to_string(SKF) + "_" + std::to_string(l)};
//Timestep details
double coarse_time_step = 5E-6;//Time step used initially
double fine_time_step = 5E-8;//Time step switched to if required, not implemented

double am_cell_unit_multiplier{ 1E12 }; //1E12, for different AM cell units
double am_cell_stim_multiplier{ 1.666666 };//Changes to 2nA, as required by protocol

int print_adjuster{ 200 };//Adjusts how many timesteps it is between each printing to file

double activationPoint{ -30E-3 }; // Reference point for activation potential activation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INITIALISATION PARAMETERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Allow or block initialisation of pathways using import files from previous simulations
bool import_am{ false };
bool import_n{ false };
bool import_an{ false };
bool import_nh{ false };

//If performing analysis on a single test cell to get parameters; test_cells_ .txt will be (time \t voltage \t dvdt) (dvdt currently incorrect, but good enough for peak analysis)
bool analyse_indivdial_cell{ true };
//Set the individual cell to analyse; index is for the test cell array, so 0=am, 1=n, 2=an, 3=nh
int individual_cell_index{ 1 };

int N_slow = 200; //Number of cells in the slow pathway
int N_fast = 325; //Number of cells in the fast pathway

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TEST CELL PARAMETERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Stimulations
bool stimulate_test_cells{ true };//Allow stimulation of the test cells
double test_cell_stim_interval{ stim_interval};//Control interval between stimulations
double test_stim_duration{ 0.001 };//Control stimulation duration

//Multiple clamps for voltage clamping analysis
bool clamp_multiple_voltages{ false };//Do multiple voltage clamping analysis
double first_clamp_voltage{ -40E-3 };//First voltage to be clamped
double last_clamp_voltage{ 40E-3 };//Last voltage to be clamped
double clamp_voltage_step{ 1E-3 };//Step between each clamping voltage
int multiple_clamp_type{ 0 };//0 for standard pulse, 1 for triangular
double triangle_peak_voltage{ 35E-3 };//Peak of the triangular voltage clamp
double triangle_peak_time{ 1.03194 };//Time at triangular clamp peak
double triangle_return_to_hold_time{ 1.20106 };//Time when returning to triangular holding voltage 

//Both types of clamps
double clamp_duration{ 300E-3 }; //duration of the clamp
double clamp_hold_start_time{ 0.5 };//Time at which holding voltage starts
double clamp_peak_start_time{ 1 };//Time to clamp peak
double clamp_holding_voltage{ -50E-3 };//Voltage before and after clamp

//Single clamp
bool clamp_test_cells{ false };
double clamp_peak_voltage{ -80E-3 };//Voltage for the single clamp

//Allowing cells to run
bool allow_am{ true };
bool allow_an{ true };
bool allow_nh{ true };
bool allow_n{ true };

int solve_method = 1;
