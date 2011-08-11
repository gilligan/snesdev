#include "proto.h"
#include <stdio.h>

extern
struct link
{
  char label[20];
  unsigned char bank;
  int address;
  struct link *next;
} dummy;
extern struct link *first;


void addlink(char *label, unsigned char b, int addr)
{
  struct link *newlink;

  newlink = (struct link *)malloc(sizeof(dummy));
  strcpy(newlink->label, label);
  newlink->bank = b;
  newlink->address = addr;
  newlink->next = first;
  first = newlink;
}

void loadsymbols(char *fname)
{
  FILE *sym;
  char lline[80], *label;
  char hold[80];
  char *line = (char *) lline;
  unsigned char bank;
  unsigned int addr;
  char a[2];
  int i = 0;

  sym = fopen(fname, "r");
  if (sym == NULL)
  {
    printf("Error -- Could not open %s for reading.\n");
    exit(-1);
  }

  while(!feof(sym))
  {
    strcpy(lline,"");
    strcpy(hold,"");
    fgets(lline, 80, sym);
    if (feof(sym)) break;

    strcpy(hold, lline);
    line = (char *) lline;

    label = (char *)strtok(line, "=$");
    if (strstr(label, " ") != NULL) label = (char*)strtok(label, " ");
    line = (char *) hold;
    line = (char *)strstr(line, "=$");
    line = line + 2; /* Move over the '=$' */
    sprintf(a, "%.2s", line);
    bank = hex(a);
    line += 2; /* Move over bank */
    addr = hex(line);
    addlink(label, bank, addr);
    i++;
  }
  printf("; Loaded %d symbols from %s\n", i, fname);
}

void *checksym(char bank, int pc)
{
  struct link *ptr;
  char *msg = NULL;

  ptr = first;
  while(ptr != NULL)
  {
    if (ptr->bank * 65536 + ptr->address == bank * 65536 + pc)
       msg = ptr->label;
    ptr = ptr->next;
  }

  return msg;
}

