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
            log_trace(io_log_debug, "Desconexion de MEMORIA - IO");
            control_key = 0;
            break;
        default:
            log_warning(logger, "Operacion desconocida de MEMORIA - IO");
            break;
        }
    }
}
