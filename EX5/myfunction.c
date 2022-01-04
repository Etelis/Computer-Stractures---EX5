#pragma GCC optimize ("O1,O2,O3,Ofast,unroll-loops,fast-math")
#include <stdbool.h>

#define MN m*n
#define MIN(X) ((X) < 255 ? X : 255)
#define MAX(X) ((X) > 0 ? X : 0)
#define MULT_MINUS_1(X) ((X) * (-1))
#define MULT_10(X) (((X) << 3) + ((X) << 1))

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

typedef struct {
    int_fast16_t red;
    int_fast16_t green;
    int_fast16_t blue;
} pixel_sum;

/**
 * A full code explanation will be given here:
 *      Algorithm:
 *          The algorithm used is quite simple, let i be the current row of the iteration process, if colum 0 + colum 1 are summed in the outer - loop the inner loop could only sum the third row to achieve the sum of the cube (3x3).
 *          after doing so moving to j++, actually colum0 = colum 1, colum 1 = colum 2 by understanding so we could decrease number of calculations done dramatically.
 *      I have managed to distinguish between different scenarios:
 *          1. KernelScale = 9 no filter: blur: meaning a simple average should be done by using above algorithm.
 *          2. KernelScale = 1 no filter:
 *          3. KernelScale = 7 filter: blur : meaning all items should be summed, max + min should be removed and sum should be done over 7 items.
 *          4. KernelScale = 1 filter: same as 2.
 */

void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {
//    /*
//    * [1, 1, 1]
//    * [1, 1, 1]
//    * [1, 1, 1]
//    */
//    int blurKernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
//
//    /*
//    * [-1, -1, -1]
//    * [-1, 9, -1]
//    * [-1, -1, -1]
//    */
//    int sharpKernel[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};

    // iteration on matrix in an optimized way.
    register int_fast16_t endOfIteration = m - 1;
    register int_fast16_t i, j;
    int_fast16_t size = MN * sizeof(pixel);

    // if condition 1 is filled.
    if (flag == '1') {

        // why copy when we could only apply by pointers.
        register pixel* pixelsImg = (pixel *) image->data;
        register pixel* backupOrg = malloc(size);

        // backupOrg should be copied to keep avoid changes.
        memcpy(backupOrg, pixelsImg, size);

        // let's implement the smooth function:
        // avoiding unnecessary calculation, using loop unrolling.
        register int_fast16_t  currentColum, row = 0;
        register pixel *p00, *p01, *p02, *p10, *p11, *p12, *p20, *p21, *p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {};

        // calculate two colums in every row and join with the third.
        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;

            p00 = &backupOrg[currentColum];
            p01 = p00 + 1;
            p10 = p00 + m;
            p11 = p10 + 1;
            p20 = p10 + m;
            p21 = p20 + 1;

            col_0.blue = p00->blue + p10->blue + p20->blue;
            col_0.green = p00->green + p10->green + p20->green;
            col_0.red = p00->red + p10->red + p20->red;

            col_1.blue = p01->blue + p11->blue + p21->blue;
            col_1.green = p01->green + p11->green + p21->green;
            col_1.red = p01->red + p11->red + p21->red;


            for (j = 4; j <= endOfIteration; j+=2) {
                p00 = &backupOrg[currentColum];

                // third row calculation.
                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                pixelsImg[currentColum + m + 1].blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                pixelsImg[currentColum + m + 1].red = (col_0.red + col_1.red + col_2.red) / 9;
                pixelsImg[currentColum + m + 1].green = (col_0.green + col_1.green + col_2.green) / 9;

                col_0 = col_1;
                col_1 = col_2;
                currentColum++;

                // loop undrolling moving one index to the right.
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                pixelsImg[currentColum + m + 1].blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                pixelsImg[currentColum + m + 1].red = (col_0.red + col_1.red + col_2.red) / 9;
                pixelsImg[currentColum + m + 1].green = (col_0.green + col_1.green + col_2.green) / 9;

                col_0 = col_1;
                col_1 = col_2;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                pixelsImg[currentColum + m + 1].blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                pixelsImg[currentColum + m + 1].red = (col_0.red + col_1.red + col_2.red) / 9;
                pixelsImg[currentColum + m + 1].green = (col_0.green + col_1.green + col_2.green) / 9;
            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);

        //backup should be updated to current state before sharping.
        memcpy(backupOrg, pixelsImg, size);

        // sharpen the resulting image

        // avoiding unnecessary calculation, using loop unrolling.
        pixel *mid;
        row = 0;
        pixel_sum sum_clr;


        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;

            p00 = &backupOrg[currentColum];
            p01 = p00 + 1;
            p10 = p00 + m;
            mid = p11 = p10 + 1;
            p20 = p10 + m;
            p21 = p20 + 1;


            col_0.blue = p00->blue + p10->blue + p20->blue;
            col_0.green = p00->green + p10->green + p20->green;
            col_0.red = p00->red + p10->red + p20->red;

            col_1.blue = p01->blue + p11->blue + p21->blue;
            col_1.green = p01->green + p11->green + p21->green;
            col_1.red = p01->red + p11->red + p21->red;

            for (j = 4; j <= endOfIteration; j+=2) {
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;

                col_0 = col_1;
                col_1 = col_2;
                mid = p12;

                currentColum++;

                // loop undrollving moving one index to the right.

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;

                col_0 = col_1;
                col_1 = col_2;
                mid = p12;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;

            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
    }

    else { // flag != '1' - meaning an intensity will be applied

        register pixel* pixelsImg = (pixel *) image->data;
        register pixel* backupOrg = malloc(size);
        memcpy(backupOrg, pixelsImg, size);

        // let's implement the smooth function:
        // avoiding unnessery calculation, using loop unrolling.
        register int_fast16_t currentColum, row = 0, saved_intensity, saved_intensity_2, saved_intensity_3, saved_intensity_4, saved_intensity_5, saved_intensity_6, saved_intensity_7, saved_intensity_8, saved_intensity_9;
        register pixel *p00, *p01, *p02, *p10, *p11, *p12, *p20, *p21, *p22, *min_index, *max_index;;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {}, sum_clr;

        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;

            p00 = &backupOrg[currentColum];
            p01 = p00 + 1;
            p10 = p00 + m;
            p11 = p10 + 1;
            p20 = p10 + m;
            p21 = p20 + 1;

            col_0.blue = p00->blue + p10->blue + p20->blue;
            col_0.green = p00->green + p10->green + p20->green;
            col_0.red = p00->red + p10->red + p20->red;

            col_1.blue = p01->blue + p11->blue + p21->blue;
            col_1.green = p01->green + p11->green + p21->green;
            col_1.red = p01->red + p11->red + p21->red;

            saved_intensity = (p00)->red + (p00)->green + (p00)->blue;
            saved_intensity_2 = (p01)->red + (p01)->green + (p01)->blue;
            saved_intensity_4 = (p10)->red + (p10)->green + (p10)->blue;
            saved_intensity_5 = (p11)->red + (p11)->green + (p11)->blue;
            saved_intensity_7 = (p20)->red + (p20)->green + (p20)->blue;
            saved_intensity_8 = (p21)->red + (p21)->green + (p21)->blue;


            for (j = 4; j <= endOfIteration; ++j) {
                register int_fast16_t min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
                register int_fast16_t max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                saved_intensity_3 = (p02)->red + (p02)->green + (p02)->blue;
                saved_intensity_6 = (p12)->red + (p12)->green + (p12)->blue;
                saved_intensity_9 = (p22)->red + (p22)->green + (p22)->blue;

                if (saved_intensity <= min_intensity){
                    min_index = p00;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00;
                    max_intensity = saved_intensity;
                }

                if (saved_intensity_2 <= min_intensity){
                    min_index = p00 + 1;
                    min_intensity = saved_intensity_2;
                }

                if (saved_intensity_2 > max_intensity){
                    max_index = p00 + 1;
                    max_intensity = saved_intensity_2;
                }

                if (saved_intensity_3 <= min_intensity){
                    min_index = p02;
                    min_intensity = saved_intensity_3;
                }

                if (saved_intensity_3 > max_intensity){
                    max_index = p02;
                    max_intensity = saved_intensity_3;
                }

                if (saved_intensity_4 <= min_intensity){
                    min_index = p00 + m;
                    min_intensity = saved_intensity_4;
                }

                if (saved_intensity_4 > max_intensity){
                    max_index = p00 + m;
                    max_intensity = saved_intensity_4;
                }

                if (saved_intensity_5 <= min_intensity){
                    min_index = p00 + m + 1;
                    min_intensity = saved_intensity_5;
                }

                if (saved_intensity_5 > max_intensity){
                    max_index = p00 + m + 1;
                    max_intensity = saved_intensity_5;
                }

                if (saved_intensity_6 <= min_intensity){
                    min_index = p12;
                    min_intensity = saved_intensity_6;
                }

                if (saved_intensity_6 > max_intensity){
                    max_index = p12;
                    max_intensity = saved_intensity_6;
                }

                if (saved_intensity_7 <= min_intensity){
                    min_index = p00 + m + m;
                    min_intensity = saved_intensity_7;
                }

                if (saved_intensity_7 > max_intensity){
                    max_index = p00 + m + m;
                    max_intensity = saved_intensity_7;
                }

                if (saved_intensity_8 <= min_intensity){
                    min_index = p00 + m + m + 1;
                    min_intensity = saved_intensity_8;
                }

                if (saved_intensity_8 > max_intensity){
                    max_index = p00 + m + m + 1;
                    max_intensity = saved_intensity_8;
                }

                if (saved_intensity_9 <= min_intensity){
                    min_index = p22;
                    min_intensity = saved_intensity_9;
                }

                if (saved_intensity_9 > max_intensity){
                    max_index = p22;
                    max_intensity = saved_intensity_9;
                }
                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (col_0.blue + col_1.blue + col_2.blue) + (int_fast16_t)MULT_MINUS_1((min_index->blue + max_index->blue));
                sum_clr.red = (col_0.red + col_1.red + col_2.red) + (int_fast16_t)MULT_MINUS_1(min_index->red + max_index->red);
                sum_clr.green = (col_0.green + col_1.green + col_2.green) + (int_fast16_t)MULT_MINUS_1(min_index->green + max_index->green);

                pixelsImg[currentColum + m + 1].blue = (unsigned char) (sum_clr.blue / 7);
                pixelsImg[currentColum + m + 1].red = (unsigned char) (sum_clr.red / 7);
                pixelsImg[currentColum + m + 1].green = (unsigned char) (sum_clr.green / 7);

                currentColum++;
                col_0 = col_1;
                col_1 = col_2;

                saved_intensity = saved_intensity_2;
                saved_intensity_2 = saved_intensity_3;
                saved_intensity_4 = saved_intensity_5;
                saved_intensity_5 = saved_intensity_6;
                saved_intensity_7 = saved_intensity_8;
                saved_intensity_8 = saved_intensity_9;
            }
            row+= m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

        memcpy(backupOrg, pixelsImg, size);

        // sharpen the resulting image

        // avoiding unnessery calculation, using loop unrolling.
        pixel *mid;
        row = 0;

        for (i = 1; i < endOfIteration; ++i) {
            currentColum = row;

            p00 = &backupOrg[currentColum];
            p01 = p00 + 1;
            p10 = p00 + m;
            mid = p11 = p10 + 1;
            p20 = p10 + m;
            p21 = p20 + 1;


            col_0.blue = p00->blue + p10->blue + p20->blue;
            col_0.green = p00->green + p10->green + p20->green;
            col_0.red = p00->red + p10->red + p20->red;

            col_1.blue = p01->blue + p11->blue + p21->blue;
            col_1.green = p01->green + p11->green + p21->green;
            col_1.red = p01->red + p11->red + p21->red;

            for (j = 4; j <= endOfIteration; j+=2) {
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;

                col_0 = col_1;
                col_1 = col_2;
                mid = p12;

                currentColum++;
                // mid = backupOrg[currentColumPlus1Row + 1];

                // loop undrolling moving one index to the right.

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;

                col_0 = col_1;
                col_1 = col_2;
                mid = p12;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p02 + m;
                p22 = p12 + m;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (MULT_10(mid->blue)) + MULT_MINUS_1(col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (unsigned char) MIN(MAX(sum_clr.blue));

                sum_clr.green = (MULT_10(mid->green)) + MULT_MINUS_1(col_0.green + col_1.green + col_2.green);
                sum_clr.green = (unsigned char) MIN(MAX(sum_clr.green));

                sum_clr.red = (MULT_10(mid->red)) + MULT_MINUS_1(col_0.red + col_1.red + col_2.red);
                sum_clr.red = (unsigned char) MIN(MAX(sum_clr.red));

                pixelsImg[currentColum + m + 1].red = (unsigned char) sum_clr.red;
                pixelsImg[currentColum + m + 1].green = (unsigned char) sum_clr.green;
                pixelsImg[currentColum + m + 1].blue = (unsigned char) sum_clr.blue;
            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
    //free(backupOrg);
    //free(pixelsImg);
}

