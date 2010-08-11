/*****************************************************************************/
/*                                                                           */
/*                                em_load.c                                  */
/*                                                                           */
/*                       Loader module for EM drivers                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               R�merstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <modload.h>
#include <em.h>
#include <em/em-kernel.h>



unsigned char __fastcall__ em_load_driver (const char* name)
/* Load an extended memory driver and return an error code */
{
    static struct mod_ctrl ctrl = {
        read            /* Read from disk */
    };
    unsigned char Res;

    /* Check if we do already have a driver loaded. If so, remove it. */
    if (em_drv != 0) {
        em_uninstall ();
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
            return em_install (ctrl.module);

        }
    }

    /* Error loading the driver */
    return EM_ERR_CANNOT_LOAD;
}



