# System Monitoring Tool -- Concurrency & Signals

## 1. Metadata
### Author: Yuze Fu
### Date: 2025/03/29

## 2. Introduction
A viasualization program writen in C for monitoring Memory usage, CPU usage and getting information of the cores for Linux OS, whose running time is opitimized by using concurrency, especially pipe. In addition, the signal has been reinstalled which fits the needs of the program to the best.

## 3. Description of how I approach the problem
1. Use line chart to show the realtime usage of memory and cpu by choosing number of samples and time delay between each of samples.
2. Showing the number of cores and the max frequency, and drawing a sketch map of the cores.
3. Create child processes to treat memory usage, cpu usage, cores number, max frequency seperately and use pipes to let parent and child processes connect with each other, which optimizes running time by concurrency.
4. Reinstall signal handler. Change SIGINT (Ctrl + C) to be suspending, which lets the user to choose continue or terminate. Change SIGTSTP (Ctrl + Z) to be ignored. Both of the functions are designed to fit the program better.

## 4. Implementation
### 4.i 
First, use CLA to pass the graphs we need and parameters (samples and time delay) we want. Second, create at most 4 processes for each functionality of the program. Third, use a for loop to get real-time samples, one by one after specific time delay. In the loop, it sent signal to the child process through pipe and after the child process receives the signal, it will get the info and sends back to the parent process by pipe. After getting the info in the loop, it will plot on the structrue of the line chart.
### 4.ii Modules and Functions
1. main: main
2. mem_tool: mem_graph, get_mem_usage, mem_plot
3. cpu_tool: cpu_graph, get_cpu_times, cpu_plot
4. cores_tool: print_cores, cores_cnt, cores_freq, cores_plot
5. signal_handler: sigint_handler, sigtstp_handler, setup_signal_handlers
### 4.iii Founction Details
1. main: Shown in 4.i
2. mem_graph: draw the frame of memory usage chart based on samples to determine the length and total ram to determine the unit of one square
3. get_mem_usage: get memory usage (in GB) from "sys/sysinfo.h"
4. mem_plot: plot the memory usage onto the chart frame produced by "mem_graph"
5. cpu_graph: draw the frame of cpu usage chart based on samples to determine the length
6. get_cpu_times: get cpu usage（in cumulative total running time and free time）from "/proc/stat"
7. cpu_plot: plot the cpu usage (in 100%) onto the chart frame produced by "cpu_plot"
8. print_cores: print a draft graph for a single core
9. cores_cnt: count the number of cores (processors) by reading "/proc/cpuinfo"
10. cores_freq: get the max frequency from "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
11. cores_plot: print the draft graph for all cores and show the max frequency
12. sigint_handler: redesign the behavior of SIGINT (Ctrl + C)
13. sigtstp_handler: redesign the behavior of SIGTSTP (Ctrl + Z)
14. setup_signal_handlers: reinstall the SIGINT and SIGTSTP

## 5. Flow Chart
see the attached .png file

## 6. Steps to Run the Program
1. Download all .c files, .h files and the makefile into one folder.
2. Compile the executive file by typing `make` in the terminal
3. Run the program with pattern. 
`./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]`
(I assume the user will only type number for N and T)

## 7. Expected Results
1. `./myMonitoringTool`
Print memory and cpu graph with the default samples (20) and tdelay (500000us). Also print the graph for cores and max frequency
2. `./myMonitoringTool 35 25000` or `./myMonitoringTool --samples=35 --tdelay=25000`
Print memory and cpu graph with the samples = 35 and tdelay = 25000us. Also print the graph for cores and max frequency
3. `./myMonitoringTool --memory --cores`
Print memory graph with the default samples (20) and tdelay (500000us). Also print the graph for cores and max frequency
4. `./myMonitoringTool --samples=5 --tdelay=2000000 --cpu --memory`
Print cpu graph with samples = 5 and tdelay = 2000000us. Also print the graph for cores and max frequency
5. `./myMonitoringTool 35` or `./myMonitoringTool --samples=35`
Print memory and cpu graph with samples = 35 and the default tdelay (500000us). Also print the graph for cores and max frequency

## 8. Test Cases
1. `./myMonitoringTool 1 1 1`
Print error message "Typo: 1". Then, still print memory and cpu graph with samples = 1 and tdelay = 1us. Also print the graph for cores and max frequency
2. `./myMonitoringTool --mem`
Print error message "Typo: --mem"
3. `./myMonitoringTool 30 --samples=50`
It will not print error message but samples will be overwritten from 30 to 50. Then, print memory and cpu graph with samples = 50 and default tdelay (500000us). Also print the graph for cores and max frequency.

## 9. Disclaimers
1. The terminal has to be at least 60 lines long to run the program and get the visualization properly.
2. The program only fits Linux system
3. The user must have the permission to read from the files I have shown in 4.iii
4. The time will not exceed 1% if the samples is smaller than or equal to 80.
5. The time will not exceed 1% if the tdelay is bigger than or equal to 0.25s.

## 10. References
1. man page of `sys/sysinfo.h`, `unistd.h`, `/proc/cpuinfo`, `/sys/devices/system/cpu/cpu0/cpufreq/`
2. Escape Codes: "https://en.wikipedia.org/wiki/ANSI_escape_code"
3. CPU calculation: "https://www.idnt.net/en-US/kb/941772"
4. Posts on Stack Overflow (solving different warnings under stdc99 standard)