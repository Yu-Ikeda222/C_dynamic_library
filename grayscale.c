#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>
#define SIGNATURE_NUM 8

typedef struct
{
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    unsigned int ch;
} BITMAPDATA_t;

int execute(BITMAPDATA_t *src, BITMAPDATA_t *dst, char **argv, int argc)
{
    int h, w, c;
    int ave, sum;

    (*dst) = (*src);

    for (h = 0; h < (*dst).height; h++)
    {
        for (w = 0; w < (*dst).width; w++)
        {
            sum = 0;
            for (c = 0; c < (*dst).ch; c++)
            {
                sum += (*dst).data[(*dst).ch * (w + h * (*dst).width) + c];
            }
            ave = sum / (*dst).ch;
            for (c = 0; c < (*dst).ch; c++)
            {
                (*dst).data[(*dst).ch * (w + h * (*dst).width) + c] = ave;
            }
        }
    }
    return 0;
}