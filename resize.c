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
    double scale_ratio_width = atof(argv[2]), scale_ratio_height = atof(argv[2]);
    if (scale_ratio_width == 0 | scale_ratio_height == 0)
    {
        printf("倍率０はだめ");
        return -1;
    }

    (*dst).width = scale_ratio_width * ((*src).width);
    (*dst).height = scale_ratio_height * ((*src).height);
    (*dst).ch = (*src).ch;

    (*dst).data = (unsigned char *)malloc(sizeof(unsigned char) * (*dst).width * (*dst).height * (*dst).ch);

    int i = 0;
    int max = (*dst).width * (*dst).height * (*dst).ch;

    int h, w, c;
    double before_scale_x, before_scale_y;
    int w0, w1, h0, h1;
    double delta_w, delta_h;

    for (w = 0; w < (*dst).width; w++)
    {
        for (h = 0; h < (*dst).height; h++)
        {
            for (c = 0; c < (*dst).ch; c++)
            {

                //処理が長いため進捗表示
                printf("%d / %d\n", i, max);
                i++;

                before_scale_x = (double)w / (double)scale_ratio_width;
                before_scale_y = (double)h / (double)scale_ratio_height;

                w0 = (int)before_scale_x;
                w1 = w0 + 1;
                h0 = (int)before_scale_y;
                h1 = h0 + 1;

                //PNGの横端、縦端の時の対応
                if (w1 == (*src).width)
                    w1 = (*src).width - 1;
                if (h1 == (*src).height)
                    h1 = (*src).height - 1;

                delta_w = before_scale_x - w0;

                //線形補完(考え中)
                (*dst).data[(*dst).ch * (w + (*dst).width * h) + c] = (*src).data[(*src).ch * (w1 + h1 * (*src).width) + c] * delta_w * delta_h + (*src).data[(*src).ch * (w1 + h0 * (*src).width) + c] * delta_w * (1 - delta_h) + (*src).data[(*src).ch * (w0 + h1 * (*src).width) + c] * (1 - delta_w) * delta_h + (*src).data[(*src).ch * (w0 + h0 * (*src).width) + c] * (1 - delta_w) * (1 - delta_h);
            }
        }
    }

    return 0;
}