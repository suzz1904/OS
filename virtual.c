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

int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

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
    int small_last_access_ts = current_timestamp;
    int lru_page = -1;

    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].last_access_timestamp < small_last_access_ts) {
            small_last_access_ts = page_table[i].last_access_timestamp;
            lru_page = i;
        }
    }
    
    // Mark the lru as invalid
    page_table[lru_page].is_valid = 0;
    int frame = page_table[lru_page].frame_number;
    page_table[lru_page].frame_number = -1;
    page_table[lru_page].arrival_timestamp = -1;
    page_table[lru_page].last_access_timestamp = -1;
    page_table[lru_page].reference_count = -1;

    // Set the new page
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

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
    int small_ref = INT_MAX;
    int lfu_page = -1;

    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].reference_count < small_ref) {
            small_ref = page_table[i].reference_count;
            lfu_page = i;
        }
    }
    
    // Mark the lru as invalid
    page_table[lfu_page].is_valid = 0;
    int frame = page_table[lfu_page].frame_number;
    page_table[lfu_page].frame_number = -1;
    page_table[lfu_page].arrival_timestamp = -1;
    page_table[lfu_page].last_access_timestamp = -1;
    page_table[lfu_page].reference_count = -1;

    // Set the new page
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}


int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, 
                           int reference_string[REFERENCEMAX], int reference_cnt, 
                           int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;       // To track the number of page faults
    int current_time = 1;      // Start the simulated time at 1

    for (int i = 0; i < reference_cnt; i++) {
        // Increment the timestamp at the start of processing a new page access
        current_time++;  

        int page_number = reference_string[i];  // Current page to be referenced

        if (page_table[page_number].is_valid) {
            // Page is already in memory (valid), just update the last access timestamp
            page_table[page_number].last_access_timestamp = current_time;
            page_table[page_number].reference_count++;  // Increment the reference count
        } else {
            // Page fault occurs
            page_faults++;

            // If free frames are available, allocate one from the frame pool
            if (frame_cnt > 0) {
                int frame = frame_pool[--frame_cnt];  // Get a frame from the pool
                page_table[page_number].frame_number = frame;
                page_table[page_number].is_valid = 1;
                page_table[page_number].arrival_timestamp = current_time;  // Set the arrival time
                page_table[page_number].last_access_timestamp = current_time; // Set the last access time
                page_table[page_number].reference_count = 1;  // Set reference count
            } else {
                // No free frames available, need to replace a page using FIFO
                int oldest_page = -1;
                int oldest_time = current_time + 1;  // Any valid time will be smaller

                // Find the page with the oldest arrival timestamp (FIFO replacement)
                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid && page_table[j].arrival_timestamp < oldest_time) {
                        oldest_page = j;
                        oldest_time = page_table[j].arrival_timestamp;
                    }
                }

                // Replace the oldest page
                page_table[oldest_page].is_valid = 0;  // Invalidate the oldest page
                int freed_frame = page_table[oldest_page].frame_number;  // Get its frame
                page_table[oldest_page].frame_number = -1;  // Reset frame number

                // Load the new page into the freed frame
                page_table[page_number].frame_number = freed_frame;
                page_table[page_number].is_valid = 1;
                page_table[page_number].arrival_timestamp = current_time;  // Set the arrival time
                page_table[page_number].last_access_timestamp = current_time; // Set the last access time
                page_table[page_number].reference_count = 1;  // Reset reference count
            }
        }
    }

    return page_faults;
}


int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int reference_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int page_faults = 0;       // To track the number of page faults
    int current_time = 1;      // Start the simulated time at 1

    for (int i = 0; i < reference_cnt; i++) {
        // Increment the timestamp at the start of processing a new page access
        current_time++;  

        int page_number = reference_string[i];  // Current page to be referenced

        if (page_table[page_number].is_valid) {
            // Page is already in memory (valid), just update the last access timestamp
            page_table[page_number].last_access_timestamp = current_time;
            page_table[page_number].reference_count++;  // Increment the reference count
        } else {
            // Page fault occurs
            page_faults++;

            // If free frames are available, allocate one from the frame pool
            if (frame_cnt > 0) {
                int frame = frame_pool[--frame_cnt];  // Get a frame from the pool
                page_table[page_number].frame_number = frame;
                page_table[page_number].is_valid = 1;
                page_table[page_number].arrival_timestamp = current_time;  // Set the arrival time
                page_table[page_number].last_access_timestamp = current_time; // Set the last access time
                page_table[page_number].reference_count = 1;  // Set reference count
            } else {
                // No free frames available, need to replace a page using FIFO
                int lru_page = -1;
                int lru_time = current_time + 1;  // Any valid time will be smaller

                // Find the page with the lru arrival timestamp (FIFO replacement)
                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid && page_table[j].last_access_timestamp < lru_time) {
                        lru_page = j;
                        lru_time = page_table[j].last_access_timestamp;
                    }
                }

                // Replace the lru page
                page_table[lru_page].is_valid = 0;  // Invalidate the lru page
                int freed_frame = page_table[lru_page].frame_number;  // Get its frame
                page_table[lru_page].frame_number = -1;  // Reset frame number

                // Load the new page into the freed frame
                page_table[page_number].frame_number = freed_frame;
                page_table[page_number].is_valid = 1;
                page_table[page_number].arrival_timestamp = current_time;  // Set the arrival time
                page_table[page_number].last_access_timestamp = current_time; // Set the last access time
                page_table[page_number].reference_count = 1;  // Reset reference count
            }
        }
    }

    return page_faults;
}



int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int reference_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {}



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
//     int frame_cnt = 0;

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
