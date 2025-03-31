#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cores_tool.h"


void print_cores(int row, int col, int r) {
	///_|> descry: print one core
	///_|> row: the row of the current core
	///_|> col: the column of the current core
	///_|> r: the base row in the terminal of all cores, type integer
	///_|> returning: no return 

	printf("\x1b[%d;%df", r + 1 + row * 3, 1 + col * 6);
	printf("+---+\n");
	printf("\x1b[%d;%df", r + 2 + row * 3, 1 + col * 6);
	printf("|   |\n");
	printf("\x1b[%d;%df", r + 3 + row * 3, 1 + col * 6);
	printf("+---+\n");
}

int cores_cnt() {
	///_|> descry: get the number of cores from "/proc/cpuinfo" in the computer
	///_|> no argument
	///_|> returning: the number of cores, type: integer

	int num_cores = 0;
	char line[256];
	FILE *file;
	// Get the number of cores
	file = fopen("/proc/cpuinfo", "r");
	if (file == NULL) {
		perror("Error opening /proc/cpuinfo");
		exit(EXIT_FAILURE);
	}
	while (fgets(line, sizeof(line), file)) {		// read the number of processors (cores)
		if (strncmp(line, "processor", 9) == 0) {
			num_cores++;
		}
	}
	fclose(file);

	return  num_cores;
}

double cores_freq() {
	///_|> descry: get the max frequency of this computer form "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
	///_|> no argument
	///_|> returning: the max frequency (in ghz), type: double

	double max_freq_khz = 0.0, max_freq_ghz = 0.0;
	FILE *file;
	// Getting the max frequency
	file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (file) {
		if (fscanf(file, "%lf", &max_freq_khz) != 1) 
			perror("Error reading frequency");
		fclose(file);
    }
	max_freq_ghz = (double) max_freq_khz / 1e6;		// convert from khz to ghz

	return max_freq_ghz;
}

void cores_plot(int r, int num_cores, double max_freq_ghz) {
	///_|> descry: showing the number of cores and the max frequency
	///_|> r: the start row (top-left) of this print function, type integer
	///_|> num_cores: the number of cores, type integer
	///_|> max_freq_ghz: the max frequency, type double
	///_|> returning: no return

	int len = 0, wid = 0, i, j;
	// Printing the number and the graph
	printf("\x1b[%d;%df", r, 1);
	printf("v Number of Cores: %d @ %.2f GHz\n", num_cores, max_freq_ghz);
	for(i = 1; i * i <= num_cores; i++) {
		if (num_cores % i == 0) {
			wid = num_cores / i;
			len = i;
		}
	}
	for (i = 0; i < wid; i++)
		for (j = 0; j < len; j++)
			print_cores(j, i, r);
}