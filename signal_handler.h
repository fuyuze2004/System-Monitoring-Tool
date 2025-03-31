#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

void setup_signal_handlers();
void sigint_handler(int sig);
void sigtstp_handler(int sig);

#endif