#include "../include/consola.h"

void iniciar_consola_interactiva(){
    char* leido;
    leido = readline("> ");
    bool validacion_leido;

    while(strcmp(leido, "\0") != 0){
        validacion_leido = _validacion_de_instruccion_de_consola(leido);
        if(!validacion_leido){
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

bool _validacion_de_instruccion_de_consola(char* leido){
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
        log_error(kernel_logger, "Comando no reconocido.");
        resultado_validacion = false;
    }

    string_array_destroy(comando_consola);

    return resultado_validacion;
}

void _atender_instruccion_validada(char* leido){
    char** comando_consola = string_split(leido, " ");
    t_buffer* un_buffer = crear_buffer();

    if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0){
        cargar_string_al_buffer(un_buffer, comando_consola[1]);
        cargar_string_al_buffer(un_buffer, comando_consola[2]);
        cargar_string_al_buffer(un_buffer, comando_consola[2]);
        _f_iniciar_proceso(un_buffer);

    } else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){

    } else if (strcmp(comando_consola[0], "DETERNER_PLANIFICACION") == 0){

    } else if (strcmp(comando_consola[0], "INICIAR_PLANICACION") == 0){

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

void _f_iniciar_proceso(t_buffer* un_buffer){
    char* path = extraer_string_del_buffer(un_buffer);
    char* size = extraer_string_del_buffer(un_buffer);
    char* prioridad = extraer_string_del_buffer(un_buffer);
    
    //log_trace(kerne_log_debug, "BUFFER(%d): [PATH:&s][SIZE:%s][PRIO:%s]", un_buffer->size, path, size, prioridad);
    destruir_buffer(un_buffer);

    int pid = asignar_pid();
    int size_num = atoi(size);

    //avisar a memoria
    t_buffer* a_enviar = crear_buffer();
    cargar_int_al_buffer(a_enviar, pid);
    cargar_string_al_buffer(a_enviar, path);
    cargar_int_al_buffer(a_enviar, size_num);
    t_paquete* un_paquete = crear_super_paquete(CREAR_PROCESO_KM, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    //falta la lógica para el funcaionamiento del kernel, va acá parece
}


