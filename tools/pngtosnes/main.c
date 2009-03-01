#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <getopt.h>
#include <png.h>
#include <glib.h>

#include "read_image.h"

typedef enum output_mode_t {invalid=0,_2bpp=2,_4bpp=4,_8bpp=8,mode7=7,sprite=6} output_mode_t;

typedef struct bgmap_t {
    GSList tiles;
} bgmap_t;

typedef struct tile_t {
    unsigned char* data;
    int tile_num;
    int pal_num;
    int flags;
} tile_t;

typedef struct tile_info_t {
    enum size {_8_8=16, _16_16=256} size;
    int pal_num;
    output_mode_t mode;
} tile_info_t;

#define init_tile(tile,info)\
    tile->data = malloc(info->size*(sizeof(unsigned char)));\
    memset(tile->data,0,info->size*(sizeof(unsigned char)));\
    tile->pal_num = info->pal_num;

void print_usage(void);

int main(int argc, char** argv)
{

    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 img_width, img_height;
    int img_depth, color_type;

    int c;
    int option_index;
    int ret;

    int map_flag=1;
    char *input_file, *output_file;
    output_mode_t output_mode=4;
    int pal_number=0;
    int tile_offset=0;
    char* output_label=" ";

    FILE* img_fp;

    struct option long_options[] = {
        {"mode", required_argument, 0, 'm'},
        {"nomap", no_argument, &map_flag, 0},
        {"pal", required_argument, 0, 'p'},
        {"offset",required_argument, 0, 'o'},
        {"label",required_argument, 0, 'l'},
        {0,0,0,0}
    };

    if (argc==1)
        print_usage();

    while ( (c=getopt_long(argc,argv,"m:p:d:l:",long_options,&option_index)) != -1 ) {

        switch (c)
        {
            case 'm':
                if (optarg[0] == 's')
                    output_mode=sprite;
                else if (optarg[0] == '2')
                    output_mode = _2bpp;
                else if (optarg[0] == '4')
                    output_mode = _4bpp;
                else if (optarg[0] == '8')
                    output_mode = _8bpp;
                else if (optarg[0] == '7')
                    output_mode = mode7;
                else output_mode = invalid;
                break;
            case 'p':
                pal_number = atoi(optarg);
                break;
            case 'd':
                tile_offset = atoi(optarg);
                break;
            case 'l':
                output_label = optarg;
                break;
        }
    }

    if ((argc-optind)<2)
        print_usage();

    input_file = argv[optind++];
    output_file = argv[optind++];

    ret = verify_image(input_file,&img_fp);
    if (ret<0) {
        fprintf(stderr,"Error: Image %s does not seem to be a valid PNG image!\n",input_file);
        exit(1);
    }

    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL)) == NULL) {
        fclose(img_fp);
        fprintf(stderr,"Error: Failed reading png image info\n");
        exit(1);
    }


    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        fclose(img_fp);
        png_destroy_read_struct(&png_ptr,png_infopp_NULL, png_infopp_NULL);
        fprintf(stderr,"Error: Failed creating png info structure\n");
        exit(1);
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);
        fclose(img_fp);
        exit(1);
    }


    png_set_sig_bytes(png_ptr,4);
    png_init_io(png_ptr,img_fp);
    png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,png_voidp_NULL);

    img_width = png_get_image_width(png_ptr,info_ptr);
    img_height = png_get_image_height(png_ptr,info_ptr);
    color_type = png_get_color_type(png_ptr,info_ptr);
    img_depth = png_get_bit_depth(png_ptr,info_ptr);

    if ( ((img_width % 8)!=0) || ((img_height % 8) !=0) )
    {
        fprintf(stderr,"Error: Image width/height has to be multiple of 8 !\n");
        fprintf(stderr,"(%s : %lu x %lu)\n",input_file, img_width, img_height);

        png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);
        fclose(img_fp);
        exit(1);
    }

    png_bytep row_pointers[img_height];

    for (int row=0; row<img_height; row++)
        row_pointers[row] = png_malloc(png_ptr,png_get_rowbytes(png_ptr,info_ptr));

    png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);
    fclose(img_fp);

    return 0;
}

tile_t*  encode_tile(png_bytep gfx_data, const tile_info_t* info)
{
    tile_t* tile;
    init_tile(tile,info);

    switch (info->mode) {
        case _2bpp:
            break;
        case _4bpp:
            break;
        case _8bpp:
            break;
        case mode7:
            break;
        case sprite:
            break;
        case _invalid:
            break;
    }

}


void print_usage(void)
{
    printf("Usage:\n");
    printf("png2snes [options] input_file output_file\n");
    printf("options:\n");
    printf("\t-mode     specify output mode [2,4,7,8,\'s\'](default:4)\n");
    printf("\t-offset   specify offset for tile number (default:0)\n");
    printf("\t-map      enable bg map creation (default:on)\n");
    printf("\t-pal      select palette number (default:0)\n");
    printf("\t-label    specify label name for output\n");

    exit(0);
}
