#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "mem_tool.h"
#include "cpu_tool.h"
#include "cores_tool.h"
#include "signal_handler.h"


int main(int argc, char **argv) {
	setup_signal_handlers();

	int samples = 20, tdelay = 500000, i = 1, j, row[4] = {3, 18, 32, 50}, i_mem, i_cpu, h, num_cores = 0;
	double used_mem, max_freq = 0.0;
	long long int idle1, idle2, total1, total2;
	bool fmem = false, fcpu = false, fcores = false;
	
	// Checking positional arg "samples" and "tdelay"
	if (i < argc && argv[i][0] >= '0' && argv[i][0] <= '9') {
		samples  = atoi(argv[i]);
		i++;
		if (i < argc && argv[i][0] >= '0' && argv[i][0] <= '9') {
			tdelay = atoi(argv[i]);
			i++;
		}
	}
	// Checking flagged arg
	while (i < argc) {
		if (strcmp(argv[i], "--memory") == 0)	// Flagged arg "memory"
			fmem = true;
		else if (strcmp(argv[i], "--cpu") == 0)	// Flagged arg "cpu"
			fcpu = true;
		else if (strcmp(argv[i], "--cores") == 0)	// Flagged arg "cores"
			fcores = true;
		else if (strncmp(argv[i], "--samples=", 10) == 0)	// Flagged arg "samples"
			samples = atoi(strchr(argv[i], '=') + 1);
		else if (strncmp(argv[i], "--tdelay=", 9) == 0)	// Flagged arg "tdelay"
			tdelay = atoi(strchr(argv[i], '=') + 1);
		else
			fprintf(stderr, "Typo: %s\n", argv[i]);
		i++;
	}
	// Calling all 3 tools if no flagged arg
	if (fmem == false && fcpu == false && fcores == false) {
		fmem = true;
		fcpu = true;
		fcores = true;
	}
	
	// Setting the place of three tools on the terminal
	printf("\033[2J\033[H");
	i = 0;
	if (fmem || fcpu)
		printf(" Nbr of samples: %d -- every %d microSecs (%.3f secs)\n\n", 
			samples, tdelay, (double) tdelay / 1e6);
	if (fmem) {		// Function call in same order
		mem_graph(samples, row[i]);
		i_mem = i;
		i++;
	}
	if (fcpu) {
		cpu_graph(samples, row[i]);
		i_cpu = i;
		i++;
	}
	

	int mem_pipe[2], cpu_pipe[2];
    int mem_signal_pipe[2], cpu_signal_pipe[2];
	// Child process for memory info
	if (fmem) {
		if (pipe(mem_pipe) == -1 || pipe(mem_signal_pipe) == -1) {
			perror("pipe");
			exit(1);
		}
		pid_t pid_mem = -1;
		pid_mem = fork();
		if (pid_mem == 0) {
			struct sigaction sa_int, sa_tstp;

			// Setup SIGINT handler (Ctrl-C)
			sa_int.sa_handler = SIG_IGN;
			sigemptyset(&sa_int.sa_mask);
			sa_int.sa_flags = 0;
			sigaction(SIGINT, &sa_int, NULL);

			// Setup SIGTSTP handler (Ctrl-Z)
			sa_tstp.sa_handler = SIG_IGN;
			sigemptyset(&sa_tstp.sa_mask);
			sa_tstp.sa_flags = 0;
			sigaction(SIGTSTP, &sa_tstp, NULL);

			close(mem_pipe[0]);     
			close(mem_signal_pipe[1]);   
			int request = -1;
			while (request != samples - 1) {
				request = -1;
				read(mem_signal_pipe[0], &request, sizeof(int));		// Receive the signal from parent or block
				if (request != -1) {
					get_mem_usage(&h, &used_mem);						// mem_tool
					write(mem_pipe[1], &h, sizeof(int));
					write(mem_pipe[1], &used_mem, sizeof(double));		// write the info into the pipe
				}
			}
			close(mem_pipe[1]);     
			close(mem_signal_pipe[0]); 
			exit(0);
		}
		else if (pid_mem > 0) {
			close(mem_pipe[1]);
			close(mem_signal_pipe[0]);
		}
		else {
			perror("fork failed");
			exit(1);
		}
	}
	// Child process for cpu info
	if (fcpu) {
		if (pipe(cpu_pipe) == -1 || pipe(cpu_signal_pipe) == -1) {
			perror("pipe");
			exit(1);
		}
		pid_t pid_cpu = -1;
		pid_cpu = fork();
		if (pid_cpu == 0) {
			// Reset to ignore the handler function in child process
			struct sigaction sa_int, sa_tstp;
			sa_int.sa_handler = SIG_IGN;
			sigemptyset(&sa_int.sa_mask);
			sa_int.sa_flags = 0;
			sigaction(SIGINT, &sa_int, NULL);
			sa_tstp.sa_handler = SIG_IGN;
			sigemptyset(&sa_tstp.sa_mask);
			sa_tstp.sa_flags = 0;
			sigaction(SIGTSTP, &sa_tstp, NULL);

			close(cpu_pipe[0]);
			close(cpu_signal_pipe[1]);
			int request = -1;
			while (request != samples - 1) {
				request = -1;
				read(cpu_signal_pipe[0], &request, sizeof(int));	// Receive the signal or block
				if (request != -1) {
					get_cpu_times(&total2, &idle2);					// cpu_tool
					write(cpu_pipe[1], &total2, sizeof(long long int));
					write(cpu_pipe[1], &idle2, sizeof(long long int));		// Write the current cumulative time into the pipe
				}
			}
			close(cpu_pipe[1]);
			close(cpu_signal_pipe[0]);
			exit(0);
		}
		else if (pid_cpu > 0) {
			close(cpu_pipe[1]);
			close(cpu_signal_pipe[0]);
		}
		else {
			perror("fork failed");
			exit(1);
		}
	}

	// Plotting on the frame of memory and cpu
	if (fmem || fcpu) {
		get_cpu_times(&total2, &idle2);
		for (j = 0; j < samples; j++) {
			fflush(stdout);
			usleep(tdelay); // Small delay
			if (fmem) {
				write(mem_signal_pipe[1], &j, sizeof(int));  // Signal memory child
				read(mem_pipe[0], &h, sizeof(int));
				read(mem_pipe[0], &used_mem, sizeof(double));
				mem_plot(row[i_mem], j + 9, h, used_mem);		// mem_tool
			}

			if (fcpu) {
				total1 = total2;
				idle1 = idle2;
				write(cpu_signal_pipe[1], &j, sizeof(int));  // Signal CPU child
				read(cpu_pipe[0], &total2, sizeof(long long int));
				read(cpu_pipe[0], &idle2, sizeof(long long int));
				cpu_plot(total2 - total1, idle2 - idle1, row[i_cpu], j + 9);		// cpu_tool
			}
		}
	}

	// Treat the cores_tool
	int num_pipe[2], freq_pipe[2];
	if (fcores) {
		if (pipe(num_pipe) == -1 || pipe(freq_pipe) == -1) {
			perror("pipe");
			exit(1);
		}
		pid_t pid_num = -1, pid_freq = -1;
		// Process for the number of cores
		pid_num = fork();
		if (pid_num == 0) {
			close(num_pipe[0]);
			num_cores = cores_cnt();		// cores_tool
			write(num_pipe[1], &num_cores, sizeof(int));
			close(num_pipe[1]);
			exit(0);
		}
		else if (pid_num > 0) {
			close(num_pipe[1]);
			read(num_pipe[0], &num_cores, sizeof(int));
			close(num_pipe[0]);
		}
		else {
			perror("fork failed");
		}
		// Process for the max frequency
		pid_freq = fork();
		if (pid_freq == 0) {
			close(freq_pipe[0]);
			max_freq = cores_freq();		// cores_tool
			write(freq_pipe[1], &max_freq, sizeof(double));
			close(freq_pipe[1]);
			exit(0);
		}
		else if (pid_num > 0) {
			close(freq_pipe[1]);
			read(freq_pipe[0], &max_freq, sizeof(double));
			close(freq_pipe[0]);
		}
		else {
			perror("fork failed");
		}

		cores_plot(row[i], num_cores, max_freq);		// cores_tool

		i++;
	}

	// Collect the zombies
	if (fmem) {
		close(mem_pipe[0]);
		close(mem_signal_pipe[1]);
		wait(NULL);
	}
	if (fcpu) {
		close(cpu_pipe[0]);
		close(cpu_signal_pipe[1]);
		wait(NULL);
	}
	if (fcores) {
		wait(NULL);
		wait(NULL);
	}

	printf("\x1b[%d;%df\033[2K", 50, 1);
	printf("\x1b[%d;%df\033[2K", 51, 1);
	
	return 0;
}