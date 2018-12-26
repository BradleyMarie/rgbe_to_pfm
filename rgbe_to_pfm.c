/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    rgbe_to_pfm.c

Abstract:

    Converts an RGBE file to PFM.

--*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rgbe.h"

//
// Static Functions
//

static
bool 
IsLittleEndian(
    void
    )
{
    union {
        uint16_t value;
        char bytes[2];
    } value = { 0x0001 };

    return value.bytes[0] == 1;
}

static
bool
WriteToPfmFile(
    const float* image,
    int width,
    int height, 
    const char* filename
    )
{
    assert(image != NULL);
    assert(0 < width);
    assert(0 < height);
    assert(filename != NULL);

    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        return false;
    }

    if (fprintf(file, "PF\n") < 0)
    {
        fclose(file);
        return false;
    }

    if (fprintf(file, "%d %d\n", width, height) < 0)
    {
        fclose(file);
        return false;
    }

    if (fprintf(file, "%lf\n", IsLittleEndian() ? -1.0f : 1.0f) < 0)
    {
        fclose(file);
        return false;
    }

    for (size_t i = height; i != 0; i--)
    {
        for (size_t j = 0; j < width; j++)
        {
            const float* pixel = &image[(i - 1) * width * 3 + j * 3];

            if (fwrite(pixel, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return false;
            }

            if (fwrite(pixel + 1, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return false;
            }

            if (fwrite(pixel + 2, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return false;
            }
        }
    }

    if (fclose(file) == EOF)
    {
        return false;
    }

    return true;
}

//
// Functions
//

int
main(
    int argc,
    char **argv
    )
{
    if (argc != 3)
    {
        printf("usage: rgbe_to_pfm <input> <output>\n");
        return 0;
    }

    FILE *input = fopen(argv[1], "rb");

    int width, height;
    RGBE_ReadHeader(input, &width, &height, NULL);

    float *image = (float *)malloc(sizeof(float) * 3 * width * height);
    if (image == NULL)
    {
        return -1;
    }

    RGBE_ReadPixels_RLE(input, image, width, height);
    fclose(input);

    bool success = WriteToPfmFile(image, width, height, argv[2]);
    free(image);

    return (success) ? 0 : -1;
}