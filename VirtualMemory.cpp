#include "VirtualMemory.h"
#include "MemoryConstants.h"
#include "PhysicalMemory.h"
#include <iostream>



void print_bytes(uint64_t virtualAddress) {
    for (int i = VIRTUAL_ADDRESS_WIDTH-1; i >= 0; --i) {
        uint64_t bit = (virtualAddress >> i) & 1;
        std::cout << bit;
    }
    std::cout << std::endl;
}

uint64_t get_table_value(uint64_t virtualAddress, int table_index)
{

    uint64_t mask = (1ULL << OFFSET_WIDTH) - 1; // Create a mask with the last OFFSET_WIDTH bits set to 1
    uint64_t shifted_addr = (virtualAddress >> OFFSET_WIDTH * (TABLES_DEPTH - table_index));
//    std::cout << "shited ";
    print_bytes(shifted_addr);
    return  shifted_addr & mask; // Return the last OFFSET_WIDTH bits of the virtualAddress
}

uint64_t get_page_index(uint64_t virtualAddress)
{
    return  virtualAddress >> OFFSET_WIDTH; // Return the last OFFSET_WIDTH bits of the virtualAddress
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
    for(int i = 0; i < PAGE_SIZE; i++)
    {
        PMwrite(i, 0);
    }
}

bool is_there_only_zero_in_frame(word_t frame_index){
  word_t var;
  for(int i = 0; i < PAGE_SIZE; i++){
    PMread (frame_index * PAGE_SIZE + i, &var);
    if(var != 0){
      return false;
    }
  }
  return true;
}

void put_in_frame_zeros(word_t frame_index){
  for(int i = 0; i < PAGE_SIZE; i++){
      PMwrite (frame_index * PAGE_SIZE + i, 0);
  }
}


word_t get_frame(int is_next_data, uint64_t page_index, word_t frame_not_to_evict){

}

word_t get_physical_page(uint64_t virtualAddress){
  uint64_t cur_part_of_vir_addr;
  word_t cur_addr;
  word_t last_addr = 0;
  for(int i = 0; i < TABLES_DEPTH; i++){
      cur_part_of_vir_addr = get_table_value (virtualAddress, i);
      PMread (cur_part_of_vir_addr, &cur_addr);
      if(cur_addr == 0){
          cur_addr = get_frame(i == TABLES_DEPTH - 1 ? 1:0, get_page_index (virtualAddress), last_addr);
          PMwrite(last_addr * PAGE_SIZE + cur_part_of_vir_addr, cur_addr);
        }
      last_addr = cur_addr;
  }
  return cur_addr;
}

int VMwrite(uint64_t virtualAddress, word_t value)
{
  word_t cur_addr = get_physical_page(virtualAddress);
  PMwrite(cur_addr * PAGE_SIZE + get_table_value (virtualAddress, TABLES_DEPTH), value);
  return 1;
}


/* reads a word from the given virtual address
 * and puts its content in *value.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMread(uint64_t virtualAddress, word_t* value){

  word_t cur_addr = get_physical_page (virtualAddress);
  PMread (cur_addr * PAGE_SIZE + get_table_value (virtualAddress, TABLES_DEPTH), value);
  return 1;
}


int main(int argc, char **argv) {
    uint64_t n1 = 100 + (uint64_t)(1ULL << 18);
    print_bytes(n1);
    std:: cout << "Table depth " << TABLES_DEPTH << std::endl;
    std:: cout << "without offset" << get_page_index (n1) << std::endl;
    std:: cout << "table_index 0 of" << n1 << ": " << get_table_value(n1, 0) << std::endl;
    std:: cout << "table_index 1 of" << n1 << ": " << get_table_value(n1, 1) << std::endl;
    std:: cout << "table_index 2 of" << n1 << ": " << get_table_value(n1, 2) << std::endl;
    std:: cout << "table_index 3 of" << n1 << ": " << get_table_value(n1, 3) << std::endl;
    std:: cout << "table_index 4 of" << n1 << ": " << get_table_value(n1, 4) << std::endl;

    return 0;
}