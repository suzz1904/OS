#include <stdio.h>
#include "oslabs.h"  // Include the correct header file with necessary declarations

#define NULLPCB (struct PCB) {0, 0, 0, 0, 0, 0, 0}

/*
// Function declarations (implement these functions based on your scheduling policies)
struct PCB handle_process_arrival_pp(struct PCB ready_queue[], int *queue_size, struct PCB current_process, struct PCB new_process, int timestamp);
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[], int *queue_size, struct PCB current_process, struct PCB new_process, int timestamp);
struct PCB handle_process_arrival_rr(struct PCB ready_queue[], int *queue_size, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum);
struct PCB handle_process_completion_pp(struct PCB ready_queue[], int *queue_size, int timestamp);
struct PCB handle_process_completion_srtp(struct PCB ready_queue[], int *queue_size, int timestamp);
struct PCB handle_process_completion_rr(struct PCB ready_queue[], int *queue_size, int timestamp, int time_quantum);
*/


struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    // If no current process is running (NULLPCB)
    if (current_process.process_id == 0) {
        // Set up the new process to run immediately
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // If the new process has a higher priority (lower priority value)
    if (new_process.process_priority > current_process.process_priority) {
        // Preempt the current process and put it in the ready queue
        current_process.remaining_bursttime -= (timestamp - current_process.execution_starttime);
        current_process.execution_endtime = 0;  // Mark as preempted

        // Add the current process to the ready queue
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        // Set up the new process to run immediately
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
    }

    // If the new process has equal or lower priority
    // Add the new process to the ready queue
    new_process.execution_starttime = 0;
    new_process.execution_endtime = 0;
    new_process.remaining_bursttime = new_process.total_bursttime;

    ready_queue[*queue_cnt] = new_process;
    (*queue_cnt)++;

    // Return the current process since it continues running
    return current_process;
}
struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    // If the ready queue is empty, return NULLPCB
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    // Find the highest priority process in the ready queue
    int highest_priority_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].process_priority > ready_queue[highest_priority_index].process_priority) {
            highest_priority_index = i;
        }
    }

    // Select the process to run next
    struct PCB next_process = ready_queue[highest_priority_index];

    // Remove the process from the ready queue
    for (int i = highest_priority_index; i < (*queue_cnt) - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    // Set the execution start time and end time for the next process
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}
/*
int main() {
    struct PCB ready_queue[10];
    int queue_size = 0;

    // Example processes
    struct PCB current_process = NULLPCB;
    int timestamp = 0;
    struct PCB process1 = {1, 0, 10, 0, 0, 10, 2};  // Process 1, Priority 1
    struct PCB process2 = {2, 2, 5, 0, 0, 5, 1};   // Process 2, Priority 2
    struct PCB process3 = {3, 3, 7, 0, 0, 7, 3};   // Process 3, Priority 0 (highest priority)

    // Simulate arrival of processes
    current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process1, timestamp);
    printf("Current Process ID: %d\n", current_process.process_id);

    timestamp += 2;
    current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process2, timestamp);
    printf("Current Process ID: %d\n", current_process.process_id);

    timestamp += 1;
    current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process3, timestamp);
    printf("Current Process ID: %d\n", current_process.process_id);

    // Simulate process completion
    timestamp += 3;
    current_process = handle_process_completion_pp(ready_queue, &queue_size, timestamp);
    printf("Next Process ID: %d\n", current_process.process_id);

    return 0;
}

*/
