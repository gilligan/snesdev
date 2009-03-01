#include "read_image.h"

int verify_image(char* file, FILE** fp)
{
    unsigned char buf[4];

    if ((*fp = fopen(file,"rb"))==NULL)
        return -1;

    if (fread(buf,1,4,*fp) != 4)
    {
        fclose(*fp);
        return -1;
    }

    if (png_sig_cmp((unsigned char*)buf,(png_size_t)0,4)==0)
        return 0;

    return -1;
}
