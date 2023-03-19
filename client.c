#include "ModbusAP.h"
#include "ModbusTCP.h"

#include <stdio.h>
#include <stdlib.h>

#define PORT 502
#define ST_R 0
#define N_R 3

//o que retornar no exception

int main()
{
    /*int ret_wr=100, ret_rd=100;
    uint16_t val[N_R]={}, new_val[N_R];

    val[0]=10;
    val[1]=21;
    val[2]=20;

    ret_wr=Write_multiple_regs("127.0.0.1", PORT, ST_R, N_R, val);
    printf("ret_wr: %d\n", ret_wr);
    ret_rd=Read_h_regs("127.0.0.1", PORT, ST_R, N_R, new_val);
    printf("ret_rd: %d\n", ret_rd);

    for(int i=0; i<ret_rd; i++) printf("new_val[%d]: %d\n", i, new_val[i]);

    printf("\n");*/


    //CUIDADO AQUI!!!!!!!!!!!!!
    //MUDAR UNIT_ID ESTA A 2 NO .TCP E NO MODBUS 
    //MUDAR PARA 1 NO FIM, NA ENTREGA DEIXAR 2 OU O QUE FOR

    uint16_t read_val[3], D;
    int ret=100;

    //1 e 2
    printf("1 e 2\n");
    ret=Read_h_regs("127.0.0.1", 502, 153, 2, read_val);
    printf("ret: %d\n", ret);
    if(ret==-1) return -1;
    else if (ret<-1) 
    {
        ret=(abs(ret+100));
        printf("ret: %d\n", ret);
        return ret; //exption code 
    }

    printf("%d %d\n", read_val[0], read_val[1]);

    //3 
    printf("3\n");
    ret=Read_h_regs("127.0.0.1", 502, 159, 1, &read_val[2]);
    printf("ret: %d\n", ret);
    if(ret==-1) return -1;
    else if (ret<-1) 
    {
        ret=(abs(ret+100));
        printf("ret: %d\n", ret);
        return ret; //exption code 
    }


    printf("%d\n", read_val[2]);
    
    //8
    printf("8\n");
    if(read_val[2] == 0) D = 5555;
    else D = read_val[0] + read_val[1];

    //4
    printf("4\n");
    uint16_t w_55=55;


    ret=Write_multiple_regs("127.0.0.1", 502, 150, 1, &w_55);
    printf("ret: %d\n", ret);
    if(ret==-1) return -1;
    else if (ret<-1) 
    {
        ret=(abs(ret+100));
        printf("ret: %d\n", ret);
        return ret; //exption code 
    }


    //5
    printf("5\n");
    ret=Write_multiple_regs("127.0.0.1", 502, 151, 1, &D);
    printf("ret: %d\n", ret);
    if(ret==-1) return -1;
    else if (ret<-1) 
    {
        ret=(abs(ret+100));
        printf("ret: %d\n", ret);
        return ret; //exption code 
    }

    printf("5.2\n");
    ret=Write_multiple_regs("10.277.113.1", 502, 152, 1, &D);
    printf("ret: %d\n", ret);
    if(ret==-1) return -1;
    else if (ret<-1) 
    {
        ret=(abs(ret+100));
        printf("ret: %d\n", ret);
        return ret; //exption code 
    }
    

    return 0;
}