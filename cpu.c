#include <stdio.h>
#include "oslabs.h"  // Include the correct header file with necessary declarations

#define NULLPCB (struct PCB) {0, 0, 0, 0, 0, 0, 0}


struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    if (current_process.process_id == 0) {
        // No process is currently running
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // New process has higher priority (lower priority value) than the current process
    if (new_process.process_priority < current_process.process_priority) {
        // Preempt the current process and add it to the ready queue
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        // Set the new process's execution times
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
    }

    // New process has lower priority or same priority as the current process
    // Add new process to the ready queue
    new_process.execution_starttime = 0;
    new_process.execution_endtime = 0;
    new_process.remaining_bursttime = new_process.total_bursttime;

    ready_queue[*queue_cnt] = new_process;
    (*queue_cnt)++;

    return current_process;
}


struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    // If no current process is running
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // If the current process has a shorter or equal remaining burst time, do not preempt
    if (current_process.remaining_bursttime <= new_process.total_bursttime) {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;

        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;

        return current_process;  // Continue executing the current process
    } else {
        // Preempt the current process and move it to the ready queue
        current_process.remaining_bursttime -= (timestamp - current_process.execution_starttime);
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;

        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        // Start executing the new process
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;  // Return the new process for execution
    }
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        
        if (new_process.total_bursttime < time_quantum) {
            new_process.execution_endtime = timestamp + new_process.total_bursttime;
        } else {
            new_process.execution_endtime = timestamp + time_quantum;
        }
        new_process.remaining_bursttime = new_process.total_bursttime;
        
        return new_process;
    }
    else {
        int remaining_time_quantum = current_process.execution_endtime - timestamp;

        if (remaining_time_quantum > 0) {
            new_process.execution_starttime = 0;
            new_process.execution_endtime = 0;
            new_process.remaining_bursttime = new_process.total_bursttime;

            ready_queue[*queue_cnt] = new_process;
            (*queue_cnt)++;

            return current_process;
        }
        else {
            if (current_process.remaining_bursttime > 0) {
                current_process.execution_starttime = 0;
                current_process.execution_endtime = 0;
                ready_queue[*queue_cnt] = current_process;
                (*queue_cnt)++;
            }

            new_process.execution_starttime = timestamp;
            if (new_process.total_bursttime < time_quantum) {
                new_process.execution_endtime = timestamp + new_process.total_bursttime;
            } else {
                new_process.execution_endtime = timestamp + time_quantum;
            }
            
            new_process.remaining_bursttime = new_process.total_bursttime;

            return new_process;
        }
    }
}
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    if (*queue_cnt == 0) {
        // Return a null PCB or handle empty queue case
        struct PCB nullpcb = {0};
        return nullpcb;
    }

    struct PCB next_process = ready_queue[0];

    // Remove the first process from the queue
    for (int i = 0; i < (*queue_cnt) - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    // Determine execution end time based on remaining burst time and time quantum
    int burst_time_to_use = (next_process.remaining_bursttime < time_quantum) ? next_process.remaining_bursttime : time_quantum;
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + burst_time_to_use;
    next_process.remaining_bursttime -= burst_time_to_use;

    // If there is remaining burst time, the process should go back to the ready queue
    if (next_process.remaining_bursttime > 0) {
        if (*queue_cnt < QUEUEMAX) {
            ready_queue[*queue_cnt] = next_process;
            (*queue_cnt)++;
        } else {
            // Handle queue overflow
            printf("Warning: Ready queue is full. Process %d not added back.\n", next_process.process_id);
        }
    } else {
        // Process has completed
        printf("Process %d completed at time %d\n", next_process.process_id, next_process.execution_endtime);
        printf("Total Burst Time: %d, Arrival Time: %d\n", next_process.total_bursttime, next_process.arrival_timestamp);
    }

    return next_process;
}
/*
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    struct PCB next_process = ready_queue[0];
    
    // Remove the first process from the queue
    for (int i = 0; i < (*queue_cnt) - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    // Determine execution end time based on remaining burst time and time quantum
    int burst_time_to_use = (next_process.remaining_bursttime < time_quantum) ? next_process.remaining_bursttime : time_quantum;
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + burst_time_to_use;
    next_process.remaining_bursttime -= burst_time_to_use;

    // If there is remaining burst time, the process should go back to the ready queue
    if (next_process.remaining_bursttime > 0) {
        ready_queue[*queue_cnt] = next_process;
        (*queue_cnt)++;
    }

    return next_process;
}
*/

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp ) {
	if (*queue_cnt ==0) {
		return NULLPCB;
	}
	// Find the shortest remaining burst time process in the ready queue
	int shortest_rem_proc = 0;
	for(int i =1; i < *queue_cnt; i++) {
		if (ready_queue[i].remaining_bursttime < ready_queue[shortest_rem_proc].remaining_bursttime) {
			shortest_rem_proc = i;
		}
	}
	
	struct PCB next_process = ready_queue[shortest_rem_proc];

	for(int i = shortest_rem_proc; i < (*queue_cnt) -1; i++) {
		ready_queue[i] = ready_queue[i+1];
	}
	(*queue_cnt)--;

	next_process.execution_starttime = timestamp;
	next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

	return next_process;
}

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    // Find the highest priority process in the ready queue
    int highest_priority_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].process_priority < ready_queue[highest_priority_index].process_priority) {
            highest_priority_index = i;
        }
    }

    struct PCB next_process = ready_queue[highest_priority_index];

    for (int i = highest_priority_index; i < (*queue_cnt) - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

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
