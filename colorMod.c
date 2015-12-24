#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <gd.h>

#define JPG 1

#define ORIGINAL      1
#define RED_VS_BLUE   2
#define MINUS_GREEN   3
#define RED_AS_GRAY   4
#define GREEN_AS_GRAY 5
#define BLUE_AS_GRAY  6


int main(int argc, char *argv[]) {

    char *inFileName = NULL;
    char *outFileName = NULL;
    char *inFileExt = NULL;
    int fileType = JPG;
    FILE *inFile = NULL;
    FILE *outFile = NULL;
    int haveInFile = 0;
    int haveOutFile = 0;
    int haveAlgorithm = 0;
    int algorithm = ORIGINAL;
    char c;
    int cmndError = 0;
    gdImagePtr image;
    int x, y;
    int color;
    int redBlue;
    int red, green, blue;


    /*
     *  Check the command line.
     */
    while ((c = getopt(argc, argv, "i:o:a:")) != -1)  {
        switch (c)  {
        case 'i':
            haveInFile = 1;
            inFileName = optarg;
            break;
        case 'o':
            haveOutFile = 1;
            outFileName = optarg;
            break;
        case 'a':
            haveAlgorithm = 1;
            if      (!strcmp(optarg, "ORIGINAL"))      algorithm = ORIGINAL;
            else if (!strcmp(optarg, "RED_VS_BLUE"))   algorithm = RED_VS_BLUE;
            else if (!strcmp(optarg, "MINUS_GREEN"))   algorithm = MINUS_GREEN;
            else if (!strcmp(optarg, "RED_AS_GRAY"))   algorithm = RED_AS_GRAY;
            else if (!strcmp(optarg, "GREEN_AS_GRAY")) algorithm = GREEN_AS_GRAY;
            else if (!strcmp(optarg, "BLUE_AS_GRAY"))  algorithm = BLUE_AS_GRAY;
            else cmndError = 1;
            break;
        case '?':
            cmndError = 1;
            break;
        default:
            cmndError = 1;
            break;
        }
    }
    if (!haveInFile) {
        cmndError = 1;
    }
    if (cmndError) {
        fprintf(stderr, "USAGE: %s -i <input file> [-o <output file> (default = stdout)] -a <ORIGINAL (default) | RED_VS_BLUE | MINUS_GREEN | RED_AS_GRAY | GREEN_AS_GRAY | BLUE_AS_GRAY>\n", argv[0]);
        exit(-1);
    }
    if (outFile == NULL) {
        outFile = stdout;
    }


    /*
     * Check the inFileName.
     */
    inFileExt = strrchr(inFileName, '.');
    if (inFileExt == NULL) {
        fprintf(stderr, "Invalid input file, %s\n", inFileName);
        exit(-1);
    }
    if (!strcmp(inFileExt, ".jpg") || !strcmp(inFileExt, ".jpeg") || !strcmp(inFileExt, ".JPG") || !strcmp(inFileExt, ".JPEG")) {
        fileType = JPG;
    }
    else {
        fprintf(stderr, "Unsupported input file type, %s\n", inFileName);
        exit(-1);
    }


    /*
     *  Create an image from the input file.
     */
    if ((inFile = fopen(inFileName, "r")) == NULL) {
        fprintf(stderr, "Cannot open input file, %s\n", inFileName);
        exit(-1);
    }
    if ((image = gdImageCreateFromJpeg(inFile)) == NULL) {
        fprintf(stderr, "Cannot create image from input file, %s\n", inFileName);
        exit(-1);
    }
    fclose(inFile);


    /*
     * Manipulate the image.
     */
    if (algorithm == ORIGINAL) {
        /* Don't do anything. */
    }
    else if (algorithm == RED_VS_BLUE) {
        for (y = 0; y < gdImageSY(image); y++) {
            for (x = 0; x < gdImageSX(image); x++) {
                color = gdImageGetPixel(image, x, y);
                red   = (color & 0xFF0000) >> 16;
                green = (color & 0x00FF00) >> 8;
                blue  = (color & 0x0000FF) >> 0;
                if (red + blue == 0) {
                    color = 0;
                }
                else {
                    redBlue = (int)(floor(((double)(red - blue) / (double)(red + blue) * 255.0) + 0.5));
                    if (redBlue > 0) {
                        color = redBlue << 16;
                    }
                    else {
                        color = -redBlue;
                    }
                }
                gdImageSetPixel(image, x, y, color);
            }
        }
    }
    else if (algorithm == MINUS_GREEN) {
        for (y = 0; y < gdImageSY(image); y++) {
            for (x = 0; x < gdImageSX(image); x++) {
                color = gdImageGetPixel(image, x, y);
                color = color & 0xFF00FF;
                gdImageSetPixel(image, x, y, color);
            }
        }
    }
    else if (algorithm == RED_AS_GRAY) {
        for (y = 0; y < gdImageSY(image); y++) {
            for (x = 0; x < gdImageSX(image); x++) {
                color = gdImageGetPixel(image, x, y);
                red   = (color & 0xFF0000) >> 16;
                color = (red << 16) + (red << 8) + red;
                gdImageSetPixel(image, x, y, color);
            }
        }
    }
    else if (algorithm == GREEN_AS_GRAY) {
        for (y = 0; y < gdImageSY(image); y++) {
            for (x = 0; x < gdImageSX(image); x++) {
                color = gdImageGetPixel(image, x, y);
                green = (color & 0x00FF00) >> 8;
                color = (green << 16) + (green << 8) + green;
                gdImageSetPixel(image, x, y, color);
            }
        }
    }
    else if (algorithm == BLUE_AS_GRAY) {
        for (y = 0; y < gdImageSY(image); y++) {
            for (x = 0; x < gdImageSX(image); x++) {
                color = gdImageGetPixel(image, x, y);
                blue  = (color & 0x0000FF) >> 0;
                color = (blue << 16) + (blue << 8) + blue;
                gdImageSetPixel(image, x, y, color);
            }
        }
    }


    /*
     *  Create the output file from the image.
     */
    if (haveOutFile) {
        if ((outFile = fopen(outFileName, "w")) == NULL) {
            fprintf(stderr, "Cannot open output file, %s\n", outFileName);
            exit(-1);
        }
    }
    else {
        outFile = stdout;
    }
    if (fileType == JPG) {
        gdImageJpeg(image, outFile, -1);
    }
    if (haveOutFile) {
        fclose(outFile);
    }


    /*
     *  Cleanup.
     */
    gdImageDestroy(image);


    return(0);
}
