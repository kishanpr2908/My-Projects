/*
 * queue.h
 * -------
 * QueueNode / Queue struct definitions and Queue Management declarations.
 * Used when all slots of a given type are full and a vehicle must wait.
 *
 * Parking Management System
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

/* ── Queue node ────────────────────────────────────────────────── */
/*
 * A single entry in the waiting queue.
 *
 * Fields:
 *   plate       - Licence plate of the waiting vehicle
 *   ownerName   - Owner name (for display purposes)
 *   type        - Vehicle type (determines which slot pool to wait for)
 *   arrivalTime - Unix timestamp when the vehicle joined the queue
 */
typedef struct {
    char        plate[PLATE_LEN];
    char        ownerName[NAME_LEN];
    VehicleType type;
    time_t      arrivalTime;
} QueueNode;

/* ── Circular queue structure ──────────────────────────────────── */
/*
 * Fixed-size circular (ring) queue.
 *
 * Fields:
 *   nodes   - Array of waiting vehicle entries
 *   front   - Index of the next vehicle to be dequeued
 *   rear    - Index where the next vehicle will be inserted
 *   size    - Current number of vehicles in the queue
 */
typedef struct {
    QueueNode nodes[MAX_QUEUE_SIZE];
    int       front;
    int       rear;
    int       size;
} Queue;

/* ── Function declarations ─────────────────────────────────────── */

/* Initialise an empty queue (called once at startup). */
void initQueue(Queue *q);

/* Add a vehicle to the rear of the queue.
   Returns 1 on success, 0 if the queue is full. */
int  enqueue(Queue *q,
             const char *plate, const char *owner,
             VehicleType type);

/* Remove and return the front vehicle from the queue.
   Writes the dequeued node into *out.
   Returns 1 on success, 0 if the queue is empty. */
int  dequeue(Queue *q, QueueNode *out);

/* Peek at the front vehicle without removing it.
   Returns a pointer to the front QueueNode, or NULL if empty. */
QueueNode *peekQueue(const Queue *q);

/* Returns TRUE if the queue is empty. */
Bool isQueueEmpty(const Queue *q);

/* Returns TRUE if the queue is full. */
Bool isQueueFull(const Queue *q);

/* Calculate how long (seconds) the front vehicle has been waiting. */
double waitingSeconds(const QueueNode *node);

/* Print all vehicles currently in the waiting queue. */
void displayQueue(const Queue *q);

#endif /* QUEUE_H */
