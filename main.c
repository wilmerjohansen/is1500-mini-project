#include<stdlib.h>

extern void print(const char*);
extern void print_dec(int x);
extern void print_hex32(unsigned int x);
extern void print_hex8(unsigned char x);

void handle_interrupt(void) {}

/* Memory addresses (adjust based on your memory layout) */
#define START_ADDRESS 0x2000  // The starting address where the image is uploaded

/* This function prints memory content in hex */
void print_memory_hex(unsigned int* address, unsigned int len) {
  for (unsigned int i = 0; i < len; i++) {
    print_hex32(address[i]);
    print("\n");
  }
}

int main() {
    // Pointer to the beginning of the BMP file in memory (adjust as needed)
    unsigned int *bmpFile = (unsigned int*)START_ADDRESS;

    // Step 1: Identify the BMP header signature (first 2 bytes)
    unsigned short headerSignature = (unsigned short)(*bmpFile & 0xFFFF);
    if (headerSignature == 0x4D42) { // 'BM' in little-endian
        print("BMP file identified.\n");
    } else {
        print("Not a BMP file.\n");
        return -1;
    }

    // Step 2: Extract the pixel data offset from bytes at offset 0x0A
    unsigned int pixelArrayOffset = (*((unsigned int*)(((char*)bmpFile) + 0x0A)));

    print("Pixel array starts at offset: ");
    print_hex32(pixelArrayOffset);
    print("\n");

    // Step 3: Extract the color depth (bits per pixel) from offset 0x2A
    int width = *((int*)(((char*)bmpFile) + 0x12));
    print("Width: ");
    print_dec(width);
    print(", ");
    print_hex32(width);
    print("\n");

    // Step 4: Extract the image height as a signed 32-bit integer
    int height = *((int*)(((char*)bmpFile) + 0x16));
    print("Height: ");
    print_dec(height);
    print(", ");
    print_hex32(height);
    print("\n");

    // Step 3: Extract the color depth (bits per pixel) from offset 0x2A
    unsigned short colorDepth = *((unsigned short*)(((char*)bmpFile) + 0x1C));
    print("Color depth (bits per pixel): ");
    print_dec(colorDepth);
    print("\n");

    // Step 4: Jump to the pixel array using the offset
    unsigned int *pixelArray = (unsigned int*)(((char*)bmpFile) + pixelArrayOffset);

    // Example: print the first pixel value (in BGR format)
    print("First pixel value (BGR): ");
    print_hex32(*pixelArray);
    print("\n");

    //print_memory_hex(bmpFile, 50);

    unsigned int pixelCount = abs(width * height);
    for (unsigned int i = 20000; i < 30000; i++) {
        // Each pixel is 4 bytes (ARGB), extract the current pixel value
        unsigned int pixel;
        pixel = *pixelArray;
        pixel |= 0x0000FF00;
        *pixelArray = pixel;
        pixelArray += 0x04;
    }

    print("Done!");

    return 0;
}