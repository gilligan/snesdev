/*****************************************************************************/
/*                                                                           */
/*                             tgi_load_driver.c                             */
/*                                                                           */
/*                       Loader module for TGI drivers                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <unistd.h>
#include <fcntl.h>
#include <modload.h>
#include <tgi.h>
#include <tgi/tgi-kernel.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void __fastcall__ tgi_load_driver (const char* name)
/* Install the given driver. This function is identical to tgi_load with the
 * only difference that the name of the driver is specified explicitly. You
 * should NOT use this function in most cases, use tgi_load() instead.
 */
{
    static struct mod_ctrl ctrl = {
        read            /* Read from disk */
    };
    unsigned Res;

    /* Check if we do already have a driver loaded. If so, remove it. */
    if (tgi_drv != 0) {
        tgi_unload ();
    }

    /* Now open the file */
    ctrl.callerdata = open (name, O_RDONLY);
    if (ctrl.callerdata >= 0) {

        /* Load the module */
        Res = mod_load (&ctrl);

        /* Close the input file */
        close (ctrl.callerdata);

        /* Check the return code */
        if (Res == MLOAD_OK) {

            /* Check the driver signature, install the driver */
            tgi_install (ctrl.module);
            return;

        }
    }

    /* Error loading the driver */
    tgi_error = TGI_ERR_CANNOT_LOAD;
}
