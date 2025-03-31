#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include "mem_tool.h"



void mem_graph(int samples, int r) {
	///_|> descry: draw the structure of Memory graph
	///_|> samples: number of samples the graph needs to show (the length), type integer
	///_|> r: the start row in the current terminal, type integer
	///_|> returning: no return

	struct sysinfo sys_info;
	int i, mem;
	
	printf("\x1b[%d;%df", r, 1);
	printf("v Memory        GB\n");
	printf("    GB |\n");
	for (i = 0; i < 11; i++)
		printf("       |\n");
	printf("  0 GB -");
	for (i = 0; i < samples; i++)
		printf("-");
	if (sysinfo(&sys_info) != 0) {
		perror("sysinfo");
	}
	mem = sys_info.totalram / (1024.0 * 1024.0 * 1024.0);		// total ram
	printf("\x1b[%d;%df", r + 1, 2);
	printf("%d", mem);
	printf("\x1b[%d;%df\033[2k", 50, 1);
}

void get_mem_usage(int *h, double *used_mem) {
	///_|> descry: get and calculate the memory usage (in GB) from "sys/sysinfo.h" and the height to print in the memory graph
	///_|> h: the pointer to the height we need to plot in the memory graph at the current colomn, type pointer of integer
	///_|> used_mem: the pointer to the usage of memory (in GB), type pointer of double
	///_|> returning: no return (return by the pointer of parameters)

	struct sysinfo sys_info;
	double total_memory, free_memory, unit;
	
	if (sysinfo(&sys_info) != 0) {
		perror("sysinfo");
	}
	total_memory = sys_info.totalram / (1024.0 * 1024.0 * 1024.0);		// total ram
	free_memory = sys_info.freeram/ (1024.0 * 1024.0 * 1024.0);			// free ram
	*used_mem = total_memory - free_memory;								// used ram
	unit = total_memory / 12;
	*h = (int) (*used_mem / unit) + 1;
}

void mem_plot(int row, int col, int h, double used_mem) {
	///_|> descry: ploting the memory usage onto the graph for each sample
	///_|> row: the base row of the current terminal to plot the graph, type integer
	///_|> col: the column of the current terminal to plot the graph, type integer
	///_|> h: the height to plot the graph compared to the row of the bottom structure, type integer
	///_|> used_mem: the memory usage to print on the top of the graph, type double
	///_|> returning: no return

	printf("\x1b[%d;%df", row, 11);
	printf("%.2f", used_mem);
	printf("\x1b[%d;%df", row + 13 - h, col);
	printf("#");
	printf("\x1b[%d;%df", 50, 1);
}