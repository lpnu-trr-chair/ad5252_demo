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
uint32 delay= 100;
uint8  step = 20;
char buffer[80];

void print_master_status(uint32 error_status)
{
uint32 possible_error_status[] = {I2C_PORT_I2C_MSTAT_RD_CMPLT,
    I2C_PORT_I2C_MSTAT_WR_CMPLT,
    I2C_PORT_I2C_MSTAT_XFER_INP,
    I2C_PORT_I2C_MSTAT_XFER_HALT,
    I2C_PORT_I2C_MSTAT_ERR_SHORT_XFER,
    I2C_PORT_I2C_MSTAT_ERR_ARB_LOST,
    I2C_PORT_I2C_MSTAT_ERR_BUS_ERROR,
    I2C_PORT_I2C_MSTAT_ERR_ABORT_XFER};

char *error_messages[] = {"-- RD_CMPLT  \r\n",
                          "-- WR_CMPLT  \r\n",
                          "-- XFER_INP  \r\n",
                          "-- XFER_HALT  \r\n",
                          "-- I2C error: ERR_SHORT_XFER  \r\n",
                          "-- I2C error: ERR_ARB_LOST  \r\n",
                          "-- I2C error: ERR_BUS_ERROR  \r\n",
                          "-- I2C error: ERR_ABORT_XFER  \r\n"
                        };
uint8 i=0;
sprintf(buffer,"- Master Status: %lu \r\n- Details:\r\n",error_status);
UART_PORT_UartPutString(buffer);
for (i=0; i<8 ;i++ )
    {
        if (error_status & possible_error_status[i])
        {
            sprintf(buffer,error_messages[i]);
            UART_PORT_UartPutString(buffer);
        
        };
    };  
}

void print_write_status(uint32 error_status){
    sprintf(buffer,"- Transfer status: %lu  \r\n- Details:\r\n", error_status);
    UART_PORT_UartPutString(buffer);
    
    switch(error_status)
    {
        
        case I2C_PORT_I2C_MSTR_NO_ERROR:
            sprintf(buffer,"-- Absent I2C error.  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_BUS_BUSY:
            sprintf(buffer,"-- I2C error: I2C_MSTR_BUS_BUSY  \r\n");
            break;
        case I2C_PORT_I2C_MSTR_NOT_READY:
            sprintf(buffer,"-- I2C error: I2C_MSTR_NOT_READY  \r\n");
            break;
//        case I2C_PORT_I2C_MSTR_ERR_LB_NAK:
//            sprintf(buffer,"I2C error: I2C_MSTR_ERR_LB_NAK  \r\n");
//            break;
//        case I2C_PORT_I2C_MSTR_ERR_ARB_LOST:
//            sprintf(buffer,"I2C error: I2C_MSTR_ERR_ARB_LOST  \r\n");
//            break;
//        case I2C_PORT_I2C_MSTR_ERR_BUS_ERR:
//            sprintf(buffer,"I2C error: I2C_MSTR_ERR_BUS_ERR  \r\n");            
//            break;
//        case I2C_PORT_I2C_MSTR_ERR_ABORT_START:
//            sprintf(buffer,"I2C error: I2C_MSTR_ERR_ABORT_START  \r\n");  
//            break;
//        case I2C_PORT_I2C_MSTR_ERR_TIMEOUT:
//            sprintf(buffer,"I2C error: I2C_MSTR_ERR_TIMEOUT  \r\n");  
//            break;
            
    }
    UART_PORT_UartPutString(buffer);
    
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_PORT_Start();
    I2C_PORT_Start();
    uint32 address=0b00000000000000000000000000101100; //chip 0, 7 bit 
                   // 0b00000000000000000000000000101101; //chip 1, 7 bit
    //uint32 rd_address=0b0101100;
    uint32 data_length=3;
    uint8 wr_buffer[3]={0b00000001, // RDAC1
        //0b00000011,// RDAC3
        // 0b11010001, // command increment RDAC1
        //0b10101001,  // command decrement RDAC1
        0b00000000,
        0b00000000,
    };
    uint8 rd_buffer[3] = {1,1,1};
    
    uint32 transacrtion_err = 0;
    uint32 master_status = 0;

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        sprintf(buffer,"========== Start cycle =========== \r\n");
        UART_PORT_UartPutString(buffer);
        
        sprintf(buffer,"Master status before transfer \r\n");
        UART_PORT_UartPutString(buffer);
        
        master_status = I2C_PORT_I2CMasterStatus();
        print_master_status(master_status);
        
        sprintf(buffer,"Write value: %d %d \r\n",wr_buffer[1],wr_buffer[2]);
        UART_PORT_UartPutString(buffer);
        
        transacrtion_err = I2C_PORT_I2CMasterWriteBuf(address,(uint8 *)wr_buffer, data_length, I2C_PORT_I2C_MODE_COMPLETE_XFER);
        while((I2C_PORT_I2CMasterStatus() & I2C_PORT_I2C_MSTAT_WR_CMPLT) == 0){};
        master_status =  I2C_PORT_I2CMasterStatus();
        
        sprintf(buffer,"Write transaction status: \r\n");
        UART_PORT_UartPutString(buffer);
        print_write_status(transacrtion_err);
        
        sprintf(buffer,"Master status after write transaction: \r\n");
        UART_PORT_UartPutString(buffer);
        print_master_status(master_status);
        
        
        sprintf(buffer,"Transferred bytes: %lu \r\n",I2C_PORT_I2CMasterGetWriteBufSize());
        UART_PORT_UartPutString(buffer);
        I2C_PORT_I2CMasterClearStatus();
        
        CyDelay(delay);
        transacrtion_err = I2C_PORT_I2CMasterReadBuf(address, (uint8 *)rd_buffer,1, I2C_PORT_I2C_MODE_COMPLETE_XFER);
        while((I2C_PORT_I2CMasterStatus() & I2C_PORT_I2C_MSTAT_RD_CMPLT) == 0){};
        master_status = I2C_PORT_I2CMasterStatus();

        sprintf(buffer,"Read transaction status: \r\n");
        UART_PORT_UartPutString(buffer);
        print_write_status(transacrtion_err);
        
        sprintf(buffer,"Read value: %d %d \r\n",rd_buffer[0],rd_buffer[1]);
        UART_PORT_UartPutString(buffer);
        
        sprintf(buffer,"Number of bytes read: %lu \r\n",I2C_PORT_I2CMasterGetReadBufSize());
        UART_PORT_UartPutString(buffer);
        
        sprintf(buffer,"Master status after read transaction: \r\n");
        UART_PORT_UartPutString(buffer);
        
        print_master_status(master_status);
        
        
        CyDelay(delay);
        wr_buffer[1]= wr_buffer[1]+step;
        wr_buffer[2]= wr_buffer[2]+step;
    }
}

/* [] END OF FILE */
