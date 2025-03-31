#ifndef MEM_TOOL_H
#define MEM_TOOL_H

void mem_graph(int samples, int r);
void get_mem_usage(int* h, double* used_mem);
void mem_plot(int row, int col, int h, double used_mem);

#endif
