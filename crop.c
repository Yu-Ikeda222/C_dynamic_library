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
    int start_x = atoi(argv[2]), end_x = atoi(argv[3]);
    int start_y = atoi(argv[4]), end_y = atoi(argv[5]);

    printf("start_x: %d end_x: %d start_y: %d end_y: %d\n", start_x, end_x, start_y, end_y);
    if (start_x<0 | start_x>(*src).width)
    {
        printf("x座標の始まりがおかしい");
        return -1;
    }
    if (end_x<0 | end_x>(*src).width | end_x < start_x)
    {
        printf("x座標の終わりがおかしい");
        return -1;
    }
    if (start_y<0 | start_y>(*src).height)
    {
        printf("x座標の始まりがおかしい");
        return -1;
    }
    if (end_y<0 | end_y>(*src).height | end_y < start_y)
    {
        printf("x座標の終わりがおかしい");
        return -1;
    }

    (*dst).width = end_x - start_x;
    (*dst).height = end_y - start_y;
    (*dst).ch = (*src).ch;

    printf("width = %d, height = %d, ch = %d\n", (*dst).width, (*dst).height, (*dst).ch);

    (*dst).data = (unsigned char *)malloc(sizeof(unsigned char) * (*dst).width * (*dst).height * (*dst).ch);

    int h, w, c;

    for (w = 0; w < (*dst).width; w++)
    {
        for (h = 0; h < (*dst).height; h++)
        {
            for (c = 0; c < (*dst).ch; c++)
            {
                (*dst).data[((*dst).ch * (w + ((*dst).width * h))) + c] = (*src).data[((*src).ch * ((w + start_x) + ((*src).width * (h + start_y)))) + c];
            }
        }
    }

    return 0;
}