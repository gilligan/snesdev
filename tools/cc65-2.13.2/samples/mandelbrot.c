/*****************************************************************************
 * mandelbrot sample program for cc65.                                       *
 *                                                                           *
 * (w)2002 by groepaz/hitmen, TGI support by Stefan Haubenthal               *
 *****************************************************************************/



#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <tgi.h>



/* Graphics definitions */
#define SCREEN_X        (tgi_getxres())
#define SCREEN_Y        (tgi_getyres())
#define MAXCOL          (tgi_getcolorcount())

#define maxiterations   32
#define fpshift         (10)
#define tofp(_x)        ((_x)<<fpshift)
#define fromfp(_x)      ((_x)>>fpshift)
#define fpabs(_x)       (abs(_x))

#define mulfp(_a,_b)    ((((signed long)_a)*(_b))>>fpshift)
#define divfp(_a,_b)    ((((signed long)_a)<<fpshift)/(_b))

/* Workaround missing clock stuff */
#if defined(__APPLE2__) || defined(__APPLE2ENH__)
#  define clock()	0
#  define CLK_TCK	1
#endif

/* Use static local variables for speed */
#pragma staticlocals (1);



void mandelbrot (signed short x1, signed short y1, signed short x2,
		 signed short y2)
{
    register unsigned char count;
    register signed short r, r1, i;
    register signed short xs, ys, xx, yy;
    register signed short x, y;

    /* calc stepwidth */
    xs = ((x2 - x1) / (SCREEN_X));
    ys = ((y2 - y1) / (SCREEN_Y));

    yy = y1;
    for (y = 0; y < (SCREEN_Y); y++) {
	yy += ys;
	xx = x1;
	for (x = 0; x < (SCREEN_X); x++) {
	    xx += xs;
	    /* do iterations */
	    r = 0;
	    i = 0;
	    for (count = 0; (count < maxiterations) &&
		 (fpabs (r) < tofp (2)) && (fpabs (i) < tofp (2));
		 ++count) {
		r1 = (mulfp (r, r) - mulfp (i, i)) + xx;
		/* i = (mulfp(mulfp(r,i),tofp(2)))+yy; */
		i = (((signed long) r * i) >> (fpshift - 1)) + yy;
		r = r1;
	    }
	    if (count == maxiterations) {
		tgi_setcolor (0);
	    } else {
		if (MAXCOL == 2)
		    tgi_setcolor (1);
		else
		    tgi_setcolor (count % MAXCOL);
	    }
	    /* set pixel */
	    tgi_setpixel (x, y);
	}
    }
}

int main (void)
{
    clock_t t;
    unsigned long sec;
    unsigned sec10;
    unsigned char err;

    clrscr ();

    /* Load the graphics driver */                       
    cprintf ("initializing... mompls\r\n");
    tgi_load_driver (tgi_stddrv);
    err = tgi_geterror ();
    if (err  != TGI_ERR_OK) {
	cprintf ("Error #%d initializing graphics.\r\n%s\r\n",
		 err, tgi_geterrormsg (err));
	exit (EXIT_FAILURE);
    };
    cprintf ("ok.\n\r");

    /* Initialize graphics */
    tgi_init ();
    tgi_clear ();

    t = clock ();

    /* calc mandelbrot set */
    mandelbrot (tofp (-2), tofp (-2), tofp (2), tofp (2));

    t = clock () - t;

    /* Fetch the character from the keyboard buffer and discard it */
    (void) cgetc ();

    /* shut down gfx mode and return to textmode */
    tgi_done ();

    /* Calculate stats */
    sec = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;

    /* Output stats */
    cprintf ("time  : %lu.%us\n\r", sec, sec10);

    /* Wait for a key, then end */
    cputs ("Press any key when done...\n\r");
    (void) cgetc ();

    /* Done */
    return EXIT_SUCCESS;

}
