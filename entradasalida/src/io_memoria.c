#include "../include/io_memoria.h"
#include <utils/shared.h>

void atender_io_memoria()
{
    bool control_key = 1;
    while (control_key)
    {
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op)
        {
        case MENSAJE:
            //
            break;
        case PAQUETE:
            //
            break;
        case -1:
            log_error(logger, "Desconexion de MEMORIA - IO");
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operacion desconocida de MEMORIA - IO");
            break;
        }
    }
}