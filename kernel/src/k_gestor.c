#include "../include/k_gestor.h"

t_planificacion tipoPlanificacion;

t_list *procesosNEW=NULL;
t_list *procesosREADY=NULL;
t_list *procesosSuspendidos=NULL;
t_list* lista_interfaces=NULL;
t_list *procesosEXIT=NULL;
t_list *lista_id_hilos;
t_list* procesos_READY_priori;

int procesoEXEC = 0;
pthread_mutex_t mutexExec = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lista_pcb_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t modificarLista = PTHREAD_MUTEX_INITIALIZER;
int pidGlobal = 0;
t_list *listQPrimas;
int tiempoTranscurrido=0;
int seguirPlanificando=1;
int ejecutandoProceso=0;
int contador_hilos = 0;
sem_t sem_cpu_libre;
sem_t esperar_devolucion_pcb;
sem_t esperar_carga_path_memoria;
sem_t contador_q;
sem_t nuevo_bucle;
sem_t ciclo_instruccion_io;
sem_t esperar_vuelva;
t_dictionary *dictQPrimas;
t_list* listaPCBs=NULL;
int flagCambioProceso;
bool primeraVezEjecuta=true;
char** nombresRecursos = NULL;
t_list* instanciasRecursos = NULL;
int cantidad_de_recursos = 0;
t_list *lista_recursos_y_bloqueados=NULL;
bool estaCPULibre = true;
t_list *listaPidsRecursos = NULL;
sem_t sem_seguir_planificando;
int estaEJecutando;
int quantum_global_reloj;
struct timespec start_time;
struct timespec end_time;
int consulta_pid_ejecucion;
sem_t esperar_consulta_pid;
sem_t esperar_termine_ejecutar_pcb_cpu;
pthread_mutex_t proteger_lista_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t proteger_mandar_pcb = PTHREAD_MUTEX_INITIALIZER;

