#include <stdio.h>
#include "disasm.h"
#include "proto.h"

extern void find(unsigned char c);

void main (int argc, char *argv[])
{
  int i, cc;
  unsigned char code, a, b;
  char *s, s2[80];
  double goin;
  struct link *ptr;

  flag = 0;

  if (argc < 2)
  {
    printf(USAGE);
    exit(-1);
  }

  if (!strcmp(argv[1], "-help"))
  {
    printf(USAGE);
    printf(HELP);
    exit(-1);
  }

  srcfile = fopen(argv[argc-1],"r");
  if (srcfile == NULL)
  {
    printf("Could not open %s for reading.\n", argv[argc-1]);
    exit(-1);
  }
  fseek(srcfile, 0, 0);
  i = 1; rts = 0;

  while ( (argc > 2) && ( (argv[i][0] == '/') || (argv[i][0] == '-') ) )
  {
    if ( (argv[i][0] == '/') || (argv[i][0] == '-') )
    {
      argv[i]++;
      switch(argv[i][0])
      {
        case 'b':
          argv[i]++;
          ptr = first;
          while (ptr != NULL)
          {
            if (!strcasecmp(ptr->label, argv[i]))
            {
              bank = ptr->bank;
              pc = ptr->address;
              flag = 1;
            }
            ptr = ptr->next;
          }
          if (flag == 0)
          {
            if (strlen(argv[i]) != 6)
            {
              printf("Beginning address invalid.\n");
              exit(-1);
            }
            s = argv[i] + strlen(argv[i]) - 4;/*last 4 #s .. ie 018000,s=8000 */
            sprintf(s2, "%.2s", argv[i]);
            pc = hex(s);
            bank = hex(s2);
          }
          flag = 0;
          break;

        case 'e':
          argv[i]++;
          ptr = first;
          while (ptr != NULL)
          {
            if (!strcasecmp(ptr->label, argv[i]))
            {
                endbank = ptr->bank;
                eend = ptr->address;
                flag = 1;
            }
            ptr = ptr->next;
          }
          if (flag == 0)
          {
            if (strlen(argv[i]) != 6)
            {
              printf("Ending address invalid.\n");
              exit(-1);
            }
            s = argv[i] + strlen(argv[i]) - 4;/*last 4 #s .. ie 018000,s=8000 */
            sprintf(s2, "%.2s", argv[i]);
            endbank = hex(s2);
            eend = hex(s);
          }
          flag = 0;
          break;

        case 'a':
          argv[i]++;
          if (argv[i][0] == '8') accum = 0;
          if (!strcmp(argv[i], "16")) accum = 1;
          break;

        case 'x':
          argv[i]++;
          if (argv[i][0] == '8') index = 0;
          if (!strcmp(argv[i], "16")) index = 1;
          break;

        case 's':
          if (argv[i][1] == 'k')
            fseek(srcfile, 512, 0);
          if (argv[i][1] == 'y')
          {
            sym_filename = argv[++i];
            loadsymbols(sym_filename);
          }
          break;

        case 'h':
          hirom = 1;
          break;

        case 'n':
          if (hirom)
            fseek(srcfile, 0xffea, 1);
          else
            fseek(srcfile, 0x7fea, 1);
          a = fgetc(srcfile); b = fgetc(srcfile);
          if (feof(srcfile))
          {
            printf("Error -- could not locate NMI vector\n");
           exit(-1);
          }
          pc = b * 256 + a;
          if (hirom)
            fseek(srcfile, -0xffec, 1);
          else
            fseek(srcfile, -0x7fec, 1);
          break;

        case 'r':
          if (hirom)
            fseek(srcfile, 0xfffc, 1);
          else
            fseek(srcfile, 0x7ffc, 1);
          a = fgetc(srcfile); b = fgetc(srcfile);
          if (feof(srcfile))
          {
           printf("Error -- could not locate RESET vector\n");
           exit(-1);
          }
          pc = b * 256 + a;
          if (hirom)
            fseek(srcfile, -0xfffe, 1);
          else
            fseek(srcfile, -0x7ffe, 1);
          break;

        case 'i':
          if (hirom)
            fseek(srcfile, 0xffee, 1);
          else
            fseek(srcfile, 0x7fee, 1);
          a = fgetc(srcfile); b = fgetc(srcfile);
          if (feof(srcfile))
          {
           printf("Error -- could not locate IRQ vector\n");
           exit(-1);
          }
          pc = b * 256 + a;
          if (hirom)
            fseek(srcfile, -0xfff0, 1);
          else
            fseek(srcfile, -0x7ff0, 1);
          break;

        case 't':
          rts = 1;
          break;

        case 'c':
          argv[i]++;
          comments = atoi(argv[i]);
          break;

        case 'q':
          quiet = 1;
          break;

        case 'j':
          asmbler = 1; /* Jeremy Gordon's Assembler */
          break;

        case 'd':
          dcb = 1;
          break;
      }
    }
    i++;
  }

  for(cc=0; cc<bank; cc++)  /* skip over each bank */
    if (hirom)
      fseek(srcfile, 65536, 1);
    else
      fseek(srcfile, 32768, 1);

  if (hirom)
    fseek(srcfile, pc, 1);
  else
    fseek(srcfile, pc - 0x8000, 1);

  printf("; Disassembled by:\n");
  printf("; 65816 SNES Disassembler   v2.0a (C)opyright 1994  by John Corey\n");
  printf("; Begin: $%.2x%.4x  End: ", bank, pc);
  if (rts)
    printf("RTS/RTL/RTI\n");
  else
    printf("$%.2x%.4x\n", endbank, eend);

  printf("; Hirom: ");
  if (hirom) printf("Yes"); else printf("No ");
  printf("  Quiet: ");
  if (quiet) printf("Yes"); else printf("No ");
  printf("  Comments: %d  DCB: ", comments);
  if (dcb) printf("Yes"); else printf("No ");
  printf("  Symbols: ");
  if (first != NULL) printf("Yes"); else printf("No ");
  printf("  65816: ");
  if (asmbler) printf("Yes"); else printf("No ");
  printf("\n");

  if (dcb)
  {
    dodcb(bank, pc, endbank, eend);
    exit(-1);
  }

  while( (!feof(srcfile)) && (bank * 65536 + pc < endbank * 65536 + eend)
    && (rts >= 0) )
  {
    code = getc(srcfile);
    find(code);
    if (!feof(srcfile))
    {
      sprintf(s2, "%.4x", pc);
      if (strlen(s2) == 5)
      {
        bank++;
        if (hirom)
          pc -= 0x10000;
        else
          pc -= 0x8000;
      }

      s = checksym(bank,pc);
      if (s != NULL)
        printf("%s", s);
      else if (!quiet)
        printf("%.2x%.4x", bank, pc);

      if (!quiet)
        printf(" %.2x ", code);
      else printf("	"); /* print tab (it's there!) */

      sprintf(buff2, "%s ", mne);
      if (rts)
        if ( (!strcmp(mne, "RTS")) || (!strcmp(mne, "RTI")) ||
           (!strcmp(mne, "RTL")) )
          rts = -1;
      pc++;
      high = 0; low = 0; flag = 0;
      dotype(type);
      if (!quiet) printf("  ");
      printf("%s", buff2);
      if (!quiet) printf("     ");

      if (high) comment(low, high);
      if (flag > 0)
      {
	printf("	;");
        if (flag & 0x10) printf(" Index (16 bit)");
        if (flag & 0x20) printf(" Accum (16 bit)");
      } else if (flag < 0)
      {
	printf("	;");
        if ( (-flag) & 0x10) printf(" Index (8 bit)");
        if ( (-flag) & 0x20) printf(" Accum (8 bit)");
      }
      printf("\n");
    }
    if (feof(srcfile)) printf("; End of file.\n");
  }
}
