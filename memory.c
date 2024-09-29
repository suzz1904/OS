#include "oslabs.h"
#include<stdio.h>


#define NULLBLOCK (struct MEMORY_BLOCK){0, 0, 0, 0}

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int best_index = -1; 
    int best_size_diff = __INT_MAX__; 

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            int size_diff = memory_map[i].segment_size - request_size;

            if (size_diff < best_size_diff) {
                best_index = i;
                best_size_diff = size_diff;
            }
        }
    }

    if (best_index == -1) {
        return NULLBLOCK;
    }

    if (memory_map[best_index].segment_size == request_size) {
        memory_map[best_index].process_id = process_id;
        return memory_map[best_index];
    }

    struct MEMORY_BLOCK allocated_block = memory_map[best_index];
    allocated_block.end_address = allocated_block.start_address + request_size - 1;
    allocated_block.segment_size = request_size;
    allocated_block.process_id = process_id;

    memory_map[best_index].start_address = allocated_block.end_address + 1;
    memory_map[best_index].segment_size -= request_size;

    for (int i = *map_cnt; i > best_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }
    memory_map[best_index] = allocated_block;
    (*map_cnt)++;

    return allocated_block;
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {

            if (memory_map[i].segment_size == request_size) {
                memory_map[i].process_id = process_id;
                return memory_map[i];
            }

            struct MEMORY_BLOCK allocated_block = memory_map[i];   
            allocated_block.end_address = allocated_block.start_address + request_size - 1; 
            allocated_block.segment_size = request_size;
            allocated_block.process_id = process_id;

            memory_map[i].start_address = allocated_block.end_address + 1;
            memory_map[i].segment_size -= request_size;

            for (int j = *map_cnt; j > i; j--) {
                memory_map[j] = memory_map[j - 1];
            }
            memory_map[i] = allocated_block;
            (*map_cnt)++;

            return allocated_block;

        }
        
        
    }

    return NULLBLOCK;    
}
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    int worst_index = -1; 
    int largest_block_size = 0; 

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
        if (memory_map[i].segment_size > largest_block_size) {
                largest_block_size = memory_map[i].segment_size;
                worst_index = i;
            }
        }
    }

    if (worst_index == -1) {
        return NULLBLOCK;
    }

    if (memory_map[worst_index].segment_size == request_size) {
        memory_map[worst_index].process_id = process_id;
        return memory_map[worst_index];
    }

    struct MEMORY_BLOCK allocated_block = memory_map[worst_index];
    allocated_block.end_address = allocated_block.start_address + request_size - 1;
    allocated_block.segment_size = request_size;
    allocated_block.process_id = process_id;

    memory_map[worst_index].start_address = allocated_block.end_address + 1;
    memory_map[worst_index].segment_size -= request_size;

    for (int i = *map_cnt; i > worst_index; i--) {
        memory_map[i] = memory_map[i - 1];
    }
    memory_map[worst_index] = allocated_block;
    (*map_cnt)++;

    return allocated_block;
}
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size && memory_map[i].start_address >= last_address) {
            if (memory_map[i].segment_size == request_size) {
                memory_map[i].process_id = process_id;
                last_address = memory_map[i].end_address;  
                return memory_map[i];
            }

            struct MEMORY_BLOCK allocated_block = memory_map[i];  
            allocated_block.end_address = allocated_block.start_address + request_size - 1;  
            allocated_block.segment_size = request_size;
            allocated_block.process_id = process_id;  

            last_address = allocated_block.end_address;  

            memory_map[i].start_address = allocated_block.end_address + 1;
            memory_map[i].segment_size -= request_size;

            for (int j = *map_cnt; j > i; j--) {
                memory_map[j] = memory_map[j - 1];
            }
            memory_map[i] = allocated_block;
            (*map_cnt)++;

            return allocated_block;  
        }
    }

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {

            if (memory_map[i].segment_size == request_size) {
                memory_map[i].process_id = process_id;
                last_address = memory_map[i].end_address;  
                return memory_map[i];
            }

            struct MEMORY_BLOCK allocated_block = memory_map[i]; 
            allocated_block.end_address = allocated_block.start_address + request_size - 1;  
            allocated_block.segment_size = request_size;
            allocated_block.process_id = process_id;  

            last_address = allocated_block.end_address; 
            memory_map[i].start_address = allocated_block.end_address + 1;
            memory_map[i].segment_size -= request_size;

            for (int j = *map_cnt; j > i; j--) {
                memory_map[j] = memory_map[j - 1];
            }
            memory_map[i] = allocated_block;
            (*map_cnt)++;

            return allocated_block;  
        }
    }

    return NULLBLOCK; 

}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt) {
    int block_index = -1;
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address == freed_block.start_address &&
            memory_map[i].end_address == freed_block.end_address &&
            memory_map[i].segment_size == freed_block.segment_size) {
            
            memory_map[i].process_id = 0; 
            block_index = i;
            break;
        }
    }

    if (block_index > 0 && memory_map[block_index - 1].process_id == 0) {
        memory_map[block_index - 1].end_address = memory_map[block_index].end_address;
        memory_map[block_index - 1].segment_size += memory_map[block_index].segment_size;

        for (int i = block_index; i < *map_cnt - 1; i++) {
            memory_map[i] = memory_map[i + 1];
        }
        (*map_cnt)--; 
        block_index--;  
    }

    if (block_index < *map_cnt - 1 && memory_map[block_index + 1].process_id == 0) {
        memory_map[block_index].end_address = memory_map[block_index + 1].end_address;
        memory_map[block_index].segment_size += memory_map[block_index + 1].segment_size;

        for (int i = block_index + 1; i < *map_cnt - 1; i++) {
            memory_map[i] = memory_map[i + 1];
        }
        (*map_cnt)--; 
    }
}
