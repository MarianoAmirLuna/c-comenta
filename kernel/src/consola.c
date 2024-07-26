#include "../include/consola.h"
#include "../include/funciones_kernel.h"
#include <commons/log.h>


//REVISAR
void ejecutar_script(char* argumentos){ 
    ejecutar_archivo(argumentos);
    printf("El script es: %s\n", argumentos);       
}

void ejecutar_archivo(const char* filePath) {
    // Abrir el archivo y obtener las instrucciones
    FILE* file = fopen(filePath+1, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo de instrucciones.");
        return;
    }

    char lineas[256];
    while (fgets(lineas, sizeof(lineas), file) != NULL) {
        lineas[strcspn(lineas, "\n")] = '\0'; // Eliminar el salto de línea
        // Aquí puedes utilizar la variable "lineas" sin el salto de línea
    
        // Ejecutar cada instrucción en la consola
        // Aquí puedes llamar a la función que ejecuta una instrucción en la consola
        // Pasando la línea como parámetro
        //mensaje_a_consola(linea);
        if(_validacion_de_instruccion_de_consola(lineas))
        {
            _atender_instruccion_validada(lineas);
        }
    
    }
    
    fclose(file);

    // Terminar la función después de leer todas las líneas
    
}


void iniciar_consola_interactiva()
{

    // sleep(3);
    char *leido;
    leido = readline("> ");
    bool validacion_leido;

    while (1)
    { // strcmp(leido, "\0") != 0
        validacion_leido = _validacion_de_instruccion_de_consola(leido);
        if (!validacion_leido)
        { // esta bien el primer comando?
            log_error(kernel_logger, "Comando de CONSOLA no reconocido");
            free(leido);
            leido = readline("> ");
            continue; // Saltar y continura con el resto de la iteracion
        }

        _atender_instruccion_validada(leido);
        free(leido);
        leido = readline("> ");
    }
    free(leido);
}

void listar_procesos_estado()
{

    log_debug(kernel_log_debug, "PID: %d esta en EJECUCION", estaEJecutando);

    for (int i = 0; i < list_size(procesosNEW); i++)
    {
        int* pid = list_get(procesosNEW, i);
        log_debug(kernel_log_debug, "PID: %d esta en NEW", *pid);
    }

    for (int i = 0; i < list_size(procesosREADY); i++)
    {
        int* pid = list_get(procesosREADY, i);
        log_debug(kernel_log_debug, "PID: %d esta en READY", *pid);
    }

    for (int i = 0; i < list_size(procesosEXIT); i++)
    {
        int* pid = list_get(procesosEXIT, i);
        log_debug(kernel_log_debug, "PID: %d esta en EXIT", *pid);
    }

    for(int i = 0; nombresRecursos[i] != NULL ; i++){

        t_list *lista_donde_agregar = list_get(lista_recursos_y_bloqueados, i); //PREGUNTAR LUCA

        log_debug(kernel_log_debug, "Esta bloqueado por el recurso: %s", nombresRecursos[i]);

        for(int j = 0; j < list_size(lista_donde_agregar); j++){

            int* numeroXD = list_get(lista_donde_agregar,j);
            log_debug(kernel_log_debug, "PID: %d", *numeroXD);
        }
    }

    for (int i = 0; i < list_size(lista_interfaces); i++)
    {
        interfaces_io *interfaz = list_get(lista_interfaces, i);

        for(int j = 0; j < queue_size(interfaz->procesos_bloqueados); j++){

            int* pid = list_get(interfaz->procesos_bloqueados->elements,j);
            log_debug(kernel_log_debug, "PID: %d esta BLOQUEADO por %s", *pid, interfaz->nombre_interfaz);
        }
    }
}

bool _validacion_de_instruccion_de_consola(char *leido)
{ // esta funcion solo sirve para ver si escribiste bien el primer comando
    bool resultado_validacion = false;

    //[FALTA] Hacer hacer mas controles de validacion
    char **comando_consola = string_split(leido, " ");

    if (strcmp(comando_consola[0], "INICIAR_PROCESO") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "PROCESO_ESTADO") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "HELP") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "PRINT") == 0)
    {
        resultado_validacion = true;
    }
    else if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0)
    {
        resultado_validacion = true;
    }
    else
    {
        // log_error(kernel_logger, "Comando no reconocido.");
        resultado_validacion = false;
    }

    string_array_destroy(comando_consola);

    return resultado_validacion;
}

void _atender_instruccion_validada(char *leido)
{
    char **comando_consola = string_split(leido, " ");

    if (strcmp(comando_consola[0], "INICIAR_PROCESO") == 0)
    {

        printf("%s\n", comando_consola[0]);
        printf("%s\n", comando_consola[1]);

        iniciar_proceso(comando_consola[1]);

        log_trace(kernel_log_debug, "Se creo un proceso\n");
    }
    else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0)
    {

        int *pidComando = malloc(sizeof(int));

        *pidComando = atoi(comando_consola[1]);
        liberarRecursosProceso(pidComando); //deadlock
        mandar_a_exit(pidComando);

        if(*pidComando != estaEJecutando){
            finalizarProceso(*pidComando);  //eliminar de la memoria
        }
        
        free(pidComando);
        ciclo_planificacion();
    }
    else if (strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0)
    {

        sem_wait(&sem_seguir_planificando);
    }
    else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0)
    {

        // iniciar_cpu();
        sem_post(&sem_seguir_planificando);
    }
    else if (strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0)
    {

        GRADO_MULTIPROGRAMACION = atoi(comando_consola[1]);
    }
    else if (strcmp(comando_consola[0], "PROCESO_ESTADO") == 0)
    {

        listar_procesos_estado();
    }
    else if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0) 
    {
        ejecutar_archivo(comando_consola[1]);
    }
    else
    {
        log_error(kernel_logger, "Comando no reconocido, pero que paso el filtro?");
        exit(EXIT_FAILURE);
    }
    string_array_destroy(comando_consola);
}


 

