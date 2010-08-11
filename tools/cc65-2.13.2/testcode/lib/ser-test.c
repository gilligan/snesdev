#include <stdio.h>
#include <stdlib.h>
#include <serial.h>
#include <conio.h>
#include <dbg.h>


#if defined(__C64__)
#define DRIVERNAME      "c64-swlink.ser"
#elif defined(__C128__)
#define DRIVERNAME      "c128-swlink.ser"
#elif defined(__PLUS4__)
#define DRIVERNAME	"plus4-stdser.ser"
#elif defined(__CBM610__)
#define DRIVERNAME	"cbm610-std.ser"
#else
#define DRIVERNAME	"unknown"
#error "Unknown target system"
#endif


static const struct ser_params Params = {
    SER_BAUD_19200,     /* Baudrate */
    SER_BITS_8,         /* Number of data bits */
    SER_STOP_1,         /* Number of stop bits */
    SER_PAR_NONE,       /* Parity setting */
    SER_HS_HW           /* Type of handshake to use */
};



static void CheckError (const char* Name, unsigned char Error)
{
    if (Error != SER_ERR_OK) {
        fprintf (stderr, "%s: %d\n", Name, Error);
        exit (EXIT_FAILURE);
    }
}


int main (void)
{
    char Res;
    char C;
    CheckError ("ser_load_driver", ser_load_driver (DRIVERNAME));
    CheckError ("ser_open", ser_open (&Params));
    while (1) {
        if (kbhit ()) {
            C = cgetc ();
            if (C == '1') {
                break;
            } else {
                CheckError ("ser_put", ser_put (C));
                printf ("%c", C);
            }
        }
        Res = ser_get (&C);
        if (Res != SER_ERR_NO_DATA) {
            CheckError ("ser_get", Res);
            printf ("%c", C);
        }
    }
    CheckError ("ser_unload", ser_unload ());

    return EXIT_SUCCESS;
}




