#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"

#define QUEUEMAX 100 

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], 
                                        int *queue_cnt, 
                                        struct RCB current_request, 
                                        struct RCB new_request, 
                                        int timestamp) {

    if (current_request.request_id == 0) { // NULLRCB condition
        // The disk is free, so accept the new request
        request_queue[0] = new_request; // Place new request at the front
        *queue_cnt = 1; // Update the queue count
        return new_request; // Return the newly-arriving request
    } else {
        // The disk is busy, so add the new request to the queue
        if (*queue_cnt < QUEUEMAX) { // Ensure there is space in the queue
            request_queue[*queue_cnt] = new_request; // Add to the end of the queue
            (*queue_cnt)++; // Increment the count of items in the queue
        } else {
            printf("Request queue is full! Cannot add request ID: %d\n", new_request.request_id);
        }
        return current_request; // Return the currently-serviced request
    }
}



struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt) {}


struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt,int current_cylinder){}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){}

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],int  *queue_cnt, int current_cylinder, int scan_direction){}
