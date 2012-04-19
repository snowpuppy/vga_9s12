#!/usr/bin/python
# FILE: convert.py
# PURPOSE: convert png to hex
# USAGE: convert.py filename.png

# DESCRIPTION:
# This python script converts images from a .png to zeros and ones corresponding
# to the scheme used to store our pixel values in memory. We are using 3bit pixels,
# but storing them in a byte and not using the remaining two bits. The 0's and 1's
# are not stored as binary, because they will be copied and hard coded into the
# C code that we're programming onto our microcontroller. Actually, I need to
# convert each one to a hex value to save space. That will be a little extra work.
# let's do binary first and then convert binary to hex.


# Functions:
def getColor(pixel):
    red1 = pixel[0]
    grn1 = pixel[1]
    blu1 = pixel[2]
    color1 = 0

    # test to make sure the pixel is valid
    if not (red1 == 255 or red1 == 0) or not (grn1 == 255 or grn1 == 0) or not (blu1 == 255 or blu1 == 0):
        if (red1 != 128 and grn1 != 64 and blu1 != 0):
            print "There is an invalid pixel value in this picture r = %3i g = %3i b = %3i" % (red1, grn1, blu1)
            exit()

    # extract the color values
		# [r1 g1 b1 r2 g2 b2 0 0]
    if red1 == 255:
        color1 = color1 + 4
    if grn1 == 255:
        color1 = color1 + 2
    if blu1 == 255:
        color1 = color1 + 1
    if red1 == 128 and grn1 == 64 and blu1 == 0:
        color1 = 3

    return color1

    

# sys used for argument passing
import sys
# import image manipulation library
from PIL import Image

# if no images passed, then exit with help info
if len(sys.argv) < 1 :
    print 'Usage: convert.py filename.png'

# open the image passed in
theimage = Image.open(sys.argv[1])

# open the file to write stuff to
outfilename = sys.argv[1]
if outfilename[0] == ".":
    outfilename = outfilename[2:]
outfilename = outfilename[0:-3] + 'h'
outfile = open(outfilename,'w') 
# get array of pixel values from the image
pixels = theimage.load()
imwidth = theimage.size[0]
imheigh = theimage.size[1]
imsize = (imwidth/2)*imheigh

# write comments and c-style syntax
outfile.write('// This is an automatically generated file.\n')
outfile.write('// This file is generated for %s.png by convert.py\n\n' % outfilename[0:-2] )
outfile.write('#ifndef %s_H\n\n' % outfilename[0:-2].upper() )
outfile.write('#define %s_H\n\n' % outfilename[0:-2].upper() )
#outfile.write('const unsigned char image_%s[%i][%i] = {\n' % (outfilename[0:-2], imheigh, imwidth/2 ) )
outfile.write('const unsigned char image_%s[%i] = {\n' % (outfilename[0:-2], imsize) )

for h in range(0,imheigh):
    for w in range(0,imwidth,2):

        color1 = 0
        color2 = 0
        sumcolor = 0

        color1 = getColor(pixels[w,h])
        color2 = getColor(pixels[w+1,h])

        # shift color1 and color2 to the right locations
        # [c1 c1 c1 c2 c2 c2 0 0]
        color1 = color1*0x20
        color2 = color2*0x4
        sumcolor = color1 + color2

        pcolor = ""
        # print the hex value to our output file
        if h < (imheigh - 1):
            pcolor = "0x%02x," % sumcolor
        else:
            if w < (imwidth - 2):
                pcolor = "0x%02x," % sumcolor
            else:
                pcolor = "0x%02x" % sumcolor

        outfile.write(pcolor)

    outfile.write('\n')

# add closing brace
outfile.write('};\n\n')
outfile.write('#endif')

outfile.close()
