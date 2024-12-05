#include <stdio.h>

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
void display_menu(int current_selection);
KernelOption get_user_selection();

/* Display the menu */
void display_menu(int current_selection) {
    const char* kernel_names[] = {
        "Edge Detection 3x3",
        "Edge Detection 5x5",
        "Blur 3x3",
        "Blur 5x5",
        "Sharpen 3x3",
        "Sharpen 5x5",
        "Emboss 3x3",
        "Emboss 5x5"
    };

    print("\n--- Kernel Selection Menu ---\n");
    for (int i = 0; i < sizeof(kernel_names) / sizeof(kernel_names[0]); i++) {
        if (i + 1 == current_selection) {
            print("> "); // Highlight the current selection
        } else {
            print("  ");
        }
        print(kernel_names[i]);
        print("\n");
    }
    print("----------------------------\n");
}

/* Get user selection */
KernelOption get_user_selection() {
    volatile unsigned int* switches = (unsigned int*)SWITCHES_BASE_ADDRESS;
    volatile unsigned int* button = (unsigned int*)BUTTON_BASE_ADDRESS;
    volatile unsigned int* leds = (unsigned int*)LEDS_BASE_ADDRESS;

    int selection = *switches & 0xF; // Read the lower 4 bits from switches (values 0-15)
    *leds = selection;              // Display the current selection on LEDs

    if (*button & 0x1) {            // Button press confirms selection
        return (KernelOption)selection;
    }

    return 0; // No valid selection yet
}
