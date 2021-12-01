#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    int angle = atoi(argv[2]);
    double radian;

    if (angle > 359 | angle < 0)
    {
        printf("0-359まででお願いします");
        return -1;
    }

    radian = -(double)angle * M_PI / (double)180;

    (*dst).width = (*src).width;
    (*dst).height = (*src).height;
    (*dst).ch = (*src).ch;

    (*dst).data = (unsigned char *)malloc(sizeof(unsigned char) * (*dst).width * (*dst).height * (*dst).ch);
    memset((*dst).data, 0, (*dst).width * (*dst).height * (*dst).ch);

    int x, y, c;
    double original_x, original_y;
    int round_original_x, round_original_y;

    for (x = 0; x < (*dst).width; x++)
    {
        for (y = 0; y < (*dst).height; y++)
        {
            original_x = (x - (int)((*dst).width / 2)) * cos(radian) + (y - (int)((*dst).height / 2)) * sin(radian) + (*src).width / 2;
            original_y = -(x - (int)((*dst).width / 2)) * sin(radian) + (y - (int)((*dst).height / 2)) * cos(radian) + (*src).height / 2;

            round_original_x = original_x + 0.5;
            round_original_y = original_y + 0.5;

            if (round_original_x >= (*src).width | round_original_x < 0)
                continue;
            if (round_original_y >= (*src).height | round_original_y < 0)
                continue;

            for (c = 0; c < (*dst).ch; c++)
            {
                (*dst).data[(*dst).ch * (x + y * (*dst).width) + c] = (*src).data[(*src).ch * (round_original_x + round_original_y * (*src).width) + c];
            }
        }
    }

    return 0;
}