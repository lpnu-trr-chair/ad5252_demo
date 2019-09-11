/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint32 delay=20000;
char buffer[80];

void print_error_status(uint32 error_status)
{
    switch(error_status)
    {
        
        case I2C_PORT_I2C_MSTR_NO_ERROR:
            sprintf(buffer,"Absent I2C error.  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_BUS_BUSY:
            sprintf(buffer,"I2C error: I2C_MSTR_BUS_BUSY  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_NOT_READY:
            sprintf(buffer,"I2C error: I2C_MSTR_NOT_READY  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_ERR_LB_NAK:
            sprintf(buffer,"I2C error: I2C_MSTR_ERR_LB_NAK  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_ERR_ARB_LOST:
            sprintf(buffer,"I2C error: I2C_MSTR_ERR_ARB_LOST  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_ERR_BUS_ERR:
            sprintf(buffer,"I2C error: I2C_MSTR_ERR_BUS_ERR  \r\n");            
            break;
        case I2C_PORT_I2C_MSTR_ERR_ABORT_START:
            sprintf(buffer,"I2C error: I2C_MSTR_ERR_ABORT_START  \r\n");  
            break;
        case I2C_PORT_I2C_MSTR_ERR_TIMEOUT:
            sprintf(buffer,"I2C error: I2C_MSTR_ERR_TIMEOUT  \r\n");  
            break;
            
    }
    UART_PORT_UartPutString(buffer);
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_PORT_Start();
    I2C_PORT_Start();
    uint32 address=0b0101100; //chip 0, 7 bit 
    //uint32 rd_address=0b0101100;
    uint32 data_length=2;
    uint8 wr_buffer[3]={//0b00000001, // RDAC1
        0b00000011,// RDAC3
        
        0b00000000,
        0b00000000,
    };
    uint8 rd_buffer[3] = {1,1,1};
    
    uint32 err = 0;

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        sprintf(buffer,"Write value: %d %d \r\n",wr_buffer[1],wr_buffer[2]);
        UART_PORT_UartPutString(buffer);
        err = I2C_PORT_I2CMasterWriteBuf(address,wr_buffer, data_length, I2C_PORT_I2C_MODE_COMPLETE_XFER);
        print_error_status(err);
        
        sprintf(buffer,"Transferred bytes: %lu \r\n",I2C_PORT_I2CMasterGetWriteBufSize());
        UART_PORT_UartPutString(buffer);
        err = I2C_PORT_I2CMasterReadBuf(address, rd_buffer,2, I2C_PORT_I2C_MODE_COMPLETE_XFER);
        sprintf(buffer,"Read value: %d %d \r\n",rd_buffer[0],rd_buffer[1]);
        UART_PORT_UartPutString(buffer);
        print_error_status(err);
        CyDelay(delay);
        
        wr_buffer[1]= wr_buffer[1]+10;
        wr_buffer[2]= wr_buffer[2]+10;
    }
}

/* [] END OF FILE */
