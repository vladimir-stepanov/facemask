#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstring>

struct ASTCHeader
{
    uint8_t magic[4];
    uint8_t blockdim_x;
    uint8_t blockdim_y;
    uint8_t blockdim_z;
    uint8_t xsize[3];
    uint8_t ysize[3];
    uint8_t zsize[3];
};

struct KtxHeader
{
    uint8_t identifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };
    uint8_t endianness[4] = {1, 2, 3, 4};
    uint32_t glType = 0;
    uint32_t glTypeSize = 1;
    uint32_t glFormat = 0;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat = 0x1908; //GL_RGBA
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth = 0;
    uint32_t numberOfArrayElements = 0;
    uint32_t numberOfFaces = 1;
    uint32_t numberOfMipmapLevels = 1;
    uint32_t bytesOfKeyValueData = 0;
    // here could go key-value data, but we skip it
    uint32_t imageSize;
};

void printUsage()
{
    fprintf(stderr, "astc2ktx <input file> <output file>\n");
}

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printUsage();
        return -1;
    }

    printf("Converting ASTC from %s to KTX %s\n", argv[1], argv[2]);

    FILE *input = fopen(argv[1], "rb");
    if(!input) {
        fprintf(stderr, "Failed to open %s for input!\n", argv[1]);
        return -1;
    }

    FILE *output = fopen(argv[2], "wb");
    if(!output) {
        fprintf(stderr, "Failed to open %s for output!\n", argv[2]);
        fclose(input);
        return -1;
    }

    ASTCHeader astcHeader;
    static const uint8_t correctMagic[] = {0x13, 0xab, 0xa1, 0x5c};
    fread(&astcHeader, sizeof(ASTCHeader), 1, input);

    if(memcmp(correctMagic, &astcHeader.magic, sizeof(correctMagic))) {
        fprintf(stderr, "Invalid ASTC magic! Corrupted file?\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    uint32_t width = astcHeader.xsize[0] + (astcHeader.xsize[1] << 8) + (astcHeader.xsize[2] << 16);
    uint32_t height = astcHeader.ysize[0] + (astcHeader.ysize[1] << 8) + (astcHeader.ysize[2] << 16);

    printf("Size: %ix%i, block size: %ix%i\n", width, height, astcHeader.blockdim_x, astcHeader.blockdim_y);

    size_t blockSize = astcHeader.blockdim_x * astcHeader.blockdim_y;
    uint32_t format = 0x93AF;
    switch(blockSize) {
        case 144:   format++;
        case 120:   format++;
        case 100:   format++;
        case 80:    format++;
        case 60:    format++;
        case 50:    format++;
        case 64:    format++;
        case 48:    format++;
        case 40:    format++;
        case 36:    format++;
        case 30:    format++;
        case 25:    format++;
        case 20:    format++;
        case 16:    format++;
            break;
        default:
            fprintf(stderr, "Unsupported block size\n");
            return -1;
    }

    printf("OpenGL internal format: 0x%04x\n", format);

    size_t dataSize = 16 * ((width + astcHeader.blockdim_x - 1) / astcHeader.blockdim_x)
                         * ((height + astcHeader.blockdim_y - 1) / astcHeader.blockdim_y);

    printf("Data size: %zu bytes\n", dataSize);

    std::vector<uint8_t> data(dataSize);
    if(dataSize != fread(&data[0], 1, dataSize, input))
    {
        fprintf(stderr, "Could NOT read the data completely!\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    KtxHeader ktxHeader;
    ktxHeader.glInternalFormat = format;
    ktxHeader.pixelWidth = width;
    ktxHeader.pixelHeight = height;
    ktxHeader.imageSize = dataSize;

    fwrite(&ktxHeader, 1, sizeof(ktxHeader), output);
    fwrite(&data[0], 1, data.size(), output);

    fclose(output);
    fclose(input);

    return 0;
}
