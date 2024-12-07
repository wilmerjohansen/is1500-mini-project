extern void print(const char*);
extern void print_dec(int x);
extern void print_hex32(unsigned int x);
extern void print_hex8(unsigned char x);
void clear_counters();

void clear_counters() {
    // Clear all counters (mcycle, minstret, mhpmcounterX, etc.)
    asm volatile ("csrw mcycle, x0");      // Clear clock cycle counter
    asm volatile ("csrw minstret, x0");     // Clear instruction counter
    asm volatile ("csrw mhpmcounter3, x0"); // Clear memory instruction counter
    asm volatile ("csrw mhpmcounter4, x0"); // Clear I-cache miss counter
    asm volatile ("csrw mhpmcounter5, x0"); // Clear D-cache miss counter
    asm volatile ("csrw mhpmcounter6, x0"); // Clear I-cache stall counter
    asm volatile ("csrw mhpmcounter7, x0"); // Clear D-cache stall counter
    asm volatile ("csrw mhpmcounter8, x0"); // Clear data hazard stall counter
    asm volatile ("csrw mhpmcounter9, x0"); // Clear ALU stall counter
}

void read_counters() {
    unsigned int mcycle, minstret, mhpmcounter3, mhpmcounter4, mhpmcounter5, mhpmcounter6, mhpmcounter7, mhpmcounter8, mhpmcounter9;

    // Read values from performance counters
    asm volatile ("csrr %0, mcycle" : "=r"(mcycle));                // Read clock cycle counter
    asm volatile ("csrr %0, minstret" : "=r"(minstret));             // Read instruction counter
    asm volatile ("csrr %0, mhpmcounter3" : "=r"(mhpmcounter3));     // Read memory instruction counter
    asm volatile ("csrr %0, mhpmcounter4" : "=r"(mhpmcounter4));     // Read I-cache miss counter
    asm volatile ("csrr %0, mhpmcounter5" : "=r"(mhpmcounter5));     // Read D-cache miss counter
    asm volatile ("csrr %0, mhpmcounter6" : "=r"(mhpmcounter6));     // Read I-cache stall counter
    asm volatile ("csrr %0, mhpmcounter7" : "=r"(mhpmcounter7));     // Read D-cache stall counter
    asm volatile ("csrr %0, mhpmcounter8" : "=r"(mhpmcounter8));     // Read data hazard stall counter
    asm volatile ("csrr %0, mhpmcounter9" : "=r"(mhpmcounter9));     // Read ALU stall counter

    print("\nmcycle: ");
    print_dec(mcycle);
    print("\nminstret: ");
    print_dec(minstret);
    print("\nmhpmcounter3: ");
    print_dec(mhpmcounter3);
    print("\nmhpmcounter4: ");
    print_dec(mhpmcounter4);
    print("\nmhpmcounter5: ");
    print_dec(mhpmcounter5);
    print("\nmhpmcounter6: ");
    print_dec(mhpmcounter6);
    print("\nmhpmcounter7: ");
    print_dec(mhpmcounter7);
    print("\nmhpmcounter8: ");
    print_dec(mhpmcounter8);
    print("\nmhpmcounter9: ");
    print_dec(mhpmcounter9);
}