#include <../include/memoria.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

/*
PCB iniciar_PCBxd()
{ // revisar si anda o hay que poner struct adelante


  PCB pcb;

  pcb.pid = 5;
  pcb.program_counter = 1;
  pcb.quantum = 3;
  pcb.registros_cpu.AX = 0;
  pcb.registros_cpu.BX = 0;
  pcb.registros_cpu.CX = 0;
  pcb.registros_cpu.DX = 0;
  pcb.registros_cpu.EAX = 0;
  pcb.registros_cpu.EBX = 0;
  pcb.registros_cpu.ECX = 0;
  pcb.registros_cpu.EDX = 0;
  pcb.registros_cpu.SI = 0;
  pcb.registros_cpu.DI = 0;

  return pcb;
}

bool condition_id_igualxd(void *elemento)
{
	PCB *dato = (PCB *)elemento;
	return (dato->pid == 5);
} */

int main() {

	/*
    PCB pcbxd = iniciar_PCBxd();

	t_list* listaxd = list_create();

	list_add(listaxd,&pcbxd);

    PCB* pcb = list_find(listaxd, condition_id_igualxd);

    printf("el pid recien salido de la lista: %d\n",pcb->pid);
	*/

	inicializar_memoria();
	
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);
   
	log_trace(memoria_log_debug, "listo para escuchar al CPU");
	
	fd_cpu = esperar_cliente(fd_memoria);  

	log_trace(memoria_log_debug, "listo para escuchar al KERNEL");
	fd_kernel = esperar_cliente(fd_memoria);

	log_trace(memoria_log_debug, "listo para escuchar al IO");
	fd_io = esperar_cliente(fd_memoria);

	//Atender los mensajes del kernel

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	pthread_detach(hilo_kernel);	
	
	//Atender los mensajes del CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);
	pthread_detach(hilo_cpu);

	//Atender los mensajes del IO
	pthread_t hilo_io;
	pthread_create(&hilo_io, NULL, (void*)atender_memoria_io, NULL);
	pthread_join(hilo_io, NULL);

	printf("llegue al while");

	while(1){
		printf("estoy en el while\n");
		sleep(20);
	}

	return 0;

}

/*
void atender_proceso(t_buffer* un_buffer){
  int pid = extraer_int_del_buffer(un_buffer);
  char* path = extraer_string_del_buffer(un_buffer);

  free(path);
}
*/