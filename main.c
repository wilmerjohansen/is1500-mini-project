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

/**
 * Apply a generic convolution kernel to the image data.
 * 
 * @param pixelData      Pointer to the image pixel data.
 * @param width          Width of the image.
 * @param height         Height of the image.
 * @param bytesPerPixel  Number of bytes per pixel.
 * @param kernel         Pointer to the kernel weights.
 * @param kernelSize     Size of the kernel (e.g., 3 for a 3x3 kernel).
 */
void apply_kernel(unsigned char* pixelData, int width, int height, int bytesPerPixel, int* kernel, int kernelSize) {
    int rowSize = (width * bytesPerPixel + 3) & ~3; // Account for padding
    int halfKernel = kernelSize / 2;

    // Define a buffer immediately after the image data
    unsigned char* tempData = pixelData + (rowSize * height) * 2;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int pixelIndex = row * rowSize + col * bytesPerPixel;

            // Initialize accumulators for RGB channels
            int sumR = 0, sumG = 0, sumB = 0;
            int kernelSum = 0; // Keep track of the total kernel weight for normalization

            // Apply the kernel
            for (int ky = -halfKernel; ky <= halfKernel; ky++) {
                for (int kx = -halfKernel; kx <= halfKernel; kx++) {
                    int neighborRow = row + ky;
                    int neighborCol = col + kx;

                    // Replicate border pixels for out-of-bounds areas
                    if (neighborRow < 0) neighborRow = 0;
                    if (neighborRow >= height) neighborRow = height - 1;
                    if (neighborCol < 0) neighborCol = 0;
                    if (neighborCol >= width) neighborCol = width - 1;

                    int neighborIndex = neighborRow * rowSize + neighborCol * bytesPerPixel;
                    unsigned char* neighborPixel = pixelData + neighborIndex;

                    int kernelWeight = kernel[(ky + halfKernel) * kernelSize + (kx + halfKernel)];

                    sumB += neighborPixel[0] * kernelWeight; // Blue channel
                    sumG += neighborPixel[1] * kernelWeight; // Green channel
                    sumR += neighborPixel[2] * kernelWeight; // Red channel
                    kernelSum += kernelWeight;
                }
            }

            // Normalize the result if kernel weights don't sum to 1
            if (kernelSum != 0) {
                sumB /= kernelSum;
                sumG /= kernelSum;
                sumR /= kernelSum;
            }

            // Clamp values to the range [0, 255]
            sumR = sumR < 0 ? 0 : (sumR > 255 ? 255 : sumR);
            sumG = sumG < 0 ? 0 : (sumG > 255 ? 255 : sumG);
            sumB = sumB < 0 ? 0 : (sumB > 255 ? 255 : sumB);

            // Write the processed pixel to the temporary buffer
            unsigned char* outputPixel = tempData + pixelIndex;
            outputPixel[0] = sumB; // Blue channel
            outputPixel[1] = sumG; // Green channel
            outputPixel[2] = sumR; // Red channel
        }
    }

    // Copy the processed data back into the original pixel array
    for (int i = 0; i < rowSize * height; i++) {
        pixelData[i] = tempData[i];
    }
}

/* Predefined Kernels */
int edgeDetection3x3[9] = {
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
};

int edgeDetection5x5[25] = {
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, 24, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1
};

int blur3x3[9] = {
     1,  1,  1,
     1,  1,  1,
     1,  1,  1
};

int blur5x5[25] = {
     1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,
     1,  1,  1,  1,  1
};

int sharpen3x3[9] = {
     0, -1,  0,
    -1,  5, -1,
     0, -1,  0
};

int sharpen5x5[25] = {
     0,  0, -1,  0,  0,
     0, -1, -1, -1,  0,
    -1, -1, 13, -1, -1,
     0, -1, -1, -1,  0,
     0,  0, -1,  0,  0
};

int emboss3x3[9] = {
    -2, -1,  0,
    -1,  1,  1,
     0,  1,  2
};

int emboss5x5[25] = {
    -2, -1,  0,  1,  2,
    -1, -1,  0,  1,  1,
     0,  0,  1,  0,  0,
     1,  1,  0, -1, -1,
     2,  1,  0, -1, -2
};

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
    print("\n");

    // Step 4: Extract the image height as a signed 32-bit integer
    int height = *((int*)(((char*)bmpFile) + 0x16));
    print("Height: ");
    print_dec(height);
    print("\n");

    // Step 3: Extract the color depth (bits per pixel) from offset 0x2A
    unsigned short colorDepth = *((unsigned short*)(((char*)bmpFile) + 0x1C));
    print("Color depth (bits per pixel): ");
    print_dec(colorDepth);
    print("\n");

    // Step 4: Jump to the pixel array using the offset
    unsigned int *pixelArray = (unsigned int*)(((char*)bmpFile) + pixelArrayOffset);

    // Apply one of the kernels (for example, edge detection with 3x3 kernel)
    apply_kernel((unsigned char*)pixelArray, width, abs(height), colorDepth / 8, edgeDetection5x5, 5);
    // Apply another one
    apply_kernel((unsigned char*)pixelArray, width, abs(height), colorDepth / 8, blur3x3, 3);

    print("Done!");
    return 0;
}
