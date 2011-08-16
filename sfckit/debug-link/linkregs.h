#ifndef LINKREGS_H
#define LINKREGS_H

/* register defines */
#define REG_DATA_WR    0x3800
#define REG_DATA_RD    0x3801
#define REG_CMD        0x3802
#define REG_PUTC       0x3803

/* command defines */
#define CMD_PRINTF     0x00
#define CMD_HEXDUMP    0x01
#define CMD_CRASHDBG   0x02

#endif
