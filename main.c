/*
 * main.c
 * ------
 * Entry point — Parking Management System.
 * Menu-driven interface that wires all modules together.
 *
 * Menu options:
 *   1. Vehicle Entry
 *   2. Vehicle Exit
 *   3. View Slot Map
 *   4. View All Parked Vehicles
 *   5. View Waiting Queue
 *   6. Search Vehicle by Plate
 *   7. Revenue Report
 *   0. Save & Exit
 *
 * Parking Management System
 */

#include <ctype.h>
#include "../include/vehicle.h"
#include "../include/slots.h"
#include "../include/queue.h"
#include "../include/billing.h"
#include "../include/utils.h"

/* ═══════════════════════════════════════════════════════════════
 * Forward declarations of menu-action handlers
 * ═══════════════════════════════════════════════════════════════ */
static void handleVehicleEntry(Vehicle vehicles[], int *count,
                                ParkingSlot slots[], Queue *q);
static void handleVehicleExit(Vehicle vehicles[], int count,
                               ParkingSlot slots[], Queue *q);
static void handleSearchVehicle(Vehicle vehicles[], int count);
static void printBanner(void);
static void printMenu(const ParkingSlot slots[]);
static int  getMenuChoice(void);
static void readLine(const char *prompt, char *buf, int bufSize);
static int  getVehicleType(void);

/* ═══════════════════════════════════════════════════════════════
 * main
 * ═══════════════════════════════════════════════════════════════ */
int main(void)
{
    /* ── Runtime data stores ──────────────────────────────────── */
    Vehicle     vehicles[MAX_VEHICLES];
    ParkingSlot slots[MAX_TOTAL_SLOTS];
    Queue       waitQueue;
    int         vehicleCount = 0;
    int         choice;

    /* ── Initialise all subsystems ────────────────────────────── */
    initSlots(slots);
    initQueue(&waitQueue);
    vehicleCount = loadVehicles(vehicles);
    loadSlots(slots);

    /* ── Main menu loop ───────────────────────────────────────── */
    do {
        clearScreen();
        printBanner();
        printMenu(slots);

        choice = getMenuChoice();

        switch (choice) {

        /* ── 1. Vehicle Entry ─────────────────────────────────── */
        case 1:
            clearScreen();
            printDivider(60);
            printf("  VEHICLE ENTRY\n");
            printDivider(60);
            handleVehicleEntry(vehicles, &vehicleCount, slots, &waitQueue);
            pressEnterToContinue();
            break;

        /* ── 2. Vehicle Exit ──────────────────────────────────── */
        case 2:
            clearScreen();
            printDivider(60);
            printf("  VEHICLE EXIT\n");
            printDivider(60);
            handleVehicleExit(vehicles, vehicleCount, slots, &waitQueue);
            pressEnterToContinue();
            break;

        /* ── 3. Slot Map ──────────────────────────────────────── */
        case 3:
            clearScreen();
            printDivider(60);
            printf("  PARKING SLOT MAP\n");
            printDivider(60);
            displaySlotMap(slots);
            pressEnterToContinue();
            break;

        /* ── 4. All Parked Vehicles ───────────────────────────── */
        case 4:
            clearScreen();
            printDivider(60);
            printf("  CURRENTLY PARKED VEHICLES\n");
            printDivider(60);
            displayVehicles(vehicles, vehicleCount);
            pressEnterToContinue();
            break;

        /* ── 5. Waiting Queue ─────────────────────────────────── */
        case 5:
            clearScreen();
            printDivider(60);
            printf("  WAITING QUEUE\n");
            printDivider(60);
            if (isQueueEmpty(&waitQueue))
                printf("\n  No vehicles are currently waiting.\n\n");
            else
                displayQueue(&waitQueue);
            pressEnterToContinue();
            break;

        /* ── 6. Search Vehicle ────────────────────────────────── */
        case 6:
            clearScreen();
            printDivider(60);
            printf("  SEARCH VEHICLE\n");
            printDivider(60);
            handleSearchVehicle(vehicles, vehicleCount);
            pressEnterToContinue();
            break;

        /* ── 7. Revenue Report ────────────────────────────────── */
        case 7:
            clearScreen();
            printDivider(60);
            printf("  REVENUE REPORT\n");
            printDivider(60);
            showRevenueReport();
            pressEnterToContinue();
            break;

        /* ── 0. Exit ──────────────────────────────────────────── */
        case 0:
            clearScreen();
            printf("\n  Saving data...\n");
            saveVehicles(vehicles, vehicleCount);
            saveSlots(slots);
            printDivider(60);
            printf("  Thank you for using Parking Management System.\n");
            printf("  Goodbye!\n");
            printDivider(60);
            printf("\n");
            break;

        default:
            printf("\n  [!] Invalid choice. Please enter 0-7.\n");
            pressEnterToContinue();
            break;
        }

    } while (choice != 0);

    return 0;
}

/* ═══════════════════════════════════════════════════════════════
 * printBanner
 * Displays the ASCII art title header.
 * ═══════════════════════════════════════════════════════════════ */
static void printBanner(void)
{
    printf("\n");
    printDivider(60);
    printf("    ____  ___    ____  __ __ __  _   ______\n");
    printf("   / __ \\/ _ |  / __ \\/ //_//  / | / / ___/\n");
    printf("  / /_/ __ | / /_/ / ,<  / /| |/ / (_ /\n");
    printf("  \\____/_/ |_|/_____/_/|_|/_/ |___/\\___/\n");
    printf("\n");
    printf("     P A R K I N G   M A N A G E M E N T\n");
    printf("             S Y S T E M  v1.0\n");
    printDivider(60);
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════
 * printMenu
 * Displays the main menu with live slot availability counts.
 * ═══════════════════════════════════════════════════════════════ */
static void printMenu(const ParkingSlot slots[])
{
    int free2w = getAvailableCount(slots, TWO_WHEELER);
    int free4w = getAvailableCount(slots, FOUR_WHEELER);

    printf("  Slot Availability:  2-Wheelers: %d/%d free   "
           "4-Wheelers: %d/%d free\n",
           free2w, MAX_2W_SLOTS, free4w, MAX_4W_SLOTS);
    printf("\n");
    printDivider(60);
    printf("   [1]  Vehicle Entry\n");
    printf("   [2]  Vehicle Exit\n");
    printf("   [3]  View Slot Map\n");
    printf("   [4]  View All Parked Vehicles\n");
    printf("   [5]  View Waiting Queue\n");
    printf("   [6]  Search Vehicle by Plate\n");
    printf("   [7]  Revenue Report\n");
    printf("   [0]  Save & Exit\n");
    printDivider(60);
}

/* ═══════════════════════════════════════════════════════════════
 * getMenuChoice
 * Reads a single integer choice from the user safely.
 * ═══════════════════════════════════════════════════════════════ */
static int getMenuChoice(void)
{
    char buf[8];
    int  choice;

    printf("  Enter choice: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return -1;

    if (sscanf(buf, "%d", &choice) != 1)
        return -1;

    return choice;
}

/* ═══════════════════════════════════════════════════════════════
 * readLine
 * Prompts the user and reads a trimmed line into buf safely.
 * ═══════════════════════════════════════════════════════════════ */
static void readLine(const char *prompt, char *buf, int bufSize)
{
    printf("  %s: ", prompt);
    if (fgets(buf, bufSize, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    /* Strip trailing newline */
    buf[strcspn(buf, "\n")] = '\0';
    trimWhitespace(buf);
}

/* ═══════════════════════════════════════════════════════════════
 * getVehicleType
 * Prompts user to select vehicle type; returns VehicleType or -1.
 * ═══════════════════════════════════════════════════════════════ */
static int getVehicleType(void)
{
    char buf[8];
    int  t;

    printf("  Vehicle Type — [1] 2-Wheeler  [2] 4-Wheeler: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) return -1;
    if (sscanf(buf, "%d", &t) != 1)             return -1;
    if (t != 1 && t != 2)                        return -1;
    return t;
}

/* ═══════════════════════════════════════════════════════════════
 * handleVehicleEntry
 *
 * Workflow:
 *   1. Collect plate, owner, type from user.
 *   2. addVehicle() — registers in vehicle array.
 *   3. assignSlot() — tries to park immediately.
 *      If no slot: enqueue() — adds to waiting queue.
 *   4. Auto-drains queue whenever a slot is free
 *      (called after each successful assignment).
 * ═══════════════════════════════════════════════════════════════ */
static void handleVehicleEntry(Vehicle vehicles[], int *count,
                                ParkingSlot slots[], Queue *q)
{
    char plate[PLATE_LEN];
    char owner[NAME_LEN];
    int  typeChoice;
    int  idx;
    int  slotId;

    printf("\n");
    readLine("Licence Plate (e.g. MH12AB1234)", plate, PLATE_LEN);
    readLine("Owner Name",                      owner, NAME_LEN);

    typeChoice = getVehicleType();
    if (typeChoice == -1) {
        printf("\n  [ERROR] Invalid vehicle type selection.\n");
        return;
    }

    /* Register vehicle */
    idx = addVehicle(vehicles, count, plate, owner,
                     (VehicleType)typeChoice);
    if (idx == -1)
        return;   /* addVehicle already printed the error */

    /* Try to assign a slot immediately */
    slotId = assignSlot(slots, (VehicleType)typeChoice,
                        vehicles[idx].plate);

    if (slotId != -1) {
        /* Slot assigned */
        vehicles[idx].slotId = slotId;
        printf("\n  [OK] Vehicle %s parked at Slot S%02d.\n",
               vehicles[idx].plate, slotId + 1);
    } else {
        /* No slot available — add to waiting queue */
        if (isQueueFull(q)) {
            printf("\n  [ERROR] Parking lot and waiting queue are both full."
                   " Entry denied.\n");
            /* Roll back the vehicle registration */
            removeVehicle(vehicles, *count, vehicles[idx].plate);
            (*count)--;
            return;
        }
        vehicles[idx].entryTime = time(NULL);   /* queue start time */
        enqueue(q, vehicles[idx].plate, vehicles[idx].ownerName,
                (VehicleType)typeChoice);
        printf("\n  [QUEUE] No %s slot available."
               " Vehicle %s added to waiting queue (position %d).\n",
               (typeChoice == 1) ? "2-Wheeler" : "4-Wheeler",
               vehicles[idx].plate, q->size);
    }
}

/* ═══════════════════════════════════════════════════════════════
 * handleVehicleExit
 *
 * Workflow:
 *   1. Find vehicle by plate.
 *   2. Calculate fee and generate receipt.
 *   3. releaseSlot() — frees the physical slot.
 *   4. removeVehicle() — marks vehicle as exited.
 *   5. Drain queue: if a waiting vehicle matches the freed slot
 *      type, dequeue it and assign the now-free slot.
 * ═══════════════════════════════════════════════════════════════ */
static void handleVehicleExit(Vehicle vehicles[], int count,
                               ParkingSlot slots[], Queue *q)
{
    char      plate[PLATE_LEN];
    Vehicle  *v;
    BillingRecord rec;
    QueueNode  next;
    int        newSlot;
    double     durationSecs;
    time_t     exitNow;

    printf("\n");
    readLine("Enter Plate Number of Exiting Vehicle", plate, PLATE_LEN);
    toUpperCase(plate);

    v = findVehicle(vehicles, count, plate);
    if (!v) {
        printf("\n  [ERROR] No active vehicle found with plate \"%s\".\n",
               plate);
        return;
    }

    exitNow      = time(NULL);
    durationSecs = difftime(exitNow, v->entryTime);

    /* ── Build billing record ────────────────────────────────── */
    strncpy(rec.plate,     v->plate,     PLATE_LEN - 1);
    strncpy(rec.ownerName, v->ownerName, NAME_LEN  - 1);
    rec.plate[PLATE_LEN - 1]     = '\0';
    rec.ownerName[NAME_LEN - 1]  = '\0';
    rec.type        = v->type;
    rec.entryTime   = v->entryTime;
    rec.exitTime    = exitNow;
    rec.waitTime    = v->waitTime;
    rec.hoursParked = durationSecs / 3600.0;
    rec.totalFee    = calculateFee(v->type, durationSecs);

    /* ── Generate receipt & persist billing ──────────────────── */
    generateReceipt(&rec);
    saveBillingRecord(&rec);

    /* ── Free the parking slot ───────────────────────────────── */
    releaseSlot(slots, v->plate);

    /* ── Mark vehicle as exited ──────────────────────────────── */
    removeVehicle(vehicles, count, plate);

    /* ── Auto-assign freed slot to next waiting vehicle ─────── */
    if (!isQueueEmpty(q)) {
        /* Peek at front to check type compatibility */
        QueueNode *front = peekQueue(q);
        if (front && front->type == rec.type) {
            dequeue(q, &next);

            newSlot = assignSlot(slots, next.type, next.plate);

            /* Update the vehicle record with its new slot */
            {
                int i;
                for (i = 0; i < count; i++) {
                    if (strcmp(vehicles[i].plate, next.plate) == 0) {
                        vehicles[i].slotId   = newSlot;
                        vehicles[i].waitTime =
                            waitingSeconds(&next);
                        break;
                    }
                }
            }

            if (newSlot != -1)
                printf("\n  [QUEUE] Slot freed — Vehicle %s dequeued"
                       " and assigned Slot S%02d.\n",
                       next.plate, newSlot + 1);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════
 * handleSearchVehicle
 * Searches by plate and displays full vehicle details.
 * ═══════════════════════════════════════════════════════════════ */
static void handleSearchVehicle(Vehicle vehicles[], int count)
{
    char      plate[PLATE_LEN];
    Vehicle  *v;
    char      timeBuf[24];

    printf("\n");
    readLine("Enter Plate Number to Search", plate, PLATE_LEN);
    toUpperCase(plate);

    v = findVehicle(vehicles, count, plate);
    if (!v) {
        printf("\n  [NOT FOUND] No active vehicle with plate \"%s\".\n\n",
               plate);
        return;
    }

    formatTimestamp(v->entryTime, timeBuf, sizeof(timeBuf));

    printf("\n");
    printDivider(50);
    printf("  Vehicle Details\n");
    printDivider(50);
    printf("  Plate      : %s\n",   v->plate);
    printf("  Owner      : %s\n",   v->ownerName);
    printf("  Type       : %s\n",
           (v->type == TWO_WHEELER) ? "2-Wheeler" : "4-Wheeler");
    printf("  Entry Time : %s\n",   timeBuf);
    if (v->slotId >= 0)
        printf("  Slot       : S%02d\n", v->slotId + 1);
    else
        printf("  Slot       : Waiting in queue\n");
    if (v->waitTime > 0.0)
        printf("  Wait Time  : %.0f seconds\n", v->waitTime);
    printDivider(50);
    printf("\n");
}
