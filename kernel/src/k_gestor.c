#include "../include/k_gestor.h"

t_planificacion tipoPlanificacion=RR;

t_list *procesosNEW=NULL;
t_list *procesosREADY=NULL;
int procesoEXEC=0;
pthread_mutex_t mutexExec = PTHREAD_MUTEX_INITIALIZER;
int quantum = 3;
int tiempoTranscurrido=0;
int seguirPlanificando=1;
int ejecutandoProceso=0;
sem_t sem_cpu_libre;
