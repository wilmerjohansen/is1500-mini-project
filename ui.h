/* Memory-mapped I/O addresses */
#define SWITCHES_BASE_ADDRESS 0x4000010
#define BUTTON_BASE_ADDRESS 0x40000d0
#define LEDS_BASE_ADDRESS 0x4000000

extern void print(const char*);
extern void print_dec(int x);
extern void print_hex32(unsigned int x);
extern void print_hex8(unsigned char x);

/* Define kernel options */
typedef enum {
    KERNEL_EDGE_3x3 = 1,
    KERNEL_EDGE_5x5,
    KERNEL_BLUR_3x3,
    KERNEL_BLUR_5x5,
    KERNEL_SHARPEN_3x3,
    KERNEL_SHARPEN_5x5,
    KERNEL_EMBOSS_3x3,
    KERNEL_EMBOSS_5x5,
} KernelOption;

/* Function prototypes */
void display_menu(int none_allowed, int current_selection, unsigned int size, int width, int height, unsigned short colorDepth);
KernelOption get_user_selection();
KernelOption get_user_chosen();
void cls();

void cls() {
    print("\033[2J");
    print("\033[H");
}

/* Display the menu */
void display_menu(int none_allowed, int current_selection, unsigned int size, int width, int height, unsigned short colorDepth) {
    cls();

    print("BMP file loaded with\n");

    print("Size: ");
    print_dec(size);
    print("\n");

    print("Width: ");
    print_dec(width);
    print("\n");

    print("Height: ");
    print_dec(height);
    print("\n");

    print("Color depth: ");
    print_dec(colorDepth);
    print("\n");

    const char* kernel_names[] = {
        "None (second choice)",
        "Edge Detection 3x3",
        "Edge Detection 5x5",
        "Blur 3x3",
        "Blur 5x5",
        "Sharpen 3x3",
        "Sharpen 5x5",
        "Emboss 3x3",
        "Emboss 5x5"
    };

    if (none_allowed == 0) {
        print("\n--- Kernel Selection Menu ---\n");
        print("--- Pick the first kernel ---\n");
    } else {
        print("\n--- Kernel Selection Menu ---\n");
        print("--- Pick the second kernel --\n");
    }

    for (int i = 0; i < sizeof(kernel_names) / sizeof(kernel_names[0]); i++) {
        if (none_allowed == 0 && i == 0)
            continue;
        if (i + 1 == current_selection) {
            print("<"); // Highlight the current selection
        } else {
            print(" ");
        }
        print(kernel_names[i]);
        if (i + 1 == current_selection)
            print(">"); // Highlight the current selection
        print("\n");
    }
    print("----------------------------\n");
}

/* Get user selection */
KernelOption get_user_selection() {
    volatile unsigned int* switches = (unsigned int*)SWITCHES_BASE_ADDRESS;
    volatile unsigned int* leds = (unsigned int*)LEDS_BASE_ADDRESS;

    int selection = *switches & 0xF; // Read the lower 4 bits from switches (values 0-15)
    *leds = selection;              // Display the current selection on LEDs

    return selection; // No valid selection yet
}

KernelOption get_user_chosen() {
    volatile unsigned int* switches = (unsigned int*)SWITCHES_BASE_ADDRESS;
    volatile unsigned int* button = (unsigned int*)BUTTON_BASE_ADDRESS;

    int selection = *switches & 0xF; // Read the lower 4 bits from switches (values 0-15)

    if (*button & 0x1) {            // Button press confirms selection
        return (KernelOption)selection;
    }

    return 0; // No valid selection yet
}

int get_button_unpress() {
    volatile unsigned int* button = (unsigned int*)BUTTON_BASE_ADDRESS;

    if ((*button & 0x1) == 0)
        return 1;

    return 0;
}