#include "../include/k_gestor.h"

t_planificacion tipoPlanificacion=FIFO;

t_list *procesosNEW=NULL;
t_list *procesosREADY=NULL;
int procesoEXEC=0;
pthread_mutex_t mutexExec = PTHREAD_MUTEX_INITIALIZER;
int quantum = 10;
int tiempoTranscurrido=0;
