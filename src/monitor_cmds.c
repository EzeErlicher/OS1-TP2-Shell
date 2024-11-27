#include "monitor_cmds.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t monitor_pid = -1; // Trackea el PID del programa de monitoreo
int monitor_status = 0; // 0: Stopped, 1: Running

void start_monitor()
{
    if (monitor_status == 1)
    {
        printf("Monitor already running.\n");
        return;
    }

    if ((monitor_pid = fork()) == 0)
    {
        // proceso hijo
        execl("../OS1-TP1-Monitoring-Programm/build/monitoring_programm", "monitor", "../config.json", NULL);
        perror("Error starting monitor");
        exit(EXIT_FAILURE);
    }
    else if (monitor_pid > 0)
    {
        monitor_status = 1;
        printf("Monitor started with PID %d.\n", monitor_pid);
    }
    else
    {
        perror("Error forking monitor process");
    }
}

void stop_monitor()
{
    if (monitor_status == 0)
    {
        printf("Monitor is not running.\n");
        return;
    }

    kill(monitor_pid, SIGTERM);    // Envía la señal de terminación al monitor
    waitpid(monitor_pid, NULL, 0); // Espera a que el monitor cierre
    monitor_status = 0;
    monitor_pid = -1;
    printf("Monitor stopped.\n");
}

void status_monitor()
{
    if (monitor_status == 1)
    {
        printf("Monitor is running with PID %d.\n", monitor_pid);
    }
    else
    {
        printf("Monitor is not running.\n");
    }
}