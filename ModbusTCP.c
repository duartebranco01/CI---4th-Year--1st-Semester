#include "ModbusTCP.h"
#include "ModbusAP.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>	//write
#include <stdint.h>
#include <strings.h> //bzero

#define MBAP_SIZE 7
#define APDU_MAX 256
#define PROTOCOL_ID 0
#define UNIT_ID 1
#define APDU_MAX 256
#define MIN_PORT 0
#define MAX_PORT 65535

uint16_t TI=0;

int Send_Modbus_request(char* server_add, int port, uint8_t* APDU, uint16_t APDUlen, uint8_t* APDU_R)
{
    
    //FAZER MAIS CHECKS
    int client_sockfd;
    uint16_t TI_R, protocolID_R, len_send, len_rcv;
    uint8_t unitID_R;
    uint8_t ADU[APDU_MAX+MBAP_SIZE]={}, MBAP_R[MBAP_SIZE]={};
    struct sockaddr_in client;

    TI++;

    if(port<MIN_PORT || port>MAX_PORT || APDUlen>APDU_MAX)
    {
        printf("----------------ERROR: Wrong parameters.\n"); 
        return -1;
    }
    
    //transaction identifier
    //PDU= strcat (strcat (generate MBAP, APDU)), cuidado com /0

    
    //-------------------------Server Connection

    if((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("----------------ERROR: socket\n"); 
        close(client_sockfd); 
        return -1;
    }
    else printf("Socket created\n");

    //puts(server_add);
    //printf("%d\n", port);
    
    bzero((char*)&client, sizeof(client)); // delete sizeof(server_adrr) bytes in pointer &server_addr
	client.sin_family = AF_INET; 
	inet_aton(server_add, &client.sin_addr);
    //inet_aton("127.0.0.1", &client.sin_addr);
	client.sin_port = htons(port);		//server TCP port must be network byte ordered. 

    if(connect(client_sockfd, (struct sockaddr *)&client, (socklen_t) sizeof(client)) < 0) 
    {
        printf("----------------ERROR: connect\n"); 
        close(client_sockfd); 
        return -1;
    }
    else printf("Connected\n");

    //-------------------------MBAP

    len_send = APDUlen + 1;

    //Transaction Identifier
    ADU[0] = (uint8_t) ((TI>>8) & 0xFF); //HI
    ADU[1] = (uint8_t) (TI & 0xFF); //LO
    //Protocol Identifier, 0=MODBUS
    ADU[2] = (uint8_t) ((PROTOCOL_ID>>8) & 0xFF);
    ADU[3] = (uint8_t) (PROTOCOL_ID & 0xFF);
    //Length, APDUlen+1, +1 for UnitID
    ADU[4] = (uint8_t) ((len_send>>8) & 0xFF); 
    ADU[5] = (uint8_t) (len_send & 0xFF);
    ADU[6] = (uint8_t) UNIT_ID;

    printf("MBAP:");
    for(int j=0; j<MBAP_SIZE; j++) printf(" %.2x", ADU[j]);
    printf("\n");

    //-------------------------Copy APDU

    for(int i=0; i<APDUlen + MBAP_SIZE; i++)
    {
        ADU[i+MBAP_SIZE]=APDU[i];
    }

    printf("ADU:");
    for(int j=MBAP_SIZE; j<APDUlen+MBAP_SIZE; j++) printf(" %.2x", ADU[j]);
    printf("\n");

    //-------------------------SEND
    
    if(write(client_sockfd, ADU, APDUlen+MBAP_SIZE)<0)
    {
        printf("----------------ERROR: write\n"); 
        close(client_sockfd); 
        return -1;
    }

    //-------------------------VERIFY MBAP_R

    if(read(client_sockfd, MBAP_R, MBAP_SIZE)<0) //read response MBAP for response lenght
    {
        printf("----------------ERROR: read\n"); 
        close(client_sockfd); 
        return -1;
    }

    printf("MBAP_R:");
    for(int j=0; j<MBAP_SIZE; j++) printf(" %.2x", MBAP_R[j]);
    printf("\n");


    TI_R = (MBAP_R[0]<<8) + MBAP_R[1];
    protocolID_R = (MBAP_R[2]<<8) + MBAP_R[3];
    len_rcv = (MBAP_R[4]<<8) + MBAP_R[5]; //len_rcv is lenght of response, its diferent from len_send
    unitID_R = MBAP_R[6];

    //CUIDADO AQUIII!!!!!!
    //!!!!!!!!!!!!!
    if(TI!=TI_R || ((uint16_t) PROTOCOL_ID)!=protocolID_R || ((uint8_t) UNIT_ID)!=unitID_R) 
    { 
        printf("----------------ERROR: Verify MBAP_R\n"); 
        close(client_sockfd); 
        return -1; //memset 0 no APDU_R??
    }

    //-------------------------READ RESPONSE TO APDU_r

    if(read(client_sockfd, APDU_R, len_rcv-1)<0) //read data, -1 cuz apdu
    {
        printf("----------------ERROR: read\n"); 
        close(client_sockfd); 
        return -1;
    }

    printf("APDU_R:");
    for(int j=0; j<len_rcv-1; j++) printf(" %.2x", APDU_R[j]);
    printf("\n");
    
    close(client_sockfd);
    return 0;

}