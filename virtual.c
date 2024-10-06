#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
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
     if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // Page is not in memory, check if there are free frames
    if (*frame_cnt > 0) {
        int frame = frame_pool[--(*frame_cnt)];
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame;
    }

    // No free frames, need to replace a page
    int oldest_timestamp = current_timestamp;
    int oldest_page = -1;

    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].arrival_timestamp < oldest_timestamp) {
            oldest_timestamp = page_table[i].arrival_timestamp;
            oldest_page = i;
        }
    }

    // Mark the oldest page as invalid
    page_table[oldest_page].is_valid = 0;
    int frame = page_table[oldest_page].frame_number;
    page_table[oldest_page].frame_number = -1;
    page_table[oldest_page].arrival_timestamp = -1;
    page_table[oldest_page].last_access_timestamp = -1;
    page_table[oldest_page].reference_count = -1;

    // Set the new page
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int reference_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int page_faults = 0;
    int timestamp = 1;
    int frame_index = 0;

        for (int i = 0; i < reference_cnt; i++) {
            int page = reference_string[i];
            
            if (!page_table[page].is_valid) {
                page_faults++;

                if (frame_index < frame_cnt) {
                    // Allocate a new frame
                    page_table[page].frame_number = frame_pool[frame_index++];
                } else {
                    // Find the oldest page to replace
                    int oldest_timestamp = INT_MAX;
                    int oldest_page = -1;
                    
                    for (int j = 0; j < table_cnt; j++) {
                        if (page_table[j].is_valid && page_table[j].arrival_timestamp < oldest_timestamp) {
                            oldest_timestamp = page_table[j].arrival_timestamp;
                            oldest_page = j;
                        }
                    }

                    // Invalidate the oldest page
                    page_table[oldest_page].is_valid = false;
                    page_table[oldest_page].frame_number = -1;
                    page_table[oldest_page].arrival_timestamp = -1;
                    page_table[oldest_page].last_access_timestamp = -1;
                    page_table[oldest_page].reference_count = -1;

                    // Assign the frame to the new page
                    page_table[page].frame_number = page_table[oldest_page].frame_number;
                }

                page_table[page].is_valid = true;
                page_table[page].arrival_timestamp = timestamp;
                page_table[page].reference_count = 1;
            } else {
                page_table[page].reference_count++;
            }

            page_table[page].last_access_timestamp = timestamp;
            timestamp++;
        }

        return page_faults;
}



int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



// int main() {
//     struct PTE page_table[TABLEMAX];
//     int table_cnt = 8;
//     int reference_string[] = {0, 3, 2, 6, 3, 4, 5, 2, 4, 5, 6};
//     int reference_cnt = sizeof(reference_string) / sizeof(reference_string[0]);
//     int frame_pool[] = {0, 1, 2};
//     int frame_cnt = sizeof(frame_pool) / sizeof(frame_pool[0]);

//     // Initialize page table
//     for (int i = 0; i < table_cnt; i++) {
//         page_table[i].is_valid = false;
//         page_table[i].frame_number = -1;
//         page_table[i].arrival_timestamp = -1;
//         page_table[i].last_access_timestamp = -1;
//         page_table[i].reference_count = -1;
//     }

//     int faults = count_page_faults_fifo(page_table, table_cnt, reference_string, reference_cnt, frame_pool, frame_cnt);

//     printf("Page faults: %d\n", faults);

//     return 0;
// }

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