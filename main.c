#include <stdlib.h>
#include <stdio.h>   

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

void apply_blur(unsigned char* pixelData, int width, int height, int bytesPerPixel) {
    int rowSize = (width * bytesPerPixel + 3) & ~3; // Account for padding

    // Define a buffer immediately after the image data
    unsigned char* tempData = pixelData + (rowSize * height) * 2;

    print("pixelData address: ");
    print_hex32((unsigned int)pixelData);
    print("\n");

    print("tempData address: ");
    print_hex32((unsigned int)tempData);
    print("\n");

    // Iterate over each pixel, skipping the edges
    for (int row = 1; row < height - 1; row++) {

        for (int col = 1; col < width - 1; col++) {
            int pixelIndex = row * rowSize + col * bytesPerPixel;

            // Initialize accumulators for RGB channels
            int sumR = 0, sumG = 0, sumB = 0;

            // Iterate over the 3x3 kernel
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int neighborIndex = (row + ky) * rowSize + (col + kx) * bytesPerPixel;
                    unsigned char* neighborPixel = pixelData + neighborIndex;

                    sumB += neighborPixel[0]; // Blue channel
                    sumG += neighborPixel[1]; // Green channel
                    sumR += neighborPixel[2]; // Red channel
                }
            }

            // Write the averaged values to the temporary buffer
            unsigned char* outputPixel = tempData + pixelIndex;
            outputPixel[0] = sumB / 9; // Blue channel
            outputPixel[1] = sumG / 9; // Green channel
            outputPixel[2] = sumR / 9; // Red channel
        }
    }

    // Copy the blurred data back into the original pixel array
    for (int i = 0; i < rowSize * height; i++) {
        pixelData[i] = tempData[i];
    }
}

void apply_edge_detection(unsigned char* pixelData, int width, int height, int bytesPerPixel) {
    int rowSize = (width * bytesPerPixel + 3) & ~3; // Account for padding

    // Define a buffer immediately after the image data
    unsigned char* tempData = pixelData + (rowSize * height) * 2;

    print("pixelData address: ");
    print_hex32((unsigned int)pixelData);
    print("\n");

    print("tempData address: ");
    print_hex32((unsigned int)tempData);
    print("\n");

    // Edge detection kernel:
    // [-1, -1, -1]
    // [-1,  8, -1]
    // [-1, -1, -1]
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            int pixelIndex = row * rowSize + col * bytesPerPixel;

            // Initialize accumulators for RGB channels
            int sumR = 0, sumG = 0, sumB = 0;

            // Apply the 3x3 kernel for edge detection
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int neighborIndex = (row + ky) * rowSize + (col + kx) * bytesPerPixel;
                    unsigned char* neighborPixel = pixelData + neighborIndex;

                    int weight = -1; // Default weight for surrounding pixels
                    if (ky == 0 && kx == 0) {
                        weight = 8;  // Center pixel weight
                    }

                    sumB += neighborPixel[0] * weight; // Blue channel
                    sumG += neighborPixel[1] * weight; // Green channel
                    sumR += neighborPixel[2] * weight; // Red channel
                }
            }

            // Clamp values to be within 0-255 range
            sumR = sumR < 0 ? 0 : (sumR > 255 ? 255 : sumR);
            sumG = sumG < 0 ? 0 : (sumG > 255 ? 255 : sumG);
            sumB = sumB < 0 ? 0 : (sumB > 255 ? 255 : sumB);

            // Write the edge-detected values to the temporary buffer
            unsigned char* outputPixel = tempData + pixelIndex;
            outputPixel[0] = sumB; // Blue channel
            outputPixel[1] = sumG; // Green channel
            outputPixel[2] = sumR; // Red channel
        }
    }

    // Copy the edge-detected data back into the original pixel array
    for (int i = 0; i < rowSize * height; i++) {
        pixelData[i] = tempData[i];
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

    // Step 3: Extract the image width as a signed 32-bit integer
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

    //apply_blur((unsigned char*)pixelArray, width, abs(height), colorDepth / 8);
    apply_edge_detection((unsigned char*)pixelArray, width, abs(height), colorDepth / 8);

    print("Done!");

    return 0;
}