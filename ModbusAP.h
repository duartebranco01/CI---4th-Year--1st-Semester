#ifndef MODBUSAP
#define MODBUSAP

#include "ModbusTCP.h"

#include <stdio.h>
#include <unistd.h>	
#include <stdint.h>

int Write_multiple_regs(char* server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t* val);
int Read_h_regs(char* server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t* val);
// optional: Get_request(), Send_Response (tentar fazer, podem ser testes)

#endif