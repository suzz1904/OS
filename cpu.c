#include <stdio.h>
#include "oslabs.h"  

#define NULLPCB (struct PCB) {0, 0, 0, 0, 0, 0, 0}


struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    if (new_process.process_priority <  current_process.process_priority) {
        current_process.execution_endtime = 0;  
        current_process.remaining_bursttime -= (timestamp - current_process.execution_starttime);  

        ready_queue[*queue_cnt] = current_process;  
        (*queue_cnt)++;  

        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.remaining_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;  

        return new_process;  
    }
    else {
    new_process.execution_starttime = 0;  
    new_process.execution_endtime = 0;    
    new_process.remaining_bursttime = new_process.total_bursttime;  

    ready_queue[*queue_cnt] = new_process;
    (*queue_cnt)++;  

    return current_process;  
}




struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    if (current_process.remaining_bursttime <= new_process.total_bursttime) {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;

        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;

        return current_process;  
    } else {
        current_process.remaining_bursttime -= (timestamp - current_process.execution_starttime);
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;

        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;  
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
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum ) {
	if (*queue_cnt ==0) {
		return NULLPCB;
	}

    else {
	int earliest_arrival_time = 0;
	for(int i =1; i < *queue_cnt; i++) {
		if (ready_queue[i].arrival_timestamp < ready_queue[earliest_arrival_time].arrival_timestamp) {
			earliest_arrival_time = i;
		}
	}
	
	struct PCB next_process = ready_queue[earliest_arrival_time];

	for(int i = earliest_arrival_time; i < (*queue_cnt) -1; i++) {
		ready_queue[i] = ready_queue[i+1];
	}
	(*queue_cnt)--;

    int sum_time = 0;
    if (time_quantum <= next_process.remaining_bursttime) {
        sum_time = time_quantum;
    }
    else {
        sum_time = next_process.remaining_bursttime;
    }
	next_process.execution_starttime = timestamp;
	next_process.execution_endtime = timestamp + sum_time;

	return next_process;
    }
}

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp ) {
	if (*queue_cnt ==0) {
		return NULLPCB;
	}
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
// int main() {
//     struct PCB ready_queue[10];
//     int queue_size = 0;

//     // Example processes
//     struct PCB current_process = NULLPCB;
//     int timestamp = 0;
// //	struct PCB process1 = {1, 1, 4, 1, 5, 4, 8}; // Process 1 [PID:1, AT:1, TBT:4, EST:1, EET:5, RBT:4, Priority:8]
// //	struct PCB process2 = {2, 2, 3, 0, 0, 3, 6};    // Process 2 [PID:2, AT:2, TBT:3, EST:0, EET:0, RBT:3, Priority:6]
//     struct PCB process1 = {1, 0, 10, 0, 0, 10, 2};  // Process 1, Priority 1
//     struct PCB process2 = {2, 2, 5, 0, 0, 5, 1};   // Process 2, Priority 2
//     struct PCB process3 = {3, 3, 7, 0, 0, 7, 3};   // Process 3, Priority 0 (highest priority)
//     // Simulate arrival of processes
//     current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process1, timestamp);
//     printf("Current Process ID: %d\n", current_process.process_id);

//     timestamp += 1;
//     current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process2, timestamp);
//     printf("Current Process ID: %d\n", current_process.process_id);

//     timestamp += 2;
//     current_process = handle_process_arrival_pp(ready_queue, &queue_size, current_process, process3, timestamp);
//     printf("Current Process ID: %d\n", current_process.process_id);

//     // Simulate process completion
//     timestamp += 3;
//     current_process = handle_process_completion_pp(ready_queue, &queue_size, timestamp);
//     printf("Next Process ID: %d\n", current_process.process_id);

//     return 0;
// }
