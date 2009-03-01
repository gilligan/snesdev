#include "png.h"

/** 
 * @brief Test if the input file is a valid PNG with indexed mode
 * and proper dimensions.
 * 
 * @param file png file to check
 * @return file handle if valid, NULL otherwise
 */
int verify_image(char* file, FILE** fp);
