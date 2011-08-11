#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

extern "C" {
    void link_power();
    void link_reset();
    unsigned link_run();
    uint8_t link_read(unsigned addr);
    void link_write(unsigned addr, uint8_t data);
}

void exec_cmd(uint8_t* buffer, int buffer_pos, uint8_t cmd);

#define BUFFER_SIZE 32*1024

/* register defines */
#define REG_DATA_WR    0x3800
#define REG_DATA_RD    0x3801
#define REG_CMD        0x3802

/* command defines */
#define CMD_PRINTF     0x00
#define CMD_HEXDUMP    0x01



/*
 * initialize
 */
void link_power()
{
    printf("SFC-DEBUG: POWER UP\n");
    link_reset();
}

/**
 * initialize
 */
void link_reset() 
{
    printf("SFC-DEBUG: RESET\n");
}

/**
 * does nothing right now
 */
unsigned link_run() 
{
    return 21477272u;
}

/**
 * write data to snes
 */
uint8_t link_read(unsigned addr) 
{
    return 0;
}


/**
 * receive data from snes
 */
void link_write(unsigned addr, uint8_t data) 
{
    static uint8_t output_buffer[BUFFER_SIZE];
    static int buffer_pos;

    switch (addr & 0xffff)
    {
        case REG_DATA_WR:
            output_buffer[buffer_pos] = data;
            buffer_pos = (buffer_pos==BUFFER_SIZE)?0:buffer_pos+1;
            break;

        case REG_CMD:
            exec_cmd(output_buffer, buffer_pos, data);
            buffer_pos = 0;
            memset(output_buffer, 0, BUFFER_SIZE);
            break;

        default:
            printf("unknown register: %x",(addr & 0xffff));
    }
}



static void hexdump(void *data, int size)
{
    /* dumps size bytes of *data to stdout. Looks like:
     * [0000] 75 6E 6B 6E 6F 77 6E 20
     *                  30 FF 00 00 00 00 39 00 unknown 0.....9.
     * (in a single line of course)
     */

    unsigned char *p = (unsigned char*)data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4x",
               ((intptr_t)p-(intptr_t)data) );
        }
            
        c = *p;
        if (isalnum(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) { 
            /* line completed */
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}




/**
 * Execute the specified command
 */
void exec_cmd(uint8_t* buffer, int buffer_pos, uint8_t cmd)
{
    printf("command byte: %d\n",cmd);
    switch(cmd)
    {
        case CMD_PRINTF:
            printf("%s",buffer);
            break;

        case CMD_HEXDUMP:
            hexdump(buffer,buffer_pos);
            break;

        default:
            printf("unknown command byte: %x\n",cmd);
            break;
    }
}
