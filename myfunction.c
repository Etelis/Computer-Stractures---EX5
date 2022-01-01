#include <stdbool.h>
#include "stdlib.h"
#define MN m*n
#define SIZE MN*sizeof(pixel)

typedef struct {
    short int intensity_threshold;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

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
    pixel* pixelsImg = malloc(SIZE);
    pixel* backupOrg = malloc(SIZE);
    printf("malloc finished");

    // iteration on matrix in an optimized way.
    register int endOfMatrix = MN - 1, i = 0, j = 0, endOfIteration = m - 1;

    if (flag == '1') {
        // iteration on matrix in an optimized way.
        for (; i <= endOfMatrix; ++i ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];
            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }
        printf("\npixelsImg initiallized\n");

        // let's implement the smooth function:
        // avoiding unnessery calculation, using loop unrolling.
        register int  currentColum, currentColumPlus1Row;
        register int currentColumPlus2Row, third, third_2, third_3;
        pixel p00, p01, p02, p10, p11, p12, p20, p21, p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {};
        register pixel sum;
        i = 1, j = 4;
        for (; i < endOfIteration; ++i) {
            currentColum = (i-1);
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

            third = currentColum + 2;
            third_2 = third + m;
            third_3 = third_2 + m;

            for (; j <= endOfIteration; j+=2) {
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

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

                third++, third_2++, third_3++;
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

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

                third++, third_2++, third_3++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColumPlus1Row + 1] = sum;
            }
        }
        printf("finished loop\n");
        i = 0, j = 0;
        // replacement for the function pixelsToChars, set image-> accordingly.
        for (; i <= endOfMatrix; ++i ){
            image->data[j] = pixelsImg[i].red;
            image->data[j + 1] = pixelsImg[i].green;
            image->data[j + 2] = pixelsImg[i].blue;
            j += 3;
        }
        printf("image was converted\n");





        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);
        printf("writeBPM was finished");




        // sharpen the resulting image

        // iteration on matrix in an optimized way.
        for (; i <= endOfMatrix; ++i ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];
            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }
        printf("pixelsImg was initiallized again\n");

        // avoiding unnessery calculation, using loop unrolling.
        currentColum = 0, currentColumPlus1Row = currentColum + m;
        currentColumPlus2Row = currentColumPlus1Row + m;
        i = 1, j = 4;
        pixel mid;

        for (; i < endOfIteration; ++i) {
            currentColum = (i-1);
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

            third = currentColum + 2;
            third_2 = third + m;
            third_3 = third_2 + m;

            for (; j < endOfIteration; j+=2) {
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;

                // loop undrolling moving one index to the right.
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;

                third++, third_2++, third_3++;

                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;

                // loop undrolling moving one index to the right.
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;

                third++, third_2++, third_3++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration - 1){
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row + 1] = sum;
            }
        }
        printf("loops finished\n");

        i = 0, j = 0;
        // replacement for the function pixelsToChars, set image-> accordingly.
        for (; i <= endOfMatrix; ++i ){
            image->data[j] = pixelsImg[i].red;
            image->data[j + 1] = pixelsImg[i].green;
            image->data[j + 2] = pixelsImg[i].blue;
            j += 3;
        }

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
        printf("writeBMP finished\n");
    }

    else { // flag != '1' - meaning an intensity will be applied
        int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
        int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
        int min_row, min_col, max_row, max_col;
        int saved_intensity;

        i = 0, j = 0;
        for (; i <= endOfMatrix; ++i ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];
            // if filter is true - meaning we will be needing that intensity later on.
            pixelsImg->intensity_threshold += pixelsImg[i].red + pixelsImg[i].green + pixelsImg[i].blue;
            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }

        // let's implement the smooth function:
        // avoiding unnessery calculation, using loop unrolling.
        int  currentColum = 0, currentColumPlus1Row = currentColum + m;
        int currentColumPlus2Row = currentColumPlus1Row + m, third, third_2, third_3;
        int colum = 0,row = 0;
        pixel p00, p01, p02, p10, p11, p12, p20, p21, p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {};
        register pixel sum;
        i = 1, j = 1;
        for (; i < endOfIteration; ++i) {
            colum = 0;
            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            third = currentColum + 2;
            third_2 = third + m;
            third_3 = third_2 + m;

            for (; j < endOfIteration; ++j) {
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_0.blue = p00.blue + p10.blue + p20.blue;
                col_0.green = p00.green + p10.green + p20.green;
                col_0.red = p00.red + p10.red + p20.red;

                col_1.blue = p01.blue + p11.blue + p21.blue;
                col_1.green = p01.green + p11.green + p21.green;
                col_1.red = p01.red + p11.red + p21.red;

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue);
                sum.green = (col_0.green + col_1.green + col_2.green);
                sum.red = (col_0.red + col_1.red + col_2.red);

                saved_intensity = backupOrg[currentColum].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row;
                    min_col = colum;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row;
                    max_col = colum;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColum + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row;
                    min_col = colum + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row;
                    max_col = colum + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColum + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row;
                    min_col = colum + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row;
                    max_col = colum + 2;
                    max_intensity = saved_intensity;
                }

                saved_intensity = backupOrg[currentColumPlus1Row].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 1;
                    min_col = colum;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 1;
                    max_col = colum;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus1Row + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 1;
                    min_col = colum + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 1;
                    max_col = colum + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus1Row + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 1;
                    min_col = colum + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 1;
                    max_col = colum + 2;
                    max_intensity = saved_intensity;
                }

                saved_intensity = backupOrg[currentColumPlus2Row].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 2;
                    min_col = colum;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 2;
                    max_col = colum;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus2Row + 1].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 2;
                    min_col = colum + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 2;
                    max_col = colum  + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = backupOrg[currentColumPlus2Row + 2].intensity_threshold;
                if (saved_intensity <= min_intensity){
                    min_row = row + 2;
                    min_col = colum + 2;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_row = row + 2;
                    max_col = colum + 2;
                    max_intensity = saved_intensity;
                }
                pixel max_value = backupOrg[max_row + max_col], min_value = backupOrg[min_row + min_col];
                sum.blue -= (min_value.blue + max_value.blue);
                sum.red -= (min_value.red + max_value.red);
                sum.green -= (min_value.green + max_value.green);

                sum.blue /= 7;
                sum.green /=7;
                sum.red /=7;
                pixelsImg[currentColumPlus1Row] = sum;

                colum++;
            }
            row+= m;
        }

        // set the image's data to be dst
        image->data = (char *)pixelsImg;
        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

            // SHARPEN

        // iteration on matrix in an optimized way.
        for (; i <= endOfMatrix; ++i ){
            pixelsImg[i].red = image->data[j];
            pixelsImg[i].green = image->data[j + 1];
            pixelsImg[i].blue = image->data[j + 2];
            // instead of copyPixels, let's copy them straight away :).
            backupOrg[i] = pixelsImg[i];
            // strength reduction - addon instead of multiplication.
            j += 3;
        }

        // avoiding unnessery calculation, using loop unrolling.
        currentColum = 0, currentColumPlus1Row = currentColum + m;
        currentColumPlus2Row = currentColumPlus1Row + m;
        i = 1, j = 4;
        pixel mid;

        for (; i < endOfIteration; ++i) {
            p00 = backupOrg[currentColum];
            p01 = backupOrg[currentColum + 1];
            p10 = backupOrg[currentColumPlus1Row];
            mid = p11 = backupOrg[currentColumPlus1Row + 1];
            p20 = backupOrg[currentColumPlus2Row];
            p21 = backupOrg[currentColumPlus2Row + 1];

            third = currentColum + 2;
            third_2 = third + m;
            third_3 = third_2 + m;

            for (; j < endOfIteration; j+=2) {
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_0.blue = p00.blue + p10.blue + p20.blue;
                col_0.green = p00.green + p10.green + p20.green;
                col_0.red = p00.red + p10.red + p20.red;

                col_1.blue = p01.blue + p11.blue + p21.blue;
                col_1.green = p01.green + p11.green + p21.green;
                col_1.red = p01.red + p11.red + p21.red;

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row] = sum;

                // loop undrolling moving one index to the left.
                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                p00 = p01;
                p01 = p12;
                p10 = p11;
                p11 = p12;
                p20 = p21;
                p21 = p22;

                third++, third_2++, third_3++;
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_0.blue = p00.blue + p10.blue + p20.blue;
                col_0.green = p00.green + p10.green + p20.green;
                col_0.red = p00.red + p10.red + p20.red;

                col_1.blue = p01.blue + p11.blue + p21.blue;
                col_1.green = p01.green + p11.green + p21.green;
                col_1.red = p01.red + p11.red + p21.red;

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row] = sum;

                currentColum++, currentColumPlus1Row++, currentColumPlus2Row++;
                p00 = p01;
                p01 = p12;
                p10 = p11;
                p11 = p12;
                p20 = p21;
                p21 = p22;
                third++, third_2++, third_3++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration - 1){
                p02 = backupOrg[third];
                p12 = backupOrg[third_2];
                p22 = backupOrg[third_3];

                col_0.blue = p00.blue + p10.blue + p20.blue;
                col_0.green = p00.green + p10.green + p20.green;
                col_0.red = p00.red + p10.red + p20.red;

                col_1.blue = p01.blue + p11.blue + p21.blue;
                col_1.green = p01.green + p11.green + p21.green;
                col_1.red = p01.red + p11.red + p21.red;

                col_2.blue = p02.blue + p12.blue + p22.blue;
                col_2.green = p02.green + p12.green + p22.green;
                col_2.red = p02.red + p12.red + p22.red;

                sum.blue = ((mid.blue << 3) + (mid.blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum.blue = sum.blue > 0 ? sum.blue : 0;
                sum.blue = sum.blue < 255 ? sum.blue : 255;

                sum.green = ((mid.green << 3) + (mid.green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum.green = sum.green > 0 ? sum.green : 0;
                sum.green = sum.green < 255 ? sum.green : 255;

                sum.red = ((mid.red << 3) + (mid.red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum.red = sum.red > 0 ? sum.red : 0;
                sum.red = sum.red < 255 ? sum.red : 255;

                pixelsImg[currentColumPlus1Row] = sum;
            }
            currentColum+=m, currentColumPlus1Row+=m, currentColumPlus2Row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
    //free(backupOrg);
    //free(pixelsImg);
}

