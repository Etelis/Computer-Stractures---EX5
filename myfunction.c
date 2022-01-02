#include <stdbool.h>
#include "stdlib.h"
#define MN m*n
#define SIZE MN*sizeof(pixel)

typedef struct {
    int intensity_threshold;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel_1;

typedef struct {
    int red;
    int green;
    int blue;
    // int num;
} pixel_sum;

void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {

    /*
    * [1, 1, 1]
    * [1, 1, 1]
    * [1, 1, 1]
    */
    int blurKernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

    /*
    * [-1, -1, -1]
    * [-1, 9, -1]
    * [-1, -1, -1]
    */
    int sharpKernel[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};

    // iteration on matrix in an optimized way.
    register int endOfMatrix = MN - 1, endOfIteration = m - 1;
    register int i, j;

    if (flag == '1') {
        j = 0;
        pixel_1* pixelsImg = malloc(SIZE);
        pixel_1* backupOrg = malloc(SIZE);

        // iteration on matrix in an optimized way.

        for (i = 0; i <= endOfMatrix; i+=2 ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];

            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;

            pixelsImg[i+1].red = image->data[j];
            pixelsImg[i+1].green = image->data[j + 1];
            pixelsImg[i+1].blue = image->data[j + 2];

            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i+1] = pixelsImg[i+1];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }

        if (i != endOfMatrix){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];

            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }


        // let's implement the smooth function:
        // avoiding unnessery calculation, using loop unrolling.
        register int  currentColum, currentColumPlus1Row, row = 0;
        register int currentColumPlus2Row;
        pixel_1 p00, p01, p02, p10, p11, p12, p20, p21, p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {};
        register pixel_1 sum;
        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;
            currentColumPlus1Row = currentColum + m;
            currentColumPlus2Row = currentColumPlus1Row + m;

            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            col_0.blue = p00.blue + p10.blue + p20.blue;
            col_0.green = p00.green + p10.green + p20.green;
            col_0.red = p00.red + p10.red + p20.red;

            col_1.blue = p01.blue + p11.blue + p21.blue;
            col_1.green = p01.green + p11.green + p21.green;
            col_1.red = p01.red + p11.red + p21.red;

            for (j = 4; j <= endOfIteration; j+=2) {
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColumPlus1Row + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;

                // loop undrolling moving one index to the right.
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;
                pixelsImg[currentColumPlus1Row + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;

                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColumPlus1Row + 1] = sum;
            }
            row+=m;
        }

        image->data = (char *) pixelsImg;
        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);


        // sharpen the resulting image

        // avoiding unnessery calculation, using loop unrolling.
        pixel_1 mid;
        row = 0;
        pixel_sum sum_clr;

        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;
            currentColumPlus1Row = currentColum + m;
            currentColumPlus2Row = currentColumPlus1Row + m;

            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            mid = p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            col_0.blue = p00.blue + p10.blue + p20.blue;
            col_0.green = p00.green + p10.green + p20.green;
            col_0.red = p00.red + p10.red + p20.red;

            col_1.blue = p01.blue + p11.blue + p21.blue;
            col_1.green = p01.green + p11.green + p21.green;
            col_1.red = p01.red + p11.red + p21.red;

            for (j = 4; j <= endOfIteration; j+=2) {
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                mid = backupOrg[currentColumPlus1Row + 1];

                // loop undrolling moving one index to the right.


                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                mid = backupOrg[currentColumPlus1Row + 1];
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
    }

    else { // flag != '1' - meaning an intensity will be applied
        pixel* pixelsImg = malloc(SIZE);
        pixel* backupOrg = malloc(SIZE);
        int min_index, max_index;
        int saved_intensity;
        j = 0;
        for (i = 0; i <= endOfMatrix; ++i ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];
            // if filter is true - meaning we will be needing that intensity later on.
            pixelsImg[i].intensity_threshold = pixelsImg[i].red + pixelsImg[i].green + pixelsImg[i].blue;
            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }

        // let's implement the smooth function:
        // avoiding unnessery calculation, using loop unrolling.
        register int  currentColum, currentColumPlus1Row, row = 0, colum;
        register int currentColumPlus2Row;
        pixel p00, p01, p02, p10, p11, p12, p20, p21, p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {}, sum_clr;
        register pixel sum;
        for (i = 1; i < endOfIteration; ++i) {
            colum = 0;
            currentColum = row;
            currentColumPlus1Row = currentColum + m;
            currentColumPlus2Row = currentColumPlus1Row + m;

            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            col_0.blue = p00.blue + p10.blue + p20.blue;
            col_0.green = p00.green + p10.green + p20.green;
            col_0.red = p00.red + p10.red + p20.red;

            col_1.blue = p01.blue + p11.blue + p21.blue;
            col_1.green = p01.green + p11.green + p21.green;
            col_1.red = p01.red + p11.red + p21.red;

            for (j = 1; j < endOfIteration; ++j) {
                int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
                int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0

                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = (int)(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.green = (int)(col_0.green + col_1.green + col_2.green);
                sum_clr.red = (int)(col_0.red + col_1.red + col_2.red);

                saved_intensity = backupOrg[currentColum].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColum;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColum;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColum + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_intensity = currentColum + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColum + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColum + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColum + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColum + 2;
                    max_intensity = saved_intensity;
                }

                saved_intensity = backupOrg[currentColumPlus1Row].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus1Row;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus1Row;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus1Row + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus1Row + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus1Row + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus1Row + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus1Row + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus1Row + 2;
                    max_intensity = saved_intensity;
                }

                saved_intensity = backupOrg[currentColumPlus2Row].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus2Row;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus2Row;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus2Row + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus2Row + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus2Row + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus2Row + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_index = currentColumPlus2Row + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = currentColumPlus2Row + 2;
                    max_intensity = saved_intensity;
                }
                pixel max_value = backupOrg[max_index], min_value = backupOrg[min_index];
                sum_clr.blue -= (int)(min_value.blue + max_value.blue);
                sum_clr.red -= (int)(min_value.red + max_value.red);
                sum_clr.green -= (int)(min_value.green + max_value.green);

                sum.blue = (unsigned  char) (sum_clr.blue / 7);
                sum.green = (unsigned  char) (sum_clr.green / 7);
                sum.red = (unsigned  char) (sum_clr.red / 7);
                pixelsImg[currentColumPlus1Row + 1] = sum;
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                col_0 = col_1;
                col_1 = col_2;
                colum++;
            }
            row+= m;
        }


        j = 0;
        // replacement for the function pixelsToChars, set image-> accordingly.
        for (i = 0; i <= endOfMatrix; ++i ){
            image->data[j] = pixelsImg[i].red;
            image->data[j + 1] = pixelsImg[i].green;
            image->data[j + 2] = pixelsImg[i].blue;
            j += 3;
        }

        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

        // SHARPEN
        // avoiding unnessery calculation, using loop unrolling.
        pixel mid;
        row = 0;

        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;
            currentColumPlus1Row = currentColum + m;
            currentColumPlus2Row = currentColumPlus1Row + m;

            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            mid = p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            col_0.blue = p00.blue + p10.blue + p20.blue;
            col_0.green = p00.green + p10.green + p20.green;
            col_0.red = p00.red + p10.red + p20.red;

            col_1.blue = p01.blue + p11.blue + p21.blue;
            col_1.green = p01.green + p11.green + p21.green;
            col_1.red = p01.red + p11.red + p21.red;

            for (j = 4; j <= endOfIteration; j+=2) {
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                mid = backupOrg[currentColumPlus1Row + 1];

                // loop undrolling moving one index to the right.


                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                mid = backupOrg[currentColumPlus1Row + 1];
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p02 = backupOrg[currentColum + 2];
                p12 = backupOrg[currentColumPlus1Row + 2];
                p22 = backupOrg[currentColumPlus2Row + 2];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum_clr.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = sum_clr.blue;

                sum_clr.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = sum_clr.green;

                sum_clr.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = sum_clr.red;

                pixelsImg[currentColumPlus1Row + 1] = sum;
            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);

        j = 0;
        // replacement for the function pixelsToChars, set image-> accordingly.
        for (i = 0; i <= endOfMatrix; ++i ){
            image->data[j] = pixelsImg[i].red;
            image->data[j + 1] = pixelsImg[i].green;
            image->data[j + 2] = pixelsImg[i].blue;
            j += 3;
        }

    }
    //free(backupOrg);
    //free(pixelsImg);
}
