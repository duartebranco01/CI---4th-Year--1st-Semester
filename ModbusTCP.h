#ifndef MODBUSTCP
#define MODBUSTCP

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>	//write
#include <stdint.h>
#include <strings.h> //bzero

int Send_Modbus_request(char* server_add, int port, uint8_t* APDU, uint16_t APDUlen, uint8_t* APDU_R);
//optional: Receive_Modbus_request(), Send_Modbus_response(), tentar fazer (podem ser testes)

#endif
