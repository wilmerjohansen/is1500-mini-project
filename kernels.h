/* Kernels Info Struct */
typedef struct {
    int* kernel;    // Pointer to the kernel array
    int size;       // Kernel size (e.g., 3 for 3x3, 5 for 5x5)
} KernelInfo;

/* Predefined Kernels */
int edgeDetection3x3[9] = {
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1};

int edgeDetection5x5[25] = {
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, 24, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1};

int blur3x3[9] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1};

int blur5x5[25] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1};

int sharpen3x3[9] = {
    0, -1, 0,
    -1, 5, -1,
    0, -1, 0};

int sharpen5x5[25] = {
    0, 0, -1, 0, 0,
    0, -1, -1, -1, 0,
    -1, -1, 13, -1, -1,
    0, -1, -1, -1, 0,
    0, 0, -1, 0, 0};

int emboss3x3[9] = {
    -2, -1, 0,
    -1, 1, 1,
    0, 1, 2};

int emboss5x5[25] = {
    -2, -1, 0, 1, 2,
    -1, -1, 0, 1, 1,
    0, 0, 1, 0, 0,
    1, 1, 0, -1, -1,
    2, 1, 0, -1, -2};

/* Initialize Kernels Array */
KernelInfo kernels[] = {
    {edgeDetection3x3, 3}, // Edge Detection 3x3
    {edgeDetection5x5, 5}, // Edge Detection 5x5
    {blur3x3, 3},          // Blur 3x3
    {blur5x5, 5},          // Blur 5x5
    {sharpen3x3, 3},       // Sharpen 3x3
    {sharpen5x5, 5},       // Sharpen 5x5
    {emboss3x3, 3},        // Emboss 3x3
    {emboss5x5, 5}         // Emboss 5x5
};