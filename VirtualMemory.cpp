#include "VirtualMemory.h"
#include "MemoryConstants.h"
#include "PhysicalMemory.h"

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