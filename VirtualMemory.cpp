#include "VirtualMemory.h"
#include "MemoryConstants.h"
#include "PhysicalMemory.h"
#include <iostream>



void print_bytes(uint64_t virtualAddress) {
    for (int i = VIRTUAL_ADDRESS_WIDTH-1; i >= 0; --i) {
        uint64_t bit = (virtualAddress >> i) & 1;
        std::cout << bit;
    }
//    std::cout << std::endl;
}

uint64_t get_table_value(uint64_t virtualAddress, int table_index)
{
    uint64_t mask = (1ULL << OFFSET_WIDTH) - 1; // Create a mask with the last OFFSET_WIDTH bits set to 1
    uint64_t shifted_addr = (virtualAddress >> OFFSET_WIDTH * (TABLES_DEPTH - table_index));
//    std::cout << "shited ";
    print_bytes(shifted_addr);
    return  shifted_addr & mask; // Return the last OFFSET_WIDTH bits of the virtualAddress
}

uint64_t generateBitPattern(int offsetWidth) {
    // checked!
    uint64_t number = 0; // Initialize number to 0
    for (int i = 0; i < offsetWidth; ++i) {
        number |= (1ULL << i);
    }

    return number;
}


void VMinitialize()
{
    for(int i =0; i < PAGE_SIZE; i++)
    {
        PMwrite(i, 0);
    }
}



int VMwrite(uint64_t virtualAddress, word_t value)
{

}


//int main(int argc, char **argv) {
//    uint64_t n1 = 100 + (uint64_t)(1ULL << 18);
//    print_bytes(n1);
//    std:: cout << "Table depth " << TABLES_DEPTH << std::endl;
//    std:: cout << "table_index 0 of" << n1 << ": " << get_table_value(n1, 0) << std::endl;
//    std:: cout << "table_index 1 of" << n1 << ": " << get_table_value(n1, 1) << std::endl;
//    std:: cout << "table_index 2 of" << n1 << ": " << get_table_value(n1, 2) << std::endl;
//    std:: cout << "table_index 3 of" << n1 << ": " << get_table_value(n1, 3) << std::endl;
//    std:: cout << "table_index 4 of" << n1 << ": " << get_table_value(n1, 4) << std::endl;
//
//    return 0;
//}