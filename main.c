#include "ui.h"
#include "analysis.h"
#include "kernels.h"

extern void print(const char *);
extern void print_dec(int x);
extern void print_hex32(unsigned int x);
extern void print_hex8(unsigned char x);

// Global flag to indicate if a new selection is available
volatile int new_selection_available = 0;
volatile KernelOption selected_kernel = 0; // Assuming KernelOption is defined

/* Memory addresses (adjust based on your memory layout) */
#define START_ADDRESS 0x3000 // The starting address where the image is uploaded

// Interrupt handler (modify based on your interrupt handling mechanism)
void handle_interrupt(void) {
}

int abs(int x) {
    return (x < 0) ? -x : x;
}

/* This function prints memory content in hex */
void print_memory_hex(unsigned int *address, unsigned int len) {
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
void apply_kernel(unsigned char *pixelData, int width, int height, int bytesPerPixel, int *kernel, int kernelSize)
{
    int rowSize = (width * bytesPerPixel + 3) & ~3; // Account for padding
    int halfKernel = kernelSize / 2;

    // Define a buffer immediately after the image data
    unsigned char *tempData = pixelData + (rowSize * height) * 2;

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
                    if (neighborRow < 0)
                        neighborRow = 0;
                    if (neighborRow >= height)
                        neighborRow = height - 1;
                    if (neighborCol < 0)
                        neighborCol = 0;
                    if (neighborCol >= width)
                        neighborCol = width - 1;

                    int neighborIndex = neighborRow * rowSize + neighborCol * bytesPerPixel;
                    unsigned char *neighborPixel = pixelData + neighborIndex;

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
            unsigned char *outputPixel = tempData + pixelIndex;
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

int main()
{
    // Pointer to the beginning of the BMP file in memory
    unsigned char *bmpFile = (unsigned char *)START_ADDRESS;

    // Step 1: Identify the BMP header signature (first 2 bytes)
    unsigned short headerSignature = 
        ((unsigned short)bmpFile[0]) | ((unsigned short)bmpFile[1] << 8);
        
    if (headerSignature != 0x4D42) { // Check for 'BM' signature
        print("Not a BMP file.\n");
        return -1;
    }

    // Step 2: Extract the pixel data offset from bytes at offset 0x0A
    unsigned int pixelArrayOffset = 
        ((unsigned int)bmpFile[0x0A]) |
        ((unsigned int)bmpFile[0x0B] << 8) |
        ((unsigned int)bmpFile[0x0C] << 16) |
        ((unsigned int)bmpFile[0x0D] << 24);

    // Step 3: Extract the file size from bytes at offset 2
    unsigned int fileSize = 
        ((unsigned int)bmpFile[2]) |
        ((unsigned int)bmpFile[3] << 8) |
        ((unsigned int)bmpFile[4] << 16) |
        ((unsigned int)bmpFile[5] << 24);

    // Step 4: Extract the image width as a signed 32-bit integer
    int width = 
        ((int)bmpFile[0x12]) |
        ((int)bmpFile[0x13] << 8) |
        ((int)bmpFile[0x14] << 16) |
        ((int)bmpFile[0x15] << 24);

    // Step 5: Extract the image height as a signed 32-bit integer
    int height = 
        ((int)bmpFile[0x16]) |
        ((int)bmpFile[0x17] << 8) |
        ((int)bmpFile[0x18] << 16) |
        ((int)bmpFile[0x19] << 24);

    // Step 6: Extract the color depth (bits per pixel) from bytes at offset 0x1C
    unsigned short colorDepth = 
        ((unsigned short)bmpFile[0x1C]) |
        ((unsigned short)bmpFile[0x1D] << 8);

    // Step 7: Jump to the pixel array using the offset
    unsigned char *pixelArray = bmpFile + pixelArrayOffset;

    // Display the UI for kernel selection
    int last_selected_kernel = 0;
    int selected_kernel = 0;  // Variable to store user selection
    int chosen_kernel = 0;  // Variable to store user selection
    int chosen_kernel_2 = 0;  // Variable to store user selection

    // Display the menu once to inform the user of the options
    display_menu(0, 0, fileSize, width, height, colorDepth); // Display the menu for the user

    while (1) {
        selected_kernel = get_user_selection(); // Get the user selection from switches
        selected_kernel += 1;

        if (selected_kernel != last_selected_kernel) {
            display_menu(0, selected_kernel, fileSize, width, height, colorDepth);
        }

        chosen_kernel = get_user_chosen(); // Get the user selection from switches

        if (chosen_kernel != 0) {
            // If the user made a valid selection
            break;
        }

        last_selected_kernel = selected_kernel;
    }

    while(1) {
        int button = get_button_unpress();
        if (button == 1) 
            break;
    }

    display_menu(1, selected_kernel, fileSize, width, height, colorDepth);

    while (1) {
        selected_kernel = get_user_selection(); // Get the user selection from switches
        selected_kernel += 1;

        if (selected_kernel != last_selected_kernel) {
            display_menu(1, selected_kernel, fileSize, width, height, colorDepth);
        }

        chosen_kernel_2 = get_user_chosen(); // Get the user selection from switches

        if (chosen_kernel_2 != 0) {
            // If the user made a valid selection
            break;
        }

        last_selected_kernel = selected_kernel;
    }

    KernelInfo selected_kernel_info = kernels[chosen_kernel - 1];
    apply_kernel((unsigned char *)pixelArray, width, abs(height), colorDepth / 8, selected_kernel_info.kernel, selected_kernel_info.size);

    if (chosen_kernel_2 != 0) {
        KernelInfo selected_kernel_2_info = kernels[chosen_kernel_2 - 1];
        apply_kernel((unsigned char *)pixelArray, width, abs(height), colorDepth / 8, selected_kernel_2_info.kernel, selected_kernel_2_info.size);
    }

    print("Done!");
    return 0;
}