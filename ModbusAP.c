#include "ModbusAP.h"
#include "ModbusTCP.h"

#include <stdio.h>
#include <unistd.h>	
#include <stdint.h>

#define APDU_MAX 256
#define PORT_MIN 0
#define PORT_MAX 65535
#define STR_MIN 0
#define STR_MAX 65535
#define WR_REGS_MIN 1
#define WR_REGS_MAX 123
#define RD_REGS_MIN 1 
#define RD_REGS_MAX 123
#define WR_MULT_REGS_FUNC 0x10
#define WR_MULT_REGS_ERROR 0x90
#define RD_HOLD_REGS_FUNC 0x03
#define RD_HOLD_REGS_ERROR 0x83
#define EXCEPTION_CODE1 -101
#define EXCEPTION_CODE2 -102
#define EXCEPTION_CODE3 -103



int Write_multiple_regs(char* server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t* val)
{
    uint16_t APDUlen;
    uint8_t APDU[APDU_MAX]={}, APDU_R[5]={};

    st_r=st_r-1;
    

    //FAZER MAIS CHECKS
    if(port<PORT_MIN || port>PORT_MAX || st_r<STR_MIN || st_r>STR_MAX ||n_r<WR_REGS_MIN || n_r>WR_REGS_MAX)
    {
        printf("----------------ERROR: Wrong parameters.\n"); 
        return -1;
    }

    APDUlen = 1 + 2 + 2 +1 + 2*n_r;

    //Function code
    APDU[0] = (uint8_t) WR_MULT_REGS_FUNC;
    //Starting Address
    APDU[1] = (uint8_t) ((st_r>>8) & 0xFF); //HI
    APDU[2] = (uint8_t) (st_r & 0xFF); //LO
    //Quantaty of Registers
    APDU[3] = (uint8_t)((n_r>>8) & 0xFF);
    APDU[4] = (uint8_t) (n_r & 0xFF);
    //Byte count
    APDU[5] = (uint8_t) (2*n_r); //2bytes per register

    for(int i=0; i<n_r; i++) //data to write in registes, 2byte per register
    {
        APDU[6+2*i] = (uint8_t) ((val[i]>>8) & 0xFF); //HI
        APDU[6+2*i+1] = (uint8_t) (val[i] & 0xFF); //LO
    }

    printf("APDU:");
    for(int j=0; j<APDUlen; j++) printf(" %.2x", APDU[j]);
    printf("\n");




    //-------------------------SEND
    
    if(Send_Modbus_request(server_add, port, APDU, APDUlen, APDU_R) < 0)
    {
        printf("----------------ERROR: Send_Modbus_request\n"); 
        return -1;
    }

    //-------------------------READ RESPONSE
    
    if(APDU_R[0]==((uint8_t) WR_MULT_REGS_ERROR)) //Error code 0x90, Exception code APDU_R[1]
    {
        printf("----------------Positive Confirmation. Exception Code: %u\n", APDU_R[1]);

        if(APDU_R[1]==0x01) return EXCEPTION_CODE1;
        else if(APDU_R[1]==0x02) return EXCEPTION_CODE2;
        else if(APDU_R[1]==0x03) return EXCEPTION_CODE3;

        return -1;
    }
    else if(APDU_R[0]==WR_MULT_REGS_FUNC && APDU_R[1]==APDU[1] && APDU_R[2]==APDU[2] 
            && APDU_R[3]==APDU[3] && APDU_R[4]==APDU[4])
    {
        printf("----------------Positive Confirmation. %d registers written.\n", (int)((APDU_R[3]<<8) + APDU_R[4])); 
        return ((int)((APDU_R[3]<<8) + APDU_R[4])); //number of registers written
    }
    else
    {
        printf("----------------Negative Confirmation. Non expected function code or response format incorrect.\n"); 
        return -1;
    }


    //ver exeption codes

   /* MODBUS Response PDU
The function code must be verified and the MODBUS response format analyzed according to the MODBUS Application Protocol:
• if the function code is the same as the one used in the request, and if the response format is correct, then the MODBUS response is given to the user application as a Positive Confirmation.
• If the function code is a MODBUS exception code (Function code + 80H), the MODBUS exception response is given to the user application as a Positive Confirmation.
• If the function code is different from the one used in the request (=non expected function code), or if the format of the response is incorrect, then an error is signaled to the user application using a Negative Confirmation.*/


    return 0;
}

int Read_h_regs(char* server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t* val)
{
    st_r=st_r-1;
    
    uint16_t APDUlen;
    uint8_t APDU[APDU_MAX]={}, APDU_R[APDU_MAX]={};
    
    //FAZER MAIS CHECKS
    
    if(port<PORT_MIN || port>PORT_MAX || n_r<RD_REGS_MIN || n_r>RD_REGS_MAX) //125??????
    {
        printf("----------------ERROR: Wrong parameters.\n"); 
        return -1;
    }

    APDUlen = 1 + 2 + 2;
    
    //Function code
    APDU[0] = (uint8_t) RD_HOLD_REGS_FUNC;
    //Starting Address
    APDU[1] = (uint8_t) ((st_r>>8) & 0xFF); //HI
    APDU[2] = (uint8_t) (st_r & 0xFF); //LO
    //Quantaty of Registers
    APDU[3] = (uint8_t)((n_r>>8) & 0xFF);
    APDU[4] = (uint8_t) (n_r & 0xFF);

    printf("APDU:");
    for(int k=0; k<5; k++) printf(" %.2x", APDU[k]);
    printf("\n");


    //-------------------------SEND
    
    if(Send_Modbus_request(server_add, port, APDU, APDUlen, APDU_R) < 0)
    {
        printf("----------------ERROR: Send_Modbus_request\n"); 
        return -1;
    }

    //-------------------------READ RESPONSE

    if(APDU_R[0]==RD_HOLD_REGS_ERROR) //Error code 0x90, Exception code APDU_R[1]
    {
        printf("----------------Positive Confirmation. Exception Code: %u\n", APDU_R[1]);

        if(APDU_R[1]==0x01) return EXCEPTION_CODE1;
        else if(APDU_R[1]==0x02) return EXCEPTION_CODE2;
        else if(APDU_R[1]==0x03) return EXCEPTION_CODE3;

        return -1;
    }
    else if(APDU_R[0]==RD_HOLD_REGS_FUNC && ((uint16_t) APDU_R[1])==(2*n_r))
    {

        int count1=0, count2=2;

        while (count1<n_r)
        {
            val[count1]=(uint16_t)((APDU_R[count2]<<8) + APDU_R[count2+1]);
            printf("val[%d]: %d\n", count1, val[count1]);
            count1++;
            count2+=2;
        }
        
        
        
        printf("----------------Positive Confirmation. %d registers read.\n", n_r); 
        return n_r; //number of registers read
    }
    else
    {
        printf("----------------Negative Confirmation. Non expected function code or response format incorrect.\n"); 
        return -1;
    }

    return 0;
}