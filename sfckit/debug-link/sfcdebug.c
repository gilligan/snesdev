#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "linkregs.h"
#include "util.h"

extern "C" {
    void link_power();
    void link_reset();
    unsigned link_run();
    uint8_t link_read(unsigned addr);
    void link_write(unsigned addr, uint8_t data);
}

void exec_cmd(uint8_t* buffer, int buffer_pos, uint8_t cmd);

#define FMT_BUF_SIZE (8*sizeof(uintmax_t)+1)
static char buf[FMT_BUF_SIZE];
char *binary_fmt(uintmax_t x)
{
    char *s = buf + FMT_BUF_SIZE;
    *--s = 0;
    if (!x) *--s = '0';
    for(; x; x/=2) *--s = '0' + x%2;
    return s;
}


#define BUFFER_SIZE 32*1024


/*
 * libsnes callback : power up
 */
void link_power()
{
    printf("SFC-DEBUG: POWER UP\n");
    link_reset();
}

/**
 * libsnes callback : initialize
 */
void link_reset() 
{
    printf("SFC-DEBUG: RESET\n");
}

/**
 * Desc: libsnes callback : run
 */
unsigned link_run() 
{
    return 21477272u;
}

/**
 * Desc: libsnes callback : read (snes<-chip)
 * @return unused
 */
uint8_t link_read(unsigned addr) 
{
    return 0;
}


/**
 * Desc: libsnes callback : write (snes->chip)
 * @arg addr address written to
 * @arg data data written to 'address'
 *
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

        case REG_PUTC:
            printf("read byte : %2X\n",data);
            break;

        default:
            printf("unknown register: %x",(addr & 0xffff));
    }
}




void debug_crash(uint8_t* debug_data)
{
    uint8_t status_reg = debug_data[0];
    uint16_t pc_reg = (debug_data[2]<<8) | debug_data[1];
    uint8_t direct_bank = debug_data[3];
    uint16_t reg_a = debug_data[4];
    uint16_t reg_x = (debug_data[7]<<8) | debug_data[6];
    uint16_t reg_y = (debug_data[9]<<8) | debug_data[8];
    uint16_t reg_sp = (debug_data[11]<<8) | debug_data[10];

    /* print status register */
    printf("DEBUG INFO:\n");

    printf("\tAddress: $%02X:%02X\n",direct_bank, pc_reg);
    printf("\tStack:   $%04X\n",reg_sp);

    printf("\tP: %s%s%s%s%s%s%s%s ",
            ((status_reg&(1<<7))?"N":"n"),
            ((status_reg&(1<<6))?"V":"v"),
            ((status_reg&(1<<5))?"M":"m"),
            ((status_reg&(1<<4))?"X":"x"),
            ((status_reg&(1<<3))?"D":"d"),
            ((status_reg&(1<<2))?"I":"i"),
            ((status_reg&(1<<1))?"Z":"z"),
            ((status_reg&(1<<0))?"C":"c"));
    printf("($%2X)\n",status_reg);
    printf("\tA: $%04X (%d)\n\tX: $%04X (%d)\n\tY: $%04X (%d)\n",
            reg_a,reg_a,
            reg_x,reg_x,
            reg_y,reg_y);
}


/**
 * Execute the specified command
 */
void exec_cmd(uint8_t* buffer, int buffer_pos, uint8_t cmd)
{
    switch(cmd)
    {
        case CMD_PRINTF:
            printf("%s",buffer);
            break;

        case CMD_HEXDUMP:
            hexdump(buffer,buffer_pos);
            break;

        case CMD_CRASHDBG:
            debug_crash(buffer);
            break;

        default:
            printf("unknown command byte: %x\n",cmd);
            break;
    }
}
