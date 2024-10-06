#include <stdio.h>
#include "oslabs.h"

#define nullPTE (struct PTE){0,0,0,0,0}

/*  struct PTE {
        int is_valid;
        int frame_number;
        int arrival_timestamp;
        int last_access_timestamp;
        int reference_count;
    }

*/
int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    if(page_table[page_number].is_valid){
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;

        return page_table[page_number].frame_number;
    }

        if(*frame_cnt > 0){
            int free_frame = frame_pool[--(*frame_cnt)];

            page_table[page_number].is_valid = 1;
            page_table[page_number].frame_number = free_frame;
            page_table[page_number].arrival_timestamp = current_timestamp;
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count = 1;

            return free_frame;
        }
        int oldest_page = -1;
        int oldest_arrival = current_timestamp + 1;

        for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].arrival_timestamp < oldest_arrival) {
        oldest_arrival = page_table[i].arrival_timestamp;
        oldest_page = i;

        if (oldest_page != -1) {

        int replaced_frame = page_table[oldest_page].frame_number;
        page_table[oldest_page].is_valid = 0;
        page_table[oldest_page].frame_number = -1;
        page_table[oldest_page].arrival_timestamp = -1;
        page_table[oldest_page].last_access_timestamp = -1;
        page_table[oldest_page].reference_count = -1;


        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = replaced_frame;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;


        return replaced_frame;
        }

    return -1; 
        }
    return -1; 
        }
    return -1; 
}
int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}





// int main() {
//     // Initialize a page table and frame pool for testing
//     struct PTE page_table[TABLEMAX] = { {0} };
//     int frame_pool[POOLMAX] = { 0, 1, 2, 3, 4 };
//     int frame_cnt = 5;
//     int table_cnt = 8;
//     int current_timestamp = 12;

//     // Access a page (example)
//     int page_number = 0;
//     int frame_number = process_page_access_fifo(page_table, &table_cnt, page_number, frame_pool, &frame_cnt, current_timestamp);
    
//     printf("Frame number for page %d: %d\n", page_number, frame_number);
//     return 0;
// }