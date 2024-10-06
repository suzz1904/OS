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

void initialize_fifo_queue(struct PTE page_table[], int table_cnt, 
                           int frame_queue[], int *queue_end) {
    // Temporary arrays to hold page numbers and their arrival timestamps
    int pages[TABLEMAX];
    int ats[TABLEMAX];
    int count = 0;

    // Collect all valid pages
    for (int i = 0; i < table_cnt; i++) {
        if (page_table[i].is_valid) {
            pages[count] = i;
            ats[count] = page_table[i].arrival_timestamp;
            count++;
        }
    }

    // Simple selection sort based on arrival_timestamp
    for (int i = 0; i < count - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < count; j++) {
            if (ats[j] < ats[min_idx]) {
                min_idx = j;
            }
        }
        // Swap pages
        if (min_idx != i) {
            int temp_page = pages[i];
            int temp_ats = ats[i];
            pages[i] = pages[min_idx];
            ats[i] = ats[min_idx];
            pages[min_idx] = temp_page;
            ats[min_idx] = temp_ats;
        }
    }

    // Enqueue sorted pages into frame_queue
    for (int i = 0; i < count; i++) {
        frame_queue[*queue_end] = pages[i];
        (*queue_end)++;
    }
}

int count_page_faults_fifo(struct PTE page_table[], int table_cnt, 
                           int reference_string[], int reference_cnt, 
                           int frame_pool[], int frame_cnt) {
    int page_faults = 0;
    int current_time = 1;
    int free_frame_index = 0; // Points to the next free frame in the pool

    // Initialize frame pool FIFO queue
    int frame_queue[POOLMAX]; // Store the page numbers currently in memory (FIFO)
    int queue_start = 0, queue_end = 0; // FIFO queue pointers

    // Initialize FIFO queue with preloaded pages
    initialize_fifo_queue(page_table, table_cnt, frame_queue, &queue_end);

    // If frame_pool is not empty, enqueue free frames
    // Not necessary in this specific test case as frame_cnt=0, but kept for completeness
    // Not needed for FIFO policy, frames are managed via page_table and frame_queue

    for (int i = 0; i < reference_cnt; i++) {
        int page_num = reference_string[i];

        // Check if the page is in memory (valid bit is true)
        if (page_table[page_num].is_valid) {
            // Page is already in memory, update last access timestamp and reference count
            page_table[page_num].last_access_timestamp = current_time;
            page_table[page_num].reference_count++;
        } else {
            // Page fault
            page_faults++;

            if (free_frame_index < frame_cnt) {
                // There's a free frame available
                int free_frame = frame_pool[free_frame_index++];
                page_table[page_num].frame_number = free_frame;
                page_table[page_num].arrival_timestamp = current_time;
                page_table[page_num].last_access_timestamp = current_time;
                page_table[page_num].reference_count = 1;
                page_table[page_num].is_valid = true;

                // Add this page to the FIFO queue
                frame_queue[queue_end++] = page_num;

            } else if (queue_start < queue_end) {
                // No free frames, need to replace a page using FIFO
                int page_to_replace = frame_queue[queue_start++];
                
                // Mark the old page as invalid
                page_table[page_to_replace].is_valid = false;
                // Optionally, you can reset other fields if needed
                page_table[page_to_replace].arrival_timestamp = -1;
                page_table[page_to_replace].last_access_timestamp = -1;
                page_table[page_to_replace].reference_count = -1;

                // Use the freed frame for the new page
                int freed_frame = page_table[page_to_replace].frame_number;
                page_table[page_num].frame_number = freed_frame;
                page_table[page_num].arrival_timestamp = current_time;
                page_table[page_num].last_access_timestamp = current_time;
                page_table[page_num].reference_count = 1;
                page_table[page_num].is_valid = true;

                // Add the new page to the FIFO queue
                frame_queue[queue_end++] = page_num;
            } else {
                // Edge case: No pages to replace (should not occur if frame_count > 0)
                // Handle as needed, possibly by counting it as a fault without replacement
            }
        }
        // Increment the timestamp for the next page reference
        current_time++;
    }

    return page_faults;
}



int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {}
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



// int main() {
//     struct PTE page_table[TABLEMAX] = {
//         {false, -1, -1, -1, -1},
//         {false, -1, -1, -1, -1},
//         {true, 10, 3, 3, 1},
//         {false, -1, -1, -1, -1},
//         {false, -1, -1, -1, -1},
//         {true, 20, 2, 4, 2},
//         {false, -1, -1, -1, -1},
//         {true, 30, 1, 1, 1}
//     };

//     int table_cnt = 8;
//     int reference_string[] = {2, 5, 0, 7, 4, 2, 3, 5, 1, 2, 6, 0};
//     int reference_cnt = 12;
//     int frame_pool[POOLMAX] = {}; // Empty frame pool
//     int frame_cnt = 3;

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