#include "../include/k_gestor.h"

t_planificacion tipoPlanificacion=FIFO;//PREGUNTAR LUCA

t_list *procesosNEW=NULL;
t_list *procesosREADY=NULL;
t_list *procesosSuspendidos=NULL;
int procesoEXEC=0;
pthread_mutex_t mutexExec = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t modificarLista = PTHREAD_MUTEX_INITIALIZER;
int pidGlobal=0;
t_list *listQPrimas;
double quantum = 6;
double tiempoTranscurrido=0;
int seguirPlanificando=1;
int ejecutandoProceso=0;
sem_t sem_cpu_libre;
t_dictionary *dictQPrimas;
t_list* listaPCBs=NULL;
int flagCambioProceso;
bool primeraVezEjecuta=true;
int flagSeguirPlanificando = 1;
bool estaCPULibre = true;

