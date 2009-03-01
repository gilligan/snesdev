#define USAGE "65816 SNES Disassembler   v2.0  (C)opyright 1994  by John \
Corey\ndisasm [options] <filename>\nfor help use: disasm -help\n"

#define HELP "  -skip     skip header (512 bytes)\n \
 -hirom    force HiRom disassembly\n \
 -nmi      begin disassembling at address in the NMI vector\n \
 -reset    begin disassembling at address in the RESET vector\n \
 -irq      begin disassembling at address in the IRQ vector\n \
 -bxxyyyy  begin  (default $008000)\n \
 -exxyyyy  end  (default $ffffff)\n \
             xx = bank number (in hex)\n \
             yyyy = address (in hex)\n \
 -t        stop disassembling at first RTS, RTL or RTI\n \
 -d        do NOT disassemble, convert block of code into dcb's.\n \
 -as       set accumulator size  (default 8)\n \
 -xs       set index size  (default 8)\n \
             s = 8 for 8 bit, s = 16 for 16 bit\n \
 -sym f    load up a symbol table.  f = filename\n \
 -q        quiet mode enable\n \
 -65816    disassemble with < and >'s\n \
 -cx       commenting level (default is 1)\n \
            x = 0 for no commenting, 1 for short comments, 2 for long comments\n"

#define NUMINST 255

unsigned char inst[] =
{
  0x69,0x6d,0x6f,0x65,0x71,0x77,0x61,0x75,0x7d,0x7f,0x79,0x72,0x67,0x63,0x73,
  0x29,0x2d,0x2f,0x25,0x31,0x37,0x21,0x35,0x35,0x3f,0x39,0x32,0x27,0x23,0x33,
  0x0e,0x06,0x0a,0x16,0x1e,  
  0x90,0xb0,0xf0,0x30,0xd0,0x10,0x80,0x82,0x50,0x70,
  0x89,0x2c,0x24,0x34,0x3c,
  0x00,0x18,0xd8,0x58,0xb8,
  0xc9,0xcd,0xcf,0xc5,0xd1,0xd7,0xc1,0xd5,0xdd,0xdf,0xd9,0xd2,0xc7,0xc3,0xd3,
  0xe0,0xec,0xe4,0xc0,0xcc,0xc4,
  0xce,0xc6,0x3a,0xd6,0xde,0xca,0x88,
  0x49,0x4d,0x4f,0x45,0x51,0x57,0x41,0x55,0x5d,0x5f,0x59,0x52,0x47,0x43,0x53,
  0xee,0xe6,0x1a,0xf6,0xfe,0xe8,0xc8,
  0x5c,0xdc,0x4c,0x6c,0x7c,0x22,0x20,0xfc,
  0xa9,0xad,0xaf,0xa5,0xb1,0xb7,0xa1,0xb5,0xbd,0xbf,0xb9,0xb2,0xa7,0xa3,0xb3,
  0xa2,0xae,0xa6,0xb6,0xbe,0xa0,0xac,0xa4,0xb4,0xbc,
  0x4e,0x46,0x4a,0x56,0x5e,0xea,
  0x09,0x0d,0x0f,0x05,0x11,0x17,0x01,0x15,0x1d,0x1f,0x19,0x12,0x07,0x03,0x13,
  0xf4,0xd4,0x62,0x48,0x8b,0x0b,0x4b,0x08,0xda,0x5a,0x68,0xab,0x2b,0x28,0xfa,0x7a,
  0xc2,0x2e,0x26,0x2a,0x36,0x3e,0x6e,0x66,0x6a,0x76,0x7e,
  0x40,0x6b,0x60,
  0xe9,0xed,0xef,0xe5,0xf1,0xf7,0xe1,0xf5,0xfd,0xff,0xf9,0xf2,0xe7,0xe3,0xf3,
  0x38,0xf8,0x78,0xe2,
  0x8d,0x8f,0x85,0x91,0x97,0x81,0x95,0x9d,0x9f,0x99,0x92,0x87,0x83,0x93,
  0xdb,0x8e,0x86,0x96,0x8c,0x84,0x94,0x9c,0x64,0x74,0x9e,
  0xaa,0xa8,0x5b,0x1b,0x7b,0x1c,0x14,0x0c,0x04,0x3b,0xba,0x8a,0x9a,0x9b,0x98,
  0xbb,0xcb,0xeb,0xfb,
  0x02,0x54,0x44
};

char *mnes[] =
{
  "ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC","ADC",
  "AND","AND","AND","AND","AND","AND","AND","AND","AND","AND","AND","AND","AND","AND","AND",
  "ASL","ASL","ASL","ASL","ASL",
  "BCC","BCS","BEQ","BMI","BNE","BPL","BRA","BRL","BVC","BVS",
  "BIT","BIT","BIT","BIT","BIT",
  "BRK","CLC","CLD","CLI","CLV",
  "CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP","CMP",
  "CPX","CPX","CPX","CPY","CPY","CPY",
  "DEC","DEC","DEC","DEC","DEC","DEX","DEY",
  "EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR","EOR",
  "INC","INC","INC","INC","INC","INX","INY",
  "JMP","JML","JMP","JMP","JMP","JSL","JSR","JSR",
  "LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA","LDA",
  "LDX","LDX","LDX","LDX","LDX","LDY","LDY","LDY","LDY","LDY",
  "LSR","LSR","LSR","LSR","LSR","NOP",
  "ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA","ORA",
  "PEA","PEI","PER","PHA","PHB","PHD","PHK","PHP","PHX","PHY","PLA","PLB","PLD","PLP","PLX","PLY",
  "REP","ROL","ROL","ROL","ROL","ROL","ROR","ROR","ROR","ROR","ROR",
  "RTI","RTL","RTS",
  "SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC","SBC",
  "SEC","SED","SEI","SEP",
  "STA","STA","STA","STA","STA","STA","STA","STA","STA","STA","STA","STA","STA","STA",
  "STP","STX","STX","STX","STY","STY","STY","STZ","STZ","STZ","STZ",
  "TAX","TAY","TCD","TCS","TDC","TRB","TRB","TSB","TSB","TSC","TSX","TXA","TXS","TXY","TYA",
  "TYX","WAI","XBA","XCE",
  "COP","MVN","MVP"
};

unsigned char types[] =
{
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  2, 4, 0, 8, 9,
  16, 16, 16, 16, 16, 16, 16, 17, 16, 16,
  1, 2, 4, 8, 9,
  0, 0, 0, 0, 0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  26, 2, 4, 26, 2, 4,
  2, 4, 0, 8, 9, 0, 0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  2, 4, 0, 8, 9, 0, 0,
  3, 20, 2, 20, 21, 3, 2, 21,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  26, 2, 4, 22, 11, 26, 2, 4, 8, 9,
  2, 4, 0, 8, 9, 0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  18, 23, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  24, 1, 4, 0, 8, 9, 1, 4, 0, 8, 9,
  0, 0, 0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  0, 0, 0, 25,
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  0, 2, 4, 8, 2, 4, 8, 2, 4, 8, 9,
  0, 0, 0, 0, 0, 2, 4, 2, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0,
  0, 27, 27
};

struct link
{
  char label[20];
  unsigned char bank;
  int address;
  struct link *next;
} dummy;

unsigned char type = -3;
char mne[3];
FILE *srcfile;
int accum = 0, index = 0;  /* 0 = 8 bit, 1 = 16 bit */
unsigned int pc = 0x8000, eend = 0xffff;
unsigned char bank = 0, endbank = 0xff;
char buff1[80], buff2[80];
int hirom = 0;	  /* 1 = HiRom */
int comments = 1; /* 0 = No Comments, 1 = Short Comments, 2 = Long Comments */
int quiet = 0;
int high, low, flag, rts;
int asmbler = 0, dcb = 0;
struct link *first = NULL;
char *sym_filename;

