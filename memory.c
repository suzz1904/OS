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
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){}
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt){}
