#ifndef CPU_TOOL_H
#define CPU_TOOL_H

void cpu_graph(int samples, int r);
void get_cpu_times(long long int *total, long long int *idle);
void cpu_plot(long long int total_diff, long long int idle_diff, int row, int col);

#endif