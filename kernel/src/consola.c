#include "../include/consola.h"
#include "../include/funciones_kernel.h"
#include <commons/log.h>

void iniciar_consola_interactiva(){
    
    //sleep(3);
    char* leido;
    leido = readline("> ");
    bool validacion_leido;

    while(1){ //strcmp(leido, "\0") != 0
        validacion_leido = _validacion_de_instruccion_de_consola(leido); 
        if(!validacion_leido){ //esta bien el primer comando?
            log_error(kernel_logger, "Comando de CONSOLA no reconocido");
            free(leido);
            leido = readline("> ");
            continue; //Saltar y continura con el resto de la iteracion
        }

        _atender_instruccion_validada(leido);
        free(leido);
        leido = readline("> ");
    }
    free(leido);
}    

bool _validacion_de_instruccion_de_consola(char* leido){ //esta funcion solo sirve para ver si escribiste bien el primer comando
    bool resultado_validacion = false;

    //[FALTA] Hacer hacer mas controles de validacion
    char ** comando_consola = string_split(leido, " ");

    if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "PROCESO_ESTADO") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "HELP") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "PRINT") == 0){
        resultado_validacion = true;
    }else{
        //log_error(kernel_logger, "Comando no reconocido.");
        resultado_validacion = false;
    }

    string_array_destroy(comando_consola);

    return resultado_validacion;
}

void _atender_instruccion_validada(char* leido){
    char** comando_consola = string_split(leido, " ");

    if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0){

        printf("%s\n",comando_consola[0]);
        printf("%s\n",comando_consola[1]);

        iniciar_proceso(comando_consola[1]);
        
        log_trace(kernel_log_debug, "Se creo un proceso\n");


    } else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){ 
        
        int * pidComando = malloc(sizeof(int));
        *pidComando = atoi(comando_consola[1]);
        liberarRecursosProceso(pidComando);
        
        free(pidComando);

    } else if (strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0){
        
        sem_wait(&sem_seguir_planificando);

    } else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0){

        //iniciar_cpu();
        sem_post(&sem_seguir_planificando);

    } else if (strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0){

    } else if (strcmp(comando_consola[0], "PROCESO_ESTADO") == 0){

    } else if (strcmp(comando_consola[0], "HELP") == 0){

    } else if (strcmp(comando_consola[0], "PRINT") == 0){

    } else {
        log_error(kernel_logger, "Comando no reconocido, pero que paso el filtro?");
        exit(EXIT_FAILURE);
    }
    string_array_destroy(comando_consola);
}



