#include "VirtualMemory.h"
#include "PhysicalMemory.h"


uint64_t get_table_value(uint64_t virtualAddress, int table_index)
{
    uint64_t mask = (1ULL << OFFSET_WIDTH) - 1; // Create a mask with the last OFFSET_WIDTH bits set to 1
    uint64_t shifted_addr = (virtualAddress >> OFFSET_WIDTH * (TABLES_DEPTH - table_index));
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


void put_in_frame_zeros(word_t frame_index){
    for(int i = 0; i < PAGE_SIZE; i++){
        PMwrite (frame_index * PAGE_SIZE + i, 0);
    }
}


void VMinitialize()
{
    put_in_frame_zeros(0);
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


word_t get_frame_dfs(word_t frame_not_to_evict, word_t curr_frame_index, int height, uint64_t *father_address_res, uint64_t father)
{
    if (height == TABLES_DEPTH)
    {
        return 0;
    }
    if (curr_frame_index != frame_not_to_evict && is_there_only_zero_in_frame(curr_frame_index))
    {
        return curr_frame_index;
    }

    word_t pointing_index;
    word_t found_frame_index = 0;
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        PMread(curr_frame_index*PAGE_SIZE + i, &pointing_index);
        if (pointing_index == 0)
        {
            continue;
        }
        found_frame_index = found_frame_index ? found_frame_index : get_frame_dfs(frame_not_to_evict, pointing_index, height + 1, father_address_res, curr_frame_index*PAGE_SIZE + i);
    }

    return found_frame_index;
}


void get_frame_max_point(word_t frame_not_to_evict, word_t curr_frame_index, uint64_t page_swapped_in, uint64_t curr_p,word_t *frame_res, uint64_t *res_p, uint64_t *max_val, uint64_t father, uint64_t *father_address_res, int offset, int height)
{
    if (height == TABLES_DEPTH - 1)
    {
        father = curr_frame_index*PAGE_SIZE;
    }
    else if (height == TABLES_DEPTH)
    {
        if (curr_frame_index != frame_not_to_evict)
        {
            uint64_t res = page_swapped_in > curr_p ? (page_swapped_in-curr_p) : (curr_p - page_swapped_in);
            uint64_t tot_res = (NUM_PAGES - res) < res ? (NUM_PAGES - res) : res;
            if (tot_res > *max_val)
            {
                *res_p = curr_p;
                *max_val = tot_res;
                *frame_res = curr_frame_index;
                *father_address_res = father + offset;
            }
        }
        return;
    }

    word_t pointing_index;
    word_t found_frame_index = 0;
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        PMread(curr_frame_index*PAGE_SIZE + i, &pointing_index);
        if (pointing_index == 0)
        {
            continue;
        }
        get_frame_max_point(frame_not_to_evict, pointing_index, page_swapped_in, (curr_p << OFFSET_WIDTH) + i, frame_res, res_p, max_val, father, father_address_res, i, height + 1);
    }
}

void get_max_frame_index(word_t curr_frame_index ,word_t *cur_max_frame_index, int height){

    word_t cur_value;  if (height == TABLES_DEPTH - 1){
        for(int i = 0;i < PAGE_SIZE; i++){
            PMread (curr_frame_index*PAGE_SIZE + i, &cur_value);
            if (cur_value > *cur_max_frame_index)
            {
                *cur_max_frame_index = cur_value;
            }
        }
        return;  }

    for(int i = 0; i < PAGE_SIZE; i++){
        PMread (curr_frame_index*PAGE_SIZE + i, &cur_value);      if (cur_value > *cur_max_frame_index){
            *cur_max_frame_index = cur_value;      }
        get_max_frame_index (cur_value, cur_max_frame_index, height + 1);  }
}



int get_num_of_frames(word_t curr_frame_index, int height)
{
    if (height == TABLES_DEPTH)
    {
        return 1;
    }
    if (is_there_only_zero_in_frame(curr_frame_index))
    {
        return 0;
    }

    word_t pointing_index;
    int total_num = 0;
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        PMread(curr_frame_index*PAGE_SIZE + i, &pointing_index);

        if (pointing_index == 0){
            continue;
        }
        int res = get_num_of_frames(pointing_index, height + 1);
        if (res == 0)
        {
            continue;
        }

        total_num += res;
    }

    return total_num;
}

int get_num_of_frames(word_t curr_frame_index, int height, int* max)
{
    if (height == TABLES_DEPTH)
    {
        if (curr_frame_index > *max)
        {
            *max = curr_frame_index;
        }
        return 1;
    }
    if (is_there_only_zero_in_frame(curr_frame_index))
    {
        return 0;
    }

    word_t pointing_index;
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        PMread(curr_frame_index*PAGE_SIZE + i, &pointing_index);
        if (pointing_index == 0){
            continue;
        }

        if (pointing_index > *max)
        {
            *max = pointing_index;
        }

        int res = get_num_of_frames(pointing_index, height + 1);
        if (res == 0)
        {
            continue;
        }


    }
}


word_t get_frame(int is_next_data, uint64_t page_index, word_t frame_not_to_evict){
    //first option
    uint64_t father_res = 0;
    uint64_t father;
    word_t found_frame = get_frame_dfs(frame_not_to_evict, 0, 0, &father_res, father);

    if (found_frame != 0)
    {

        PMwrite(father_res, 0); //deleting parent
        if (is_next_data)
        {
            PMrestore(found_frame, page_index);
        }
        else
        {
            put_in_frame_zeros(found_frame);
        }

        return found_frame;
    }

    //second option
    int num_frames = 0;
    get_max_frame_index(0, &num_frames, 0);
    if (num_frames+1 < NUM_FRAMES) // todo: check why not add + 1
    {
        found_frame = num_frames + 1;

        if (is_next_data)
        {
            PMrestore(found_frame, page_index);
        }
        else
        {
            put_in_frame_zeros(found_frame);
        }

        return found_frame;
    }


    uint64_t max_val = 0;
    word_t frame_res = 0;
    uint64_t res_p = 0;
    get_frame_max_point(frame_not_to_evict, 0, page_index, 0, &frame_res, &res_p, &max_val, father, &father_res, 0, 0);

    PMevict(frame_res, res_p);


    PMwrite(father_res, 0); //writing 0 to father
    if (is_next_data)
    {
        PMrestore(frame_res, page_index);
    }
    else
    {
        put_in_frame_zeros(frame_res);
    }

    return frame_res;
}

word_t get_physical_page(uint64_t virtualAddress){
  uint64_t cur_part_of_vir_addr;
  word_t cur_addr;
  word_t last_addr = 0;
  for(int i = 0; i < TABLES_DEPTH; i++){
      cur_part_of_vir_addr = get_table_value (virtualAddress, i);
      PMread (last_addr * PAGE_SIZE + cur_part_of_vir_addr, &cur_addr);
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

