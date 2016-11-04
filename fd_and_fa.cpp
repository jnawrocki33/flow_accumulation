#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <climits>
#include <time.h>


using namespace std;
#include "grid.hpp"

int const FLAT_SURFACE = 0;

int NO_DATA;

//prints the grid array data
void print_2d_array(grid grid_from_file) {
	for (int row = 0; row < grid_from_file.rows; row ++){
		for (int col = 0; col < grid_from_file.cols; col++) {
			cout << grid_from_file.data[row][col] << " ";
		}
		cout << endl;
	}
	cout << endl;
 }

//prints the header data of the grid
 void print_header(grid grid_from_file) {
 	cout << grid_from_file.header << endl;
 }

//prints relevant info about the grid
 void printInfo(grid grid_from_file) {
 	int min = INT_MAX;
 	int max = INT_MIN;
 	for (int row = 0; row < grid_from_file.rows; row++) {
 		for (int col = 0; col < grid_from_file.cols; col ++) {
 			if (grid_from_file.data[row][col] < min) { min = grid_from_file.data[row][col]; }
 			if (grid_from_file.data[row][col] > max) { max = grid_from_file.data[row][col]; }
 		}
 	}
 	cout << "This grid has " << grid_from_file.rows << " rows and " << grid_from_file.cols 
 	<< " columns, and has a min elevation of " << min << ", and a max elevation of " << max << endl;
     cout << "No data value is: " << NO_DATA << endl;
 }

//takes a file name and grid struct as parameters
 //reads the data of the file name appropriately into the grid
void read_file_into_array(string file_name, grid &grid_from_file) {

	//check for valid file name
	ifstream file;
	file.open( file_name.c_str());
	if (!file) {
		cout << "Please enter a valid file name..." << endl; 
		exit(1);
	}
	

	//initialize some variables
	int count = 0;
	string str; //will hold lines read from file
	int ** p; //pointer which will store grid data
	grid_from_file.header = ""; //initialize header of grid to blank string. read later

	while (getline(file, str)) {

		//count = 0, first line has column info
		if (count == 0) {
			string col_string;
			for (int i = 0; i < str.length(); i++) {
				if (isdigit(str[i])) {
					col_string.push_back(str[i]);
				}
			}

			grid_from_file.cols = atoi(col_string.c_str());
		}
		//second column has row info
		if (count == 1) {
			string row_string;
			for (int i = 0; i < str.length(); i++) {
				if (isdigit(str[i])) {
					row_string.push_back(str[i]);
				}
			}

			grid_from_file.rows = atoi(row_string.c_str());
		}
		//once columns and rows are read, allocate space for grid data to hold an array of int pointers
		if (count == 2) {
			p = (int **) malloc(grid_from_file.rows*sizeof(int *) );
			if (p == NULL) {exit(1);} //check for successful malloc
		}

		//get no_data value
		if (count == 5) {
			string no_data_string;
			for (int i = 0; i < str.length(); i++) {
				if (str[i] == '-' || isdigit(str[i])) {
					no_data_string.push_back(str[i]);
				}
			}

			NO_DATA = atoi(no_data_string.c_str());
		}
		//whenever count <= 5, the file is still reading header. read it into the grid's header
		if (count <= 5) {
			grid_from_file.header += str;
			grid_from_file.header += "\n";


		} 
		//else, build the array of this row by reading through the columns
		else {
			p[count - 6] = (int *) malloc(grid_from_file.cols * sizeof(int));
			if (p[count - 6] == NULL) {exit(1);} //check for succesfull malloc

			string build = "";
			int col = 0;
			for (int i = 0; i < str.length(); i++) {
				if (str[i] != ' ') {
					build += str[i];
				  } else {
					p[count - 6][col] = atoi(build.c_str());
				 	build = "";
				 	col += 1;
				}
			}
			p[count - 6][col] = atoi(build.c_str());
		}
		count++;
	}

	grid_from_file.data = p; //update the grid with the pointer to the array we created
}

//finds the lowest point in all 8 directions, for all points on the grid
void calculate_flow_diretion(grid &flow_direction, grid &grid_from_file) {


	for(int i = 0; i < flow_direction.rows; i++) {
		for(int j = 0; j < flow_direction.cols; j++) {

			if (grid_from_file.data[i][j] == NO_DATA) {
				flow_direction.data[i][j] = NO_DATA;
				continue;
			}

			bool on_left_edge = (j == 0);
			bool on_right_edge = (j == flow_direction.cols - 1);
			bool on_top_edge = (i == 0);
			bool on_bottom_edge = (i == flow_direction.rows - 1);

			int min = grid_from_file.data[i][j];
			int test;
			int direction = 1;
			bool any_downslope = false;

			//east
			if (!on_right_edge) {
				test = grid_from_file.data[i][j + 1];
				if ( (test < min) && (test != NO_DATA) ) { // if we found a new min, update the min and direction
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2; //multiply direction by powers of 2 as we change direction

			//south east
			if (!on_right_edge && !on_bottom_edge) {
				test = grid_from_file.data[i + 1][j + 1];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//south
			if (!on_bottom_edge) {
				test = grid_from_file.data[i + 1][j];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//south west
			if (!on_bottom_edge && !on_left_edge) {
				test = grid_from_file.data[i + 1][j - 1];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//west
			if (!on_left_edge) {
				test = grid_from_file.data[i][j - 1];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//north west
			if (!on_left_edge && !on_top_edge) {
				test = grid_from_file.data[i - 1][j - 1];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//north
			if (!on_top_edge) {
				test = grid_from_file.data[i - 1][j];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}
			direction *= 2;

			//north east
			if (!on_top_edge && !on_right_edge) {
				test = grid_from_file.data[i - 1][j + 1];
				if ( (test < min) && (test != NO_DATA) ) {
					min = test;
					flow_direction.data[i][j] = direction;
					any_downslope = true;
				}
			}

			if (!any_downslope) {
				flow_direction.data[i][j] = FLAT_SURFACE; // if there is no downslope, we found a flat surface
			}
		}
	}
}

//accumulate flow function
//it doesn't need to implement dynamic programming since it is only called on the flat surfaces
//thus the recursion traces out each unique tree of flow
//
int accumulate(grid &flow_accumulation, grid &flow_direction, int row, int col) {

	bool on_left_edge = (col == 0);
	bool on_right_edge = (col == flow_direction.cols - 1);
	bool on_top_edge = (row == 0);
	bool on_bottom_edge = (row == flow_direction.rows - 1);

	int flow = 1;

	if (!on_right_edge && flow_direction.data[row][col + 1] == 16) {
		flow += accumulate(flow_accumulation, flow_direction, row, col + 1);
	}
	//south_east
	if (!on_right_edge && !on_bottom_edge && flow_direction.data[row + 1][col + 1] == 32) {
		flow += accumulate(flow_accumulation, flow_direction, row + 1, col + 1);
	}
	//south
	if (!on_bottom_edge && flow_direction.data[row + 1][col] == 64) {
		flow += accumulate(flow_accumulation, flow_direction, row + 1, col);
	}
	//south_west
	if (!on_bottom_edge && !on_left_edge && flow_direction.data[row + 1][col - 1] == 128) {
		flow += accumulate(flow_accumulation, flow_direction, row + 1, col - 1);
	}
	//west
	if (!on_left_edge && flow_direction.data[row][col - 1] == 1) {
		flow += accumulate(flow_accumulation, flow_direction, row, col - 1);
	}
	//north_west
	if (!on_left_edge && !on_top_edge && flow_direction.data[row - 1][col - 1] == 2) {
		flow += accumulate(flow_accumulation, flow_direction, row - 1, col - 1);
	}
	//north
	if (!on_top_edge && flow_direction.data[row - 1][col] == 4) {
		flow += accumulate(flow_accumulation, flow_direction, row - 1, col);
	}
	//north_east
	if (!on_top_edge && !on_right_edge && flow_direction.data[row - 1][col + 1] == 8) {
		flow += accumulate(flow_accumulation, flow_direction, row - 1, col + 1);
	}

	flow_accumulation.data[row][col] = flow;
	return flow;
}

//accumulate the flow for each tree, which starts at flat surfaces
//
void calculate_flow_accumulation(grid &flow_accumulation, grid &flow_direction) {

	//find sinks
	for (int i = 0; i < flow_direction.rows; i++) {
		for (int j = 0; j < flow_direction.cols; j++) {
			if (flow_direction.data[i][j] == FLAT_SURFACE) {
				accumulate(flow_accumulation, flow_direction, i, j);
			}
		}
	}
}

void calculate_flow_accumulation_slow_algorithm(grid &flow_accumulation, grid &flow_direction) {
	for (int i = 0; i < flow_direction.rows; i++) {
		for (int j = 0; j < flow_direction.cols; j++) {
			accumulate(flow_accumulation, flow_direction, i, j);
		}
	}
}

void allocate_memory_for_grid(grid &grid_to_allocate) {

	grid_to_allocate.data = (int **) malloc(grid_to_allocate.rows * sizeof(int *));
	if (grid_to_allocate.data == NULL) {exit(1);}
	for (int i = 0; i < grid_to_allocate.rows; i++) {
		grid_to_allocate.data[i] = (int *) malloc(grid_to_allocate.cols * sizeof(int));
		if (grid_to_allocate.data[i] == NULL) {exit(1);}
	}
}

void set_default_accumulation_values(grid &flow_grid, grid &g) {
	for (int i = 0; i < flow_grid.rows; i++) {
		for (int j = 0; j < flow_grid.cols; j++) {
			flow_grid.data[i][j] = 1;
		}
	}
}


int main(int argc, char * argv[]) {

	//read in command line arguments, checking for existence
	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
		cout << "Please enter file names for input grid, output flow direction, and output flow accumulation";
		exit(1);
	} else {
		cout << "You entered file name: " << argv[1] << endl;
	}

	string file_name = argv[1];
	string output_flow_direction = argv[2];
	string output_flow_accumulation = argv[3];

	cout << file_name << endl;

	//make a grid and read the file data into it
	grid grid_from_file;
	read_file_into_array(file_name, grid_from_file);
    
    //initialize and allocate the flow direction grid
	grid flow_direction;
	flow_direction.rows = grid_from_file.rows;
	flow_direction.cols = grid_from_file.cols;
	allocate_memory_for_grid(flow_direction);

    //initialize and allocate the flow accumulation grid
	grid flow_accumulation;
	flow_accumulation.rows = grid_from_file.rows;
	flow_accumulation.cols = grid_from_file.cols;
	allocate_memory_for_grid(flow_accumulation);
	set_default_accumulation_values(flow_accumulation, grid_from_file);

	cout << "Here is the original grid:" << endl;
	printInfo(grid_from_file);
	print_header(grid_from_file);
	//print_2d_array(grid_from_file);

	calculate_flow_diretion(flow_direction, grid_from_file);
	//write to file
	ofstream flow_direction_file(output_flow_direction.c_str());
	flow_direction_file << grid_from_file.header;
	for(int i = 0; i < flow_direction.rows; i++) {
		for (int j = 0; j < flow_direction.cols; j++) {
			flow_direction_file << flow_direction.data[i][j] << " ";
		}
		flow_direction_file << "\n";
	}

	clock_t start = clock();
	calculate_flow_accumulation(flow_accumulation, flow_direction);
	clock_t end = clock();
	clock_t ticks = end - start;
	double duration = ticks / (double) CLOCKS_PER_SEC;
	cout << "dynamic algorithm took: " << duration << " seconds" << endl;

	//write to file
	ofstream flow_accumulation_file(output_flow_accumulation.c_str());
	flow_accumulation_file << grid_from_file.header;
	for(int i = 0; i < flow_accumulation.rows; i++) {
		for (int j = 0; j < flow_accumulation.cols; j++) {
			flow_accumulation_file << flow_accumulation.data[i][j] << " ";
		}
		flow_accumulation_file << "\n";
	}

	//initialize new flow accumulation grid
	grid flow_accumulation2;
	flow_accumulation2.rows = grid_from_file.rows;
	flow_accumulation2.cols = grid_from_file.cols;
	allocate_memory_for_grid(flow_accumulation2);
	set_default_accumulation_values(flow_accumulation2, grid_from_file);

	start = clock();
	calculate_flow_accumulation_slow_algorithm(flow_accumulation, flow_direction);
	end = clock();
	ticks = end - start;
	duration = ticks / (double) CLOCKS_PER_SEC;
	cout << "non-dynamic algorithm took: " << duration << " seconds" << endl;

	free(grid_from_file.data);
	free(flow_direction.data);
    free(flow_accumulation.data);
    free(flow_accumulation2.data);

	return 0;
}


