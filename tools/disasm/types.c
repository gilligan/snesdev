#include "proto.h"
#include <stdio.h>

extern int quiet;
extern FILE *srcfile;
extern unsigned int pc;
extern unsigned char types[];
extern char buff1[],buff2[];
extern int asmbler;
extern int accum,flag;
extern unsigned char bank;
extern int high, low, index;

void dotype(unsigned char t)
{
  int i, j, k;
  long ll;
  unsigned char h;
  char r, s, *msg;

  switch (t)
  {
    case 0 : if (!quiet) printf("         "); break;
    case 1 : i = getc(srcfile); pc++; if (!quiet) printf("%.2x ", i);
/* Accum  #$xx or #$xxxx */
             ll = i; strcat(buff2,"#");
             if ( (asmbler == 1) && (accum == 0) ) strcat(buff2,"<");
             if (accum == 1) { j = fgetc(srcfile); pc++; ll = j * 256 + ll;
               if (!quiet) printf("%.2x ", j); }
             msg = checksym(bank, ll); if (msg == NULL) if (accum == 0)
             sprintf(buff1, "$%.2x", ll); else sprintf(buff1, "$%.4x", ll);
             else sprintf(buff1, "%s", msg); strcat(buff2, buff1);
             if (!quiet) { printf("   "); if (accum == 0) printf("   "); }
             break;
    case 2 : i = getc(srcfile); j = getc(srcfile);
/* $xxxx */  if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "$%.2x%.2x", j, i); else sprintf(buff1, "%s",msg);
             strcat(buff2, buff1);
             pc += 2; high = j; low = i; break;
    case 3 : i = getc(srcfile); j = getc(srcfile); k = getc(srcfile);
/* $xxxxxx */ if (!quiet) printf("%.2x %.2x %.2x ", i, j, k);
             if (asmbler == 1) strcat(buff2, ">");
             msg = checksym(k, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "$%.2x%.2x%.2x", k, j, i);
             else sprintf(buff1, "%s", msg);  strcat(buff2, buff1); 
             pc += 3; break;
    case 4 : i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* $xx */    if (asmbler == 1) strcat(buff2, "<");
             msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "$%.2x", i);
             else sprintf(buff1, "%s", msg); strcat(buff2, buff1);
             pc++; break;
    case 5 : i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* ($xx),Y */ msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "($%.2x),Y", i);
             else sprintf(buff1, "(%s),Y" ,msg);
             strcat(buff2, buff1); pc++; break;
    case 6 : i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* [$xx],Y */ msg = checksym(bank, i);  if (msg == NULL)
             sprintf(buff1, "[$%.2x],Y", i);
             else sprintf(buff1,"[%s],Y",msg);
             strcat(buff2, buff1); pc++; break; 
    case 7 : i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* ($xx,X) */ msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "($%.2x,X)", i);
             else sprintf(buff1, "(%s,X)", msg);
             strcat(buff2, buff1); pc++; break;
    case 8 : i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* $xx,X */  msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "$%.2x,X", i);
             else sprintf(buff1, "%s,X", msg);
             strcat(buff2, buff1); pc++; break;
    case 9 : i = getc(srcfile); j = getc(srcfile);
/* $xxxx,X */ if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "$%.2x%.2x,X", j, i);
             else sprintf(buff1, "%s,X", msg);
             strcat(buff2, buff1); pc += 2; break;
    case 10: i = getc(srcfile); j = getc(srcfile); k = getc(srcfile);
/* $xxxxxx,X */ if (!quiet) printf("%.2x %.2x %.2x ", i, j, k);
             if (asmbler == 1) strcat(buff2, ">");
             msg = checksym(k, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "$%.2x%.2x%.2x,X", k, j, i);
             else sprintf(buff1, "%s,X", msg);
             strcat(buff2, buff1); pc += 3; break;
    case 11: i = getc(srcfile); j = getc(srcfile);
/* $xxxx,Y */ if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "$%.2x%.2x,Y", j, i);
             else sprintf(buff1, "%s,Y", msg);
             strcat(buff2, buff1);
             pc += 2; break;
    case 12: i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* ($xx) */  msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "($%.2x)", i); else sprintf(buff1, "(%s)", msg);
             strcat(buff2, buff1); pc++; break;
    case 13: i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* [$xx] */  msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "[$%.2x]", i); else sprintf(buff1, "[%s]", msg);
             strcat(buff2, buff1); pc++; break;
    case 14: i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* $xx,S */  msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "$%.x,S", i); else sprintf(buff1, "%s,S", msg);
             strcat(buff2, buff1); pc++; break;
    case 15: i = getc(srcfile); if (!quiet) printf("%.2x       ", i);
/* ($xx,S),Y */ msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "($%.2x,S),Y", i);
             else sprintf(buff1, "(%s,S),Y", msg);
             strcat(buff2, buff1); pc++; break;
    case 16: r = getc(srcfile); h = r; if (!quiet) printf("%.2x       ", h);
/* relative */ pc++; msg = checksym(bank, pc+r); if (msg == NULL)
             sprintf(buff1, "$%.4x", pc+r); else sprintf(buff1, "%s", msg);
             strcat(buff2, buff1); break;
    case 17: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* relative long */ if (!quiet) printf("%.2x %.2x    ",i,j);
             ll = j * 256 + i; if (ll > 32767) ll = -(65536-ll);
      msg = checksym((bank*65536+pc+ll)/0x10000, (bank*65536+pc+ll)&0xffff);
             if (msg == NULL) sprintf(buff1, "$%.6x", bank*65536+pc+ll);
             else sprintf(buff1, "%s", msg);
             strcat(buff2, buff1); break;
    case 18: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* #$xxxx */ if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
            sprintf(buff1, "#$%.2x%.2x", j, i); else sprintf(buff1,"#%s",msg);
             strcat(buff2, buff1); break;
/*    case 19: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* [$xxxx]  if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "[$%.2x%.2x]", j, i);
             else sprintf(buff1,"[%s]",msg);
             strcat(buff2, buff1); break;
don't really need this anymore.  will just comment it out before deleting
it */
    case 20: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* ($xxxx) */ if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
         sprintf(buff1, "($%.2x%.2x)", j, i); else sprintf(buff1, "(%s)", msg);
             strcat(buff2, buff1); break;
    case 21: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* ($xxxx,X) */ if (!quiet) printf("%.2x %.2x    ",i,j);
             msg = checksym(bank, j * 256 + i); if (msg == NULL)
             sprintf(buff1, "($%.2x%.2x,X)", j, i);
             else sprintf(buff1, "(%s,X)", msg);
             strcat(buff2,buff1); break;
    case 22: i = getc(srcfile); if (!quiet) printf("%.2x       ", i); pc++;
/* $xx,Y */  msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "$%.2x,Y", i); else sprintf(buff1, "%s,Y", msg);
             strcat(buff2, buff1); break;
    case 23: i = getc(srcfile); if (!quiet) printf("%.2x       ", i); pc++;
/* #$xx */   msg = checksym(bank, i); if (msg == NULL)
             sprintf(buff1, "#$%.2x", i); else sprintf(buff1, "#%s", msg);
             strcat(buff2, buff1); break;
    case 24: i = getc(srcfile); if (!quiet) printf("%.2x       ", i); pc++;
/* REP */    sprintf(buff1,"#$%.2x", i); strcat(buff2, buff1);
             if (i & 0x20) { accum = 1; flag = i; }
             if (i & 0x10) { index = 1; flag = i; }
             break;
    case 25: i = getc(srcfile); if (!quiet) printf("%.2x       ", i); pc++;
/* SEP */    sprintf(buff1, "#$%.2x", i); strcat(buff2, buff1);
             if (i & 0x20) { accum = 0; flag = -i; }
             if (i & 0x10) { index = 0; flag = -i; }
             break;
    case 26: i = getc(srcfile); pc++; if (!quiet) printf("%.2x ", i);
/* Index  #$xx or #$xxxx */
             ll = i; strcat(buff2,"#");
             if ( (asmbler == 1) && (index == 0) ) strcat(buff2,"<");
             if (index == 1) { j = fgetc(srcfile); pc++; ll = j * 256 + ll;
               if (!quiet) printf("%.2x ", j); }
             msg = checksym(bank, ll); if (msg == NULL) if (index == 0)
             sprintf(buff1, "$%.2x", ll); else sprintf(buff1, "$%.4x", ll);
             else sprintf(buff1, "%s", msg); strcat(buff2, buff1);
             if (!quiet) { printf("   "); if (index == 0) printf("   "); }
             break;
    case 27: i = getc(srcfile); j = getc(srcfile); pc += 2;
/* MVN / MVP */ if (!quiet) printf("%.2x %.2x    ", i, j);
             sprintf(buff1, "$%.2x,$%.2x", i, j); strcat(buff2, buff1); break;
    default: sprintf(buff2, "???"); break;
  }
}

