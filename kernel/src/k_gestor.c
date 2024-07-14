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
int quantum = 60;
int tiempoTranscurrido=0;
int seguirPlanificando=1;
int ejecutandoProceso=0;
sem_t sem_cpu_libre;
sem_t esperar_devolucion_pcb;
t_dictionary *dictQPrimas;
t_list* listaPCBs=NULL;
int flagCambioProceso;
bool primeraVezEjecuta=true;
int flagSeguirPlanificando = 1;
char** nombresRecursos = NULL;
t_list* instanciasRecursos = NULL;
int cantidad_de_recursos=0;
t_list *lista_recursos_y_bloqueados=NULL;
bool estaCPULibre = true;
int estaEJecutando;

