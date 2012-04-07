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


# SUDO CODE:

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
outfilename = outfilename[0:-3] + 'hex'
outfile = open(outfilename,'w')

# get array of pixel values from the image
pixels = theimage.load()
imwidth = theimage.size[0]
imheigh = theimage.size[1]

for h in range(0,imheigh):
    for w in range(0,imwidth):

        red = pixels[w,h][0]
        grn = pixels[w,h][1]
        blu = pixels[w,h][2]
        color = 0

        # test to make sure the pixel is valid
        if not (red == 255 or red == 0) or not (grn == 255 or grn == 0) or not (blu == 255 or blu == 0):
            if (red != 128 and blu != 64 and grn != 0):
                print "There is an invalid pixel value in this picture r = %3i g = %3i b = %3i" % (red, grn, blu)
                exit()

        # extract the color values
        if red == 255:
            color = color + 1
        if grn == 255:
            color = color + 2
        if blu == 255:
            color = color + 4
        if red == 128 and blu == 64 and grn == 0:
            color = 3

        # print the hex value to our output file
        pcolor = "%02x" % color
        outfile.write(pcolor)
    outfile.write('\n')

outfile.close()
