#!/usr/bin/python

import sys
import os
from PIL import Image

def usage():

    print "%s [v|h] <image> <basename>" % sys.argv[0]
    print ""
    print "v: slice image vertically"
    print "h: slice image horizontally"
    print ""
    print """iltool (interlace tool) splits an input image up into 2 images.
One containing the odd, the other the even (h)orizontal 
or (v)ertical lines of the input image."""
    sys.exit(0)

def build_images(mode,src_name,output_name):

    src_image = Image.open(src_name)
    w,h = src_image.size
    print "w:"+str(w)
    print "h:"+str(h)

    if mode == "v":
        output_width = (w/2)
        output_height = h
    if mode == "h":
        output_width = w
        output_height = (h/2)

    frame_1 = Image.new("RGB",(output_width,output_height))
    frame_2 = Image.new("RGB",(output_width,output_height))
    pix_data = src_image.load()

    f1_y = 0
    f2_y = 0
    f1_x = 0
    f2_x = 0


    if mode == "h":
        print "h"
        print output_width, output_height

        for y in range(0,h):
            if (y%2==0):
                for x in range(0,w):
                    frame_1.putpixel((x,f1_y),pix_data[x,y])
                f1_y = f1_y + 1
            else:
                for x in range(0,w):
                    frame_2.putpixel((x,f2_y),pix_data[x,y])
                f2_y = f2_y + 1

    if mode == "v":
        print "v"

        for x in range(0,w):
            if (x%2==0):
                for y in range(0,h):
                    frame_1.putpixel((f1_x,y),pix_data[x,y])
                f1_x = f1_x + 1
            else:
                for y in range(0,h):
                    frame_2.putpixel((f2_x,y),pix_data[x,y])
                f2_x = f2_x + 1


    frame_1.save(output_name+"_"+mode+"_even.png","PNG")
    frame_2.save(output_name+"_"+mode+"_odd.png","PNG")

def main():

    if len(sys.argv) != 4 or (sys.argv[1] != "v" and sys.argv[1] != "h"):
        usage()

    if not os.path.isfile(sys.argv[2]):
        print "Cannot open file: %s" % sys.argv[2]
        sys.exit(1)

    build_images(sys.argv[1],sys.argv[2],sys.argv[3])

main()
