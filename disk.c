#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#include <math.h>


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



struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt) {
    if (*queue_cnt == 0) {
        return (struct RCB){0, 0, 0, 0, 0}; // NULLRCB
    }

    // Find the request with the earliest arrival time
    struct RCB earliest_request = request_queue[0];
    int earliest_index = 0;

    for (int i = 1; i < *queue_cnt; i++) {
        if (request_queue[i].arrival_timestamp < earliest_request.arrival_timestamp) {
            earliest_request = request_queue[i];
            earliest_index = i;
        }
    }

    // Remove the earliest request from the queue
    for (int i = earliest_index; i < *queue_cnt - 1; i++) {
        request_queue[i] = request_queue[i + 1]; // Shift left
    }
    (*queue_cnt)--; // Decrement the count of items in the queue

    return earliest_request;
}


struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
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

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt,int current_cylinder){
    if (*queue_cnt == 0) {
        return (struct RCB){0, 0, 0, 0, 0}; // NULLRCB
    }

    // Find the request with the earliest arrival time
    struct RCB earliest_request = request_queue[0];
    int earliest_index = 0;
    int closest = 999;

    for(int i = 1; i < *queue_cnt; i++) {
        if (abs(request_queue[i].cylinder - current_cylinder) < closest || (abs(request_queue[i].cylinder - current_cylinder) == closest && request_queue[i].arrival_timestamp < earliest_request.arrival_timestamp)) {
            closest = abs(request_queue[i].cylinder - current_cylinder);
            earliest_request = request_queue[i];
            earliest_index = i;
        }
    }

    // Remove the earliest request from the queue
    for (int i = earliest_index; i < *queue_cnt - 1; i++) {
        request_queue[i] = request_queue[i + 1]; // Shift left
    }
    (*queue_cnt)--; // Decrement the count of items in the queue

    return earliest_request;
}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
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

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],int  *queue_cnt, int current_cylinder, int scan_direction){
    
}
