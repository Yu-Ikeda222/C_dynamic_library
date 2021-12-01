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
    double sum, sepia_rate;

    (*dst) = (*src);

    for (h = 0; h < (*dst).height; h++)
    {
        for (w = 0; w < (*dst).width; w++)
        {
            // sum = 0;
            // for (c = 0; c < (*dst).ch; c++)
            // {
            //     sum += (*dst).data[(*dst).ch * (w + h * (*dst).width) + c];
            // }
            // sepia_rate = sum / 765 * 100;
            for (c = 0; c < (*dst).ch; c++)
            {
                if (c == 1)
                {
                    (*dst).data[(*dst).ch * (w + h * (*dst).width) + c] = (*dst).data[(*dst).ch * (w + h * (*dst).width) + c] * 0.3;
                }
                if (c == 2)
                {
                    (*dst).data[(*dst).ch * (w + h * (*dst).width) + c] = (*dst).data[(*dst).ch * (w + h * (*dst).width) + c] * 0.8;
                }
            }
        }
    }
    return 0;
}