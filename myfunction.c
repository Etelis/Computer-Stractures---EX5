#pragma GCC optimize ("O1,O2,O3,Ofast,unroll-loops,fast-math")
#define NORMALIZE(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : (X))
#include "stdint.h"

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
 *          2. KernelScale = 1 no filter: sharpen: a sum of 10times mid - sum_of_all matrix cells, devided by 7.
 *          3. KernelScale = 7 filter: blur : meaning all items should be summed, max + min should be removed and sum should be done over 7 items.
 *          4. KernelScale = 1 filter: same as 2.
 */

void blur_no_filter(pixel *img, pixel *backup_img) {
    register pixel *prow0, *prow1, *prow2;
    pixel_sum col_0, col_1, col_2;
    register uint_fast16_t row, apply_index, j;
    for (row = 0; row < m - 2; row++) {
        // Copy first three rows to the local variables to use a friendly cache approach.
        prow0 = &backup_img[n*row];
        prow1 = &backup_img[n*(row+1)];
        prow2 = &backup_img[n*(row+2)];

        // sum first colum by each color.
        col_0.red = prow0->red + prow1->red + prow2->red;
        col_0.green = prow0->green + prow1->green + prow2->green;
        col_0.blue = prow0->blue + prow1->blue + prow2->blue;

        // sum second colum
        prow0++;
        prow1++;
        prow2++;
        col_1.red = prow0->red + prow1->red + prow2->red;
        col_1.green = prow0->green + prow1->green + prow2->green;
        col_1.blue = prow0->blue + prow1->blue + prow2->blue;

        apply_index = n * (row + 1) + 1;
        // time to iterate over the colums of the current row.
        for (j = 0; j < n - 2; j++) {
            // unrolling was not made as it damaged performance.
            prow0++;
            prow1++;
            prow2++;
            // sum third colum.
            col_2.red = prow0->red + prow1->red + prow2->red;
            col_2.green = prow0->green + prow1->green + prow2->green;
            col_2.blue = prow0->blue + prow1->blue + prow2->blue;
            // apply on image.
            img[apply_index].red = (col_0.red + col_1.red + col_2.red) / 9;
            img[apply_index].green = (col_0.green + col_1.green + col_2.green) / 9;
            img[apply_index].blue = (col_0.blue + col_1.blue + col_2.blue) / 9;

            // the following approach allows us to avoid calculating first and second colum again.
            // apply first colum to second colum.
            col_0 = col_1;
            // apply second colum to third colum.
            col_1 = col_2;
            apply_index++;
        }
    }
}


void blur_filter(pixel *img, pixel *backup_img) {
    register pixel *prow0, *prow1, *prow2;
    pixel_sum col_0, col_1, col_2;
    register uint_fast16_t row, apply_index, j;
    register int_fast16_t intensity00, intensity01, intensity02, intensity10, intensity11, intensity12, intensity20, intensity21, intensity22;
    register int_fast16_t min_intensity, max_intensity;
    register pixel *min_intensity_pixel, *max_intensity_pixel;
    for (row = 0; row < m - 2; row++) {
        // Copy first three rows to the local variables to use a friendly cache approach.
        prow0 = &backup_img[n*row];
        prow1 = &backup_img[n*(row+1)];
        prow2 = &backup_img[n*(row+2)];

        // sum first colum by each color.
        col_0.red = prow0->red + prow1->red + prow2->red;
        col_0.green = prow0->green + prow1->green + prow2->green;
        col_0.blue = prow0->blue + prow1->blue + prow2->blue;
        // calculating intensity for current row.
        intensity00 = prow0->red + prow0->green + prow0->blue;
        intensity10 = prow1->red + prow1->green + prow1->blue;
        intensity20 = prow2->red + prow2->green + prow2->blue;
        // do the same for the second row.
        prow0++;
        prow1++;
        prow2++;
        col_1.red = prow0->red + prow1->red + prow2->red;
        col_1.green = prow0->green + prow1->green + prow2->green;
        col_1.blue = prow0->blue + prow1->blue + prow2->blue;
        intensity01 = prow0->red + prow0->green + prow0->blue;
        intensity11 = prow1->red + prow1->green + prow1->blue;
        intensity21 = prow2->red + prow2->green + prow2->blue;

        apply_index = n * (row + 1) + 1;
        for (j = 0; j < n - 2; j++) {
            // unrolling was not made as it damaged performance.

            prow0++;
            prow1++;
            prow2++;

            // sum third colum.
            col_2.red = prow0->red + prow1->red + prow2->red;
            col_2.green = prow0->green + prow1->green + prow2->green;
            col_2.blue = prow0->blue + prow1->blue + prow2->blue;
            intensity02 = prow0->red + prow0->green + prow0->blue;
            intensity12 = prow1->red + prow1->green + prow1->blue;
            intensity22 = prow2->red + prow2->green + prow2->blue;

            // iterate over the intensity to destinguish who has the largest value of the current iteration.
            min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
            max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
            (intensity00 <= min_intensity) && (min_intensity_pixel = prow0 - 2, min_intensity = intensity00);
            (intensity00 > max_intensity) && (max_intensity_pixel = prow0 - 2, max_intensity = intensity00);
            (intensity01 <= min_intensity) && (min_intensity_pixel = prow0 - 1, min_intensity = intensity01);
            (intensity01 > max_intensity) && (max_intensity_pixel = prow0 - 1, max_intensity = intensity01);
            (intensity02 <= min_intensity) && (min_intensity_pixel = prow0, min_intensity = intensity02);
            (intensity02 > max_intensity) && (max_intensity_pixel = prow0, max_intensity = intensity02);
            (intensity10 <= min_intensity) && (min_intensity_pixel = prow1 - 2, min_intensity = intensity10);
            (intensity10 > max_intensity) && (max_intensity_pixel = prow1 - 2, max_intensity = intensity10);
            (intensity11 <= min_intensity) && (min_intensity_pixel = prow1 - 1, min_intensity = intensity11);
            (intensity11 > max_intensity) && (max_intensity_pixel = prow1 - 1, max_intensity = intensity11);
            (intensity12 <= min_intensity) && (min_intensity_pixel = prow1, min_intensity = intensity12);
            (intensity12 > max_intensity) && (max_intensity_pixel = prow1, max_intensity = intensity12);
            (intensity20 <= min_intensity) && (min_intensity_pixel = prow2 - 2, min_intensity = intensity20);
            (intensity20 > max_intensity) && (max_intensity_pixel = prow2 - 2, max_intensity = intensity20);
            (intensity21 <= min_intensity) && (min_intensity_pixel = prow2 - 1, min_intensity = intensity21);
            (intensity21 > max_intensity) && (max_intensity_pixel = prow2 - 1, max_intensity = intensity21);
            (intensity22 <= min_intensity) && (min_intensity_pixel = prow2, min_intensity = intensity22);
            (intensity22 > max_intensity) && (max_intensity_pixel = prow2, max_intensity = intensity22);

            // apply to image.
            img[apply_index].red = (col_0.red + col_1.red + col_2.red - min_intensity_pixel->red - max_intensity_pixel->red) / 7;
            img[apply_index].green = (col_0.green + col_1.green + col_2.green - min_intensity_pixel->green - max_intensity_pixel->green) / 7;
            img[apply_index].blue = (col_0.blue + col_1.blue + col_2.blue - min_intensity_pixel->blue - max_intensity_pixel->blue) / 7;

            // the following approach allows us to avoid calculating first and second colum again.
            // apply first colum to second colum.
            col_0 = col_1;
            col_1 = col_2;
            intensity00 = intensity01;
            intensity10 = intensity11;
            intensity20 = intensity21;
            intensity01 = intensity02;
            intensity11 = intensity12;
            intensity21 = intensity22;
            apply_index++;
        }
    }
}


void sharpen(pixel *img, pixel *backup_img) {
    register pixel *prow0, *prow1, *prow2, *mid;
    pixel_sum col_0, col_1, col_2;
    pixel_sum clr;
    register uint_fast16_t row, apply_index, j;
    for (row = 0; row < m - 2; row++) {
        // Copy first three rows to the local variables to use a friendly cache approach.
        prow0 = &backup_img[n*row];
        prow1 = &backup_img[n*(row+1)];
        prow2 = &backup_img[n*(row+2)];
        // sum first colum by each color.
        col_0.red = prow0->red + prow1->red + prow2->red;
        col_0.green = prow0->green + prow1->green + prow2->green;
        col_0.blue = prow0->blue + prow1->blue + prow2->blue;
        prow0++;
        prow1++;
        prow2++;

        // do the same for the second row.
        col_1.red = prow0->red + prow1->red + prow2->red;
        col_1.green = prow0->green + prow1->green + prow2->green;
        col_1.blue = prow0->blue + prow1->blue + prow2->blue;

        apply_index = n * (row + 1) + 1;
        for (j = 0; j < n - 2; j++) {
            // sum third colum.
            // save mid-value for future use.
            mid = prow1;
            prow0++;
            prow1++;
            prow2++;
            col_2.red = prow0->red + prow1->red + prow2->red;
            col_2.green = prow0->green + prow1->green + prow2->green;
            col_2.blue = prow0->blue + prow1->blue + prow2->blue;

            // instead of normal calculation presented at original file, calculate 10*mid - sum_collected.
            clr.red = 10 * mid->red - (col_0.red + col_1.red + col_2.red);
            clr.green = 10 * mid->green - (col_0.green + col_1.green + col_2.green);
            clr.blue = 10 * mid->blue - (col_0.blue + col_1.blue + col_2.blue);

            //  apply normalized values ( X>0 AND X<255)
            img[apply_index].red = NORMALIZE(clr.red);
            img[apply_index].green = NORMALIZE(clr.green);
            img[apply_index].blue = NORMALIZE(clr.blue);

            col_0 = col_1;
            col_1 = col_2;
            apply_index++;
        }
    }
}

void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {

    size_t size = m * n * sizeof(pixel);

    // why copy when we could only apply by pointers.
    register pixel* pixelsImg = (pixel *) image->data;
    register pixel* backupOrg = malloc(size);

    // backupOrg should be copied to keep avoid changes.
    memcpy(backupOrg, pixelsImg, size);

    if (flag == '1') {
        // blur image
        blur_no_filter(pixelsImg, backupOrg);

        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);

        // backup should be updated to current state before sharping.
        memcpy(backupOrg, pixelsImg, size);

        // sharpen the resulting image
        sharpen(pixelsImg, backupOrg);

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
    } else { // flag != '1' - meaning an intensity will be applied
        // blur image with filter
        blur_filter(pixelsImg, backupOrg);

        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

        // backup should be updated to current state before sharping.
        memcpy(backupOrg, pixelsImg, size);

        // sharpen the resulting image
        sharpen(pixelsImg, backupOrg);

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
    //free(backupOrg);
    //free(pixelsImg);
}
