#ifndef HANDLING_H
#define HANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void signal_handler(int signum);
void request_handler(int signum);

#endif /* HANDLING_H */