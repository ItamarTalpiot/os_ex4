#include "VirtualMemory.h"
#include "MemoryConstants.h"
#include "PhysicalMemory.h"

uint64_t get_table_value(uint64_t virtualAddress, int table_index)
{

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