#include <stdio.h>
#include <stdlib.h>
#include "cpu_tool.h"


void cpu_graph(int samples, int r) {
	///_|> descry: draw the structure of CPU usage graph
	///_|> samples: number of samples the graph needs to show (the length), type integer
	///_|> r: the start row in the current terminal, type integer
	///_|> returning: no return

	int i;
	
	printf("\x1b[%d;%df", r, 1);
	printf("v CPU        %%\n");
	printf("  100%% |\n");
	for (i = 0; i < 9; i++)
		printf("       |\n");
	printf("    0%% -");
	for (i = 0; i < samples; i++)
		printf("-");
	printf("\x1b[%d;%df\033[2K", 50, 1);
}

void get_cpu_times(long long int *total, long long int *idle) {
	///_|> descry: function to get CPU times from "/proc/stat"
	///_|> total: the pointer to the current cumulative total time of cpu running, type long long integer
	///_|> idle: the pointer to the current cumulative free time of cpu running, type long long integer
	///_|> returning: no return (return by the pointer parameters)

    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("Failed to open /proc/stat");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); // Read first line (CPU stats)
    fclose(file);

    // Extract CPU time values
    long long int user, nice, system, idle_time, iowait, irq, softirq;
    sscanf(line, "cpu %lld %lld %lld %lld %lld %lld %lld",
		&user, &nice, &system, &idle_time, &iowait, &irq, &softirq);			// get categorical time

    *idle = idle_time;															// calculate the free time
    *total = user + nice + system + idle_time + iowait + irq + softirq;			// calculate the total time
}

void cpu_plot(long long int total_diff, long long int idle_diff, int row, int col) {
	///_|> descry: plot the cpu usage in percentage for each sample
	///_|> total_diff: the pointer to the difference of the cumulative total time of cpu running, type long long integer
	///_|> idle: the pointer to the difference of the cumulative free time of cpu running, type long long integer
	///_l> row: the base row to plot, type integer
	///_l> col: the column to plot, type integer
	///_|> returning: no return 

	int i = 0;
	double usage;
	
	usage = 100.0 * ((double)(total_diff - idle_diff) / (double)total_diff);
	if (usage == 0.0)
		i = 1;
	while (10.0 * i < usage) 
		i++;
	
	printf("\x1b[%d;%df", row, 8);
	printf("%.2f", usage);
	printf("\x1b[%d;%df", row + 11 - i, col);
	printf(":");
	printf("\x1b[%d;%df", 50, 1);
}