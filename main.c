#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <png.h>
#define SIGNATURE_NUM 8

typedef struct
{
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    unsigned int ch;
} BITMAPDATA_t;

int png_read(BITMAPDATA_t *bitmap_data, const char *file_name);
int png_write(BITMAPDATA_t *bitmap_data, const char *filename);

int main(int argc, char *argv[])
{
    char dllname[256];
    void *handle;
    snprintf(dllname, sizeof(dllname), "./%s.dylib", argv[1]);
    printf("%s\n", dllname);
    handle = dlopen(dllname, RTLD_LAZY);

    //PNG読み込む
    BITMAPDATA_t src, dst;
    char file_name[256];
    printf("ファイル名：");
    scanf("%s", file_name);

    if (png_read(&src, file_name) == -1)
    {
        printf("pngファイルじゃないかもしれません\n");
        return -1;
    }

    int (*execute)(BITMAPDATA_t * src, BITMAPDATA_t * dst, char **argv, int argc) = dlsym(handle, "execute");
    execute(&src, &dst, argv, argc);

    char outname[256];
    sprintf(outname, "%s", "output.png");
    if (png_write(&dst, outname) == -1)
    {
        printf("失敗しました\n");
        return -1;
    }

    return 0;
}

int png_read(BITMAPDATA_t *bitmap_data, const char *file_name)
{

    FILE *fi;
    int i;
    unsigned int width, height;
    unsigned int read_size;

    png_structp png;
    png_infop info;
    png_bytepp png_row_data;
    png_byte type;
    png_byte signature[8];

    fi = fopen(file_name, "rb");
    if (fi == NULL)
    {
        printf("%sは開けません\n", file_name);
        return -1;
    }

    read_size = fread(signature, 1, SIGNATURE_NUM, fi);

    //ファイルがPNGかどうかチェック
    if (png_sig_cmp(signature, 0, SIGNATURE_NUM))
    {
        printf("png_sig_cmpでエラーが起きた!\n");
        fclose(fi);
        return -1;
    }

    // png_read構造体を生成する関数、第2引数以降はエラー警告系
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
    {
        printf("png_create_read_structでエラーが起きた!\n");
        fclose(fi);
        return -1;
    }

    // png_info構造体を生成する関数
    info = png_create_info_struct(png);
    if (info == NULL)
    {
        printf("png_crete_info_structでエラーが起きた!\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fi);
        return -1;
    }

    //PNGファイルとして読み込むファイルを設定
    png_init_io(png, fi);

    //シグネチャサイズを指定（PNGの場合8）
    png_set_sig_bytes(png, read_size);

    //PNGファイル読み込み(第3引数は画素の輝度値を8ビットにするための指定)
    png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    png_row_data = png_get_rows(png, info);
    type = png_get_color_type(png, info);

    if (type == PNG_COLOR_TYPE_RGB)
    {
        printf("RGBでした.\n");
    }
    if (type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        printf("RGBAでした.\n");
    }
    if (type == PNG_COLOR_TYPE_PALETTE)
    {
        printf("パレットでした.\n");
    }

    if (type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGB_ALPHA && type != PNG_COLOR_TYPE_PALETTE)
    {
        printf("color type is not RGB or RGBA or Pallet\n");
        printf("%hhu\n", type);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fi);
        return -1;
    }

    bitmap_data->width = width;
    bitmap_data->height = height;
    if (type == PNG_COLOR_TYPE_RGB)
    {
        bitmap_data->ch = 3;
    }
    else if (type == PNG_COLOR_TYPE_RGBA)
    {
        bitmap_data->ch = 4;
    }
    else if (type == PNG_COLOR_TYPE_PALETTE)
    {
        png_colorp palette;
        int num;
        png_get_PLTE(png, info, &palette, &num);
        bitmap_data->ch = num; //多分違う
    }
    printf("width = %d, height = %d, ch = %d\n", bitmap_data->width, bitmap_data->height, bitmap_data->ch);

    bitmap_data->data = (unsigned char *)malloc(sizeof(unsigned char) * bitmap_data->width * bitmap_data->height * bitmap_data->ch);
    if (bitmap_data->data == NULL)
    {
        printf("data mallocでエラーが起きた\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fi);
        return -1;
    }

    for (i = 0; i < bitmap_data->height; i++)
    {
        //確保してあるメモリの先頭ポインタから数える
        memcpy(bitmap_data->data + i * bitmap_data->width * bitmap_data->ch, png_row_data[i], bitmap_data->width * bitmap_data->ch);
    }

    png_destroy_read_struct(&png, &info, NULL);
    fclose(fi);

    return 0;
}

int png_write(BITMAPDATA_t *bitmap_data, const char *filename)
{
    FILE *fo;
    int i;

    png_structp png;
    png_infop info;
    png_bytepp png_row_data;
    png_byte type;

    fo = fopen(filename, "wb");
    if (fo == NULL)
    {
        printf("%sは開けません\n", filename);
        return -1;
    }

    //png_write構造体を生成する関数
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(png);

    if (bitmap_data->ch == 3)
    {
        type = PNG_COLOR_TYPE_RGB;
    }
    else if (bitmap_data->ch == 4)
    {
        type = PNG_COLOR_TYPE_RGB_ALPHA;
    }
    else if (bitmap_data->ch == 256)
    {
        type = PNG_COLOR_TYPE_PALETTE;
    }
    else
    {
        printf("ch num is invalid!\n");
        png_destroy_write_struct(&png, &info);
        fclose(fo);
        return -1;
    }
    png_init_io(png, fo);

    //生成するPNGのヘッダの設定(PNGのメタデータ)インターレースは荒くてもとりあえず表示する
    png_set_IHDR(png, info, bitmap_data->width, bitmap_data->height, 8, type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_row_data = png_malloc(png, sizeof(png_bytep) * bitmap_data->height);

    png_set_rows(png, info, png_row_data);

    for (i = 0; i < bitmap_data->height; i++)
    {
        png_row_data[i] = png_malloc(png, bitmap_data->width * bitmap_data->ch);
        memcpy(png_row_data[i], bitmap_data->data + i * bitmap_data->width * bitmap_data->ch, bitmap_data->width * bitmap_data->ch);
    }
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    //メモリ解放
    for (i = 0; i < bitmap_data->height; i++)
    {
        png_free(png, png_row_data[i]);
    }
    png_free(png, png_row_data);

    png_destroy_write_struct(&png, &info);
    fclose(fo);
    return 0;
}
