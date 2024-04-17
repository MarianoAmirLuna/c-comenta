#ifndef MAIN_H_
#define MAIN_H_

#include<stdio.h>
#include<stdlib.h>

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h> 


#include "utils.h"


t_log* iniciar_logger(void);
t_config* iniciar_config(char *rutaConexion);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
void iniciar_conexion(char *nombreIp, char *puertoIp,char *rutaConexion);

#endif /* CLIENT_H_ */