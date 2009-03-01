#define NUMINST	255

void find(unsigned char c);
extern unsigned char inst[];
extern struct link *first;
extern char *sym_filename;
extern unsigned char types[];
extern unsigned char type;
extern char mne[];
extern char *mnes[];






void find(unsigned char c)
{
  int i = 0;
  while( (i < NUMINST) && (c != inst[i]) ) i++;
  if (i == NUMINST) { type = -1; return; }
  strcpy(mne, mnes[i]);
  type = types[i];
}

unsigned int hex(char *s)
{
  int a, b, i, l = strlen(s);
  unsigned int total = 0;

  for(a=0; a<l; a++)
  {
    i = 0;
    if ( (s[a] > 47) && (s[a] < 58) ) i = s[a] - 48; /* 0 - 9 */
    if ( (s[a] > 64) && (s[a] < 71) ) i = s[a] - 55; /* A - F */
    if ( (s[a] > 96) && (s[a] < 103)) i = s[a] - 87; /* a - f */
    if (s[a] > 47) /* if valid character */
      total = total * 16 + i;
  }
  return total;
}

void spaces(int number)
{
  int i;
  printf("\n");
  for(i=0; i<number; i++) printf(" ");
}
