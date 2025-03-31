#ifndef CORES_TOOL_H
#define CORES_TOOL_H

int cores_cnt();
double cores_freq();
void cores_plot(int r, int num_cores, double max_freq_ghz);
void print_cores(int row, int col, int r);

#endif
