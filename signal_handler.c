#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "signal_handler.h"


void sigint_handler(int sig) {
    ///_|> descry: signal handler function for SIGINT (Ctrl-C)
	///_|> sig: the signal number
	///_|> returning: no return

    char response[10];
    printf("\x1b[%d;%df\033[2K", 50, 1);
    printf("Received SIGINT (Ctrl-C). Do you want to quit? (y/n): ");
    fflush(stdin);
    fflush(stdout);
    
    fgets(response, 10, stdin);
    if (response[0] == 'y' || response[0] == 'Y') {
        printf("\x1b[%d;%df\033[2K", 51, 1);
        printf("Exiting program...\n");
        kill(0, SIGTERM);
        while (wait(NULL) > 0);
        exit(0);
    } else {
        printf("\x1b[%d;%df\033[2K", 51, 1);
        printf("Continuing execution...\n");
    }
}

void sigtstp_handler(int sig) {
    ///_|> descry: signal handler for SIGTSTP (Ctrl-Z) -> Ignore it
	///_|> sig: the signal number
	///_|> returning: no return
}

// Function to set up signal handlers
void setup_signal_handlers() {
    ///_|> descry: signal reinstallation for SIGINT and SIGTSTP
	///_|> no argument
	///_|> returning: no return

    struct sigaction sa_int, sa_tstp;

    // Setup SIGINT handler (Ctrl-C)
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    // Setup SIGTSTP handler (Ctrl-Z)
    sa_tstp.sa_handler = sigtstp_handler;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = 0;
    sigaction(SIGTSTP, &sa_tstp, NULL);
}