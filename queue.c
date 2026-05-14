/*
 * queue.c
 * -------
 * Queue / Waitlist Management — full implementation.
 *
 * Uses a fixed-size circular (ring) queue so enqueue/dequeue
 * are both O(1) with no dynamic memory allocation.
 *
 * Circular index arithmetic:
 *   rear  = (rear  + 1) % MAX_QUEUE_SIZE   on enqueue
 *   front = (front + 1) % MAX_QUEUE_SIZE   on dequeue
 *
 * Parking Management System
 */

#include <string.h>
#include "../include/queue.h"
#include "../include/utils.h"

/* ═══════════════════════════════════════════════════════════════
 * initQueue
 *
 * Resets all fields to their empty-queue defaults.
 * Must be called once at program startup before any other
 * queue operation.
 * ═══════════════════════════════════════════════════════════════ */
void initQueue(Queue *q)
{
    q->front = 0;
    q->rear  = 0;
    q->size  = 0;
    memset(q->nodes, 0, sizeof(q->nodes));
}

/* ═══════════════════════════════════════════════════════════════
 * isQueueEmpty
 * Returns TRUE when no vehicles are waiting.
 * ═══════════════════════════════════════════════════════════════ */
Bool isQueueEmpty(const Queue *q)
{
    return (q->size == 0) ? TRUE : FALSE;
}

/* ═══════════════════════════════════════════════════════════════
 * isQueueFull
 * Returns TRUE when the queue has reached MAX_QUEUE_SIZE.
 * ═══════════════════════════════════════════════════════════════ */
Bool isQueueFull(const Queue *q)
{
    return (q->size == MAX_QUEUE_SIZE) ? TRUE : FALSE;
}

/* ═══════════════════════════════════════════════════════════════
 * enqueue
 *
 * Adds a waiting vehicle to the rear of the circular queue.
 *
 * Steps:
 *   1. Reject if queue is full.
 *   2. Write QueueNode at nodes[rear].
 *   3. Advance rear circularly.
 *   4. Increment size.
 *
 * Returns: 1 on success, 0 if the queue is full.
 * ═══════════════════════════════════════════════════════════════ */
int enqueue(Queue *q, const char *plate, const char *owner,
            VehicleType type)
{
    QueueNode *node;

    if (isQueueFull(q)) {
        printf("  [ERROR] Waiting queue is full (%d/%d)."
               " Cannot add vehicle \"%s\".\n",
               q->size, MAX_QUEUE_SIZE, plate);
        return 0;
    }

    node = &q->nodes[q->rear];
    memset(node, 0, sizeof(QueueNode));

    strncpy(node->plate,     plate, PLATE_LEN - 1);
    strncpy(node->ownerName, owner, NAME_LEN  - 1);
    node->plate[PLATE_LEN - 1]     = '\0';
    node->ownerName[NAME_LEN - 1]  = '\0';

    toUpperCase(node->plate);
    trimWhitespace(node->ownerName);

    node->type        = type;
    node->arrivalTime = time(NULL);

    /* Advance rear circularly */
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->size++;

    return 1;
}

/* ═══════════════════════════════════════════════════════════════
 * dequeue
 *
 * Removes the front vehicle from the queue and copies it into
 * the caller-supplied *out node.
 *
 * Steps:
 *   1. Reject if queue is empty.
 *   2. Copy nodes[front] into *out.
 *   3. Zero the vacated slot.
 *   4. Advance front circularly.
 *   5. Decrement size.
 *
 * Returns: 1 on success, 0 if the queue is empty.
 * ═══════════════════════════════════════════════════════════════ */
int dequeue(Queue *q, QueueNode *out)
{
    if (isQueueEmpty(q)) {
        printf("  [INFO] Waiting queue is empty. No vehicle to dequeue.\n");
        return 0;
    }

    /* Copy the front node to the caller's buffer */
    *out = q->nodes[q->front];

    /* Clear the vacated slot */
    memset(&q->nodes[q->front], 0, sizeof(QueueNode));

    /* Advance front circularly */
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;

    return 1;
}

/* ═══════════════════════════════════════════════════════════════
 * peekQueue
 *
 * Returns a pointer to the front QueueNode without removing it.
 * Useful for checking the next vehicle's type before deciding
 * whether to dequeue (e.g., does the freed slot match its type?).
 *
 * Returns: pointer to front node, or NULL if queue is empty.
 * ═══════════════════════════════════════════════════════════════ */
QueueNode *peekQueue(const Queue *q)
{
    if (isQueueEmpty(q))
        return NULL;

    /*
     * Cast away const so we can return a non-const pointer.
     * The caller must not modify the node through this pointer.
     */
    return (QueueNode *)&q->nodes[q->front];
}

/* ═══════════════════════════════════════════════════════════════
 * waitingSeconds
 *
 * Calculates how many seconds a vehicle has been in the queue
 * by comparing its arrivalTime against the current wall clock.
 * ═══════════════════════════════════════════════════════════════ */
double waitingSeconds(const QueueNode *node)
{
    return difftime(time(NULL), node->arrivalTime);
}

/* ═══════════════════════════════════════════════════════════════
 * displayQueue
 *
 * Prints all waiting vehicles in FIFO order (front → rear).
 * Shows position, plate, owner, type, arrival time, and how
 * long each vehicle has been waiting.
 *
 * Iterates circularly: index = (front + i) % MAX_QUEUE_SIZE
 * ═══════════════════════════════════════════════════════════════ */
void displayQueue(const Queue *q)
{
    int  i, idx;
    char arrivalBuf[24];
    long waitMin, waitSec;

    if (isQueueEmpty(q)) {
        printf("\n  No vehicles are currently waiting.\n\n");
        return;
    }

    printf("\n");
    printDivider(74);
    printf("  %-4s  %-12s  %-22s  %-12s  %-10s  %s\n",
           "POS", "PLATE", "OWNER", "TYPE", "ARRIVED", "WAITED");
    printDivider(74);

    for (i = 0; i < q->size; i++) {
        idx = (q->front + i) % MAX_QUEUE_SIZE;
        const QueueNode *node = &q->nodes[idx];

        double secs = waitingSeconds(node);
        waitMin = (long)(secs / 60.0);
        waitSec = (long)secs % 60;

        formatTimestamp(node->arrivalTime, arrivalBuf, sizeof(arrivalBuf));

        printf("  %-4d  %-12s  %-22s  %-12s  %-10s  %ldm %02lds\n",
               i + 1,
               node->plate,
               node->ownerName,
               (node->type == TWO_WHEELER) ? "2-Wheeler" : "4-Wheeler",
               arrivalBuf + 11,   /* show only HH:MM:SS part        */
               waitMin, waitSec);
    }

    printDivider(74);
    printf("  Vehicles waiting: %d / %d\n\n", q->size, MAX_QUEUE_SIZE);
}
