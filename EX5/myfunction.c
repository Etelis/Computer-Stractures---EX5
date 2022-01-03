#include <stdbool.h>

#define MN m*n
#define SIZE MN*sizeof(pixel)
#define SIZE_1 MN*sizeof(pixel)

typedef struct {
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
    register int endOfIteration = m - 1;
    register int i, j;
    int size = MN * sizeof(pixel);

    // if condition 1 is filled.
    if (flag == '1') {

        // why copy when we could only apply by pointers.
        register pixel* pixelsImg = (pixel *) image->data;
        register pixel* backupOrg = malloc(size);
        // backupOrg should be copied to keep avoid changes.
        memcpy(backupOrg, pixelsImg, size);

        // let's implement the smooth function:
        // avoiding unnecessary calculation, using loop unrolling.
        register int  currentColum, row = 0;
        register pixel *p00, *p01, *p02, *p10, *p11, *p12, *p20, *p21, *p22;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {};
        register pixel sum;

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
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColum + m + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;
                currentColum++;

                // loop undrolling moving one index to the right.
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColum + m + 1] = sum;

                col_0 = col_1;
                col_1 = col_2;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum.blue = (col_0.blue + col_1.blue + col_2.blue) / 9;
                sum.green = (col_0.green + col_1.green + col_2.green) / 9;
                sum.red = (col_0.red + col_1.red + col_2.red) / 9;

                pixelsImg[currentColum + m + 1] = sum;
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
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                // mid*10 - sum
                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                // mid*10 - sum
                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                // mid*10 - sum
                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                mid = p12;

                currentColum++;

                // loop undrollving moving one index to the right.

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                mid = p12;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
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
        register int  currentColum, row = 0, saved_intensity;
        register pixel *p00, *p01, *p02, *p10, *p11, *p12, *p20, *p21, *p22, *min_index, *max_index;;
        register pixel_sum col_0 = {}, col_1 = {}, col_2 = {}, sum_clr;
        register pixel sum;


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

            for (j = 4; j <= endOfIteration; ++j) {
                register int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
                register int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.green = (col_0.green + col_1.green + col_2.green);
                sum_clr.red = (col_0.red + col_1.red + col_2.red);

                saved_intensity = (p00)->red + (p00)->green + (p00)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p00;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00;
                    max_intensity = saved_intensity;
                }

                saved_intensity = (p00 + 1)->red + (p00 + 1)->green + (p00 + 1)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p00 + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00 + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = (p02)->red + (p02)->green + (p02)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p02;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p02;
                    max_intensity = saved_intensity;
                }

                saved_intensity = (p00 + m)->red + (p00 + m)->green + (p00 + m)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p00 + m;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00 + m;
                    max_intensity = saved_intensity;
                }
                saved_intensity = (p00 + m + 1)->red + (p00 + m + 1)->green + (p00 + m + 1)->blue ;
                if (saved_intensity <= min_intensity){
                    min_index = p00 + m + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00 + m + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = (p12)->red + (p12)->green + (p12)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p12;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p12;
                    max_intensity = saved_intensity;
                }

                saved_intensity = (p00 + m + m)->red + (p00 + m + m)->green + (p00 + m + m)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p00 + m + m;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00 + m + m;
                    max_intensity = saved_intensity;
                }
                saved_intensity = (p00 + m + m + 1)->red + (p00 + m + m + 1)->green + (p00 + m + m + 1)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p00 + m + m + 1;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p00 + m + m + 1;
                    max_intensity = saved_intensity;
                }
                saved_intensity = (p22)->red + (p22)->green + (p22)->blue;
                if (saved_intensity <= min_intensity){
                    min_index = p22;
                    min_intensity = saved_intensity;
                }

                if (saved_intensity > max_intensity){
                    max_index = p22;
                    max_intensity = saved_intensity;
                }

                sum_clr.blue -= (int)(min_index->blue + max_index->blue);
                sum_clr.red -= (int)(min_index->red + max_index->red);
                sum_clr.green -= (int)(min_index->green + max_index->green);

                sum.blue = (unsigned char) (sum_clr.blue / 7);
                sum.green = (unsigned char) (sum_clr.green / 7);
                sum.red = (unsigned char) (sum_clr.red / 7);
                pixelsImg[currentColum + m + 1] = sum;

                currentColum++;
                col_0 = col_1;
                col_1 = col_2;
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
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                mid = p12;

                currentColum++;
                // mid = backupOrg[currentColumPlus1Row + 1];

                // loop undrolling moving one index to the right.

                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
                col_0 = col_1;
                col_1 = col_2;
                mid = p12;
                currentColum++;
            }

            // time to handle the reminder - %2 == 1
            if (currentColum != endOfIteration){
                p00 = &backupOrg[currentColum];

                p02 = p00 + 2;
                p12 = p00 + m + 2;
                p22 = p00 + m + m + 2;

                col_2.blue = p02->blue + p12->blue + p22->blue;
                col_2.green = p02->green + p12->green + p22->green;
                col_2.red = p02->red + p12->red + p22->red;

                sum_clr.blue = ((mid->blue << 3) + (mid->blue << 1)) - (col_0.blue + col_1.blue + col_2.blue);
                sum_clr.blue = (sum_clr.blue > 0 ? sum_clr.blue : 0);
                sum_clr.blue = (sum_clr.blue < 255 ? sum_clr.blue : 255);
                sum.blue = (unsigned char) sum_clr.blue;

                sum_clr.green = ((mid->green << 3) + (mid->green << 1)) - (col_0.green + col_1.green + col_2.green);
                sum_clr.green = (sum_clr.green > 0 ? sum_clr.green : 0);
                sum_clr.green = (sum_clr.green < 255 ? sum_clr.green : 255);
                sum.green = (unsigned char) sum_clr.green;

                sum_clr.red = ((mid->red << 3) + (mid->red << 1)) - (col_0.red + col_1.red + col_2.red);
                sum_clr.red = (sum_clr.red > 0 ? sum_clr.red : 0);
                sum_clr.red = (sum_clr.red < 255 ? sum_clr.red : 255);
                sum.red = (unsigned char) sum_clr.red;

                pixelsImg[currentColum + m + 1] = sum;
            }
            row+=m;
        }

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
    //free(backupOrg);
    //free(pixelsImg);
}

