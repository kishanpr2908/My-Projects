/*
 * slots.c
 * -------
 * Parking Slot Management — full implementation.
 *
 * Slot layout (defined by common.h):
 *   Indices  0 … MAX_2W_SLOTS-1            → TWO_WHEELER  slots  (20 bays)
 *   Indices  MAX_2W_SLOTS … MAX_TOTAL_SLOTS-1 → FOUR_WHEELER slots (10 bays)
 *
 * Responsibilities:
 *   - Initialise the slot array on program start
 *   - Assign the nearest free slot to an incoming vehicle
 *   - Release a slot when a vehicle exits
 *   - Display a formatted visual slot map
 *   - Persist/restore slot state via binary file I/O
 *
 * Parking Management System
 */

#include <ctype.h>
#include <string.h>
#include "../include/slots.h"
#include "../include/utils.h"

/* Path to the binary persistence file (relative to project root) */
#define SLOTS_FILE "data/slots.dat"

/* Number of slot columns per row in the display grid */
#define DISPLAY_COLS 4

/* ═══════════════════════════════════════════════════════════════
 * initSlots
 *
 * Sets up the entire slot array from scratch:
 *   - Slots 0 … MAX_2W_SLOTS-1          → TWO_WHEELER
 *   - Slots MAX_2W_SLOTS … MAX_TOTAL-1  → FOUR_WHEELER
 * All slots start as SLOT_FREE with an empty plate string.
 * Called once on program startup (before loadSlots).
 * ═══════════════════════════════════════════════════════════════ */
void initSlots(ParkingSlot slots[])
{
    int i;
    for (i = 0; i < MAX_TOTAL_SLOTS; i++) {
        slots[i].slotId = i;
        slots[i].type   = (i < MAX_2W_SLOTS) ? TWO_WHEELER : FOUR_WHEELER;
        slots[i].status = SLOT_FREE;
        memset(slots[i].plate, 0, PLATE_LEN);
    }
}

/* ═══════════════════════════════════════════════════════════════
 * assignSlot
 *
 * Scans the slot array for the first SLOT_FREE bay matching the
 * requested vehicle type and assigns it to the given plate.
 *
 * Strategy — "nearest available":
 *   For TWO_WHEELER : scan indices 0 … MAX_2W_SLOTS-1
 *   For FOUR_WHEELER: scan indices MAX_2W_SLOTS … MAX_TOTAL-1
 *
 * Returns: slotId of the assigned slot, or -1 if none available.
 * ═══════════════════════════════════════════════════════════════ */
int assignSlot(ParkingSlot slots[], VehicleType type, const char *plate)
{
    int start, end, i;

    if (type == TWO_WHEELER) {
        start = 0;
        end   = MAX_2W_SLOTS;
    } else {
        start = MAX_2W_SLOTS;
        end   = MAX_TOTAL_SLOTS;
    }

    for (i = start; i < end; i++) {
        if (slots[i].status == SLOT_FREE) {
            slots[i].status = SLOT_OCCUPIED;
            strncpy(slots[i].plate, plate, PLATE_LEN - 1);
            slots[i].plate[PLATE_LEN - 1] = '\0';
            toUpperCase(slots[i].plate);
            return i;           /* return the assigned slot index   */
        }
    }
    return -1;                  /* no free slot of this type        */
}

/* ═══════════════════════════════════════════════════════════════
 * releaseSlot
 *
 * Finds the slot whose plate matches the given plate string
 * (case-insensitive), marks it SLOT_FREE, and clears the plate.
 *
 * Returns: 1 on success, 0 if no matching occupied slot found.
 * ═══════════════════════════════════════════════════════════════ */
int releaseSlot(ParkingSlot slots[], const char *plate)
{
    int  i;
    char upperPlate[PLATE_LEN];

    /* Normalise search key to uppercase */
    strncpy(upperPlate, plate, PLATE_LEN - 1);
    upperPlate[PLATE_LEN - 1] = '\0';
    toUpperCase(upperPlate);

    for (i = 0; i < MAX_TOTAL_SLOTS; i++) {
        if (slots[i].status == SLOT_OCCUPIED &&
            strcmp(slots[i].plate, upperPlate) == 0) {
            slots[i].status = SLOT_FREE;
            memset(slots[i].plate, 0, PLATE_LEN);
            return 1;
        }
    }
    return 0;   /* plate not found in any occupied slot            */
}

/* ═══════════════════════════════════════════════════════════════
 * getAvailableCount
 *
 * Counts the number of SLOT_FREE bays for the given vehicle type.
 * Used by the menu to display live availability.
 * ═══════════════════════════════════════════════════════════════ */
int getAvailableCount(const ParkingSlot slots[], VehicleType type)
{
    int i, count = 0;
    for (i = 0; i < MAX_TOTAL_SLOTS; i++) {
        if (slots[i].type   == type &&
            slots[i].status == SLOT_FREE)
            count++;
    }
    return count;
}

/* ═══════════════════════════════════════════════════════════════
 * displaySlotMap — internal section printer
 * Prints a grid of slots for one vehicle type section.
 * ═══════════════════════════════════════════════════════════════ */
static void printSlotSection(const ParkingSlot slots[],
                              int start, int end,
                              const char *sectionTitle)
{
    int i;
    int col = 0;

    printf("\n  %s\n", sectionTitle);
    printDivider(72);

    for (i = start; i < end; i++) {
        const ParkingSlot *s = &slots[i];

        if (s->status == SLOT_FREE) {
            /* Free slot — show slot ID in green-style bracket     */
            printf("  [S%02d: %-10s]", s->slotId + 1, "FREE");
        } else {
            /* Occupied — show the plate number                    */
            printf("  [S%02d: %-10s]", s->slotId + 1, s->plate);
        }

        col++;
        if (col == DISPLAY_COLS) {
            printf("\n");
            col = 0;
        }
    }
    /* Flush any partial row */
    if (col != 0)
        printf("\n");

    printDivider(72);

    /* Section summary */
    {
        int free  = getAvailableCount(slots,
                        (start == 0) ? TWO_WHEELER : FOUR_WHEELER);
        int total = end - start;
        printf("  Free: %d / %d     Occupied: %d / %d\n",
               free, total, total - free, total);
    }
}

/* ═══════════════════════════════════════════════════════════════
 * displaySlotMap
 *
 * Prints a two-section visual map:
 *   Section A — Two-Wheeler bays  (S01 … S20)
 *   Section B — Four-Wheeler bays (S21 … S30)
 *
 * Each cell: [S##: <plate / FREE>] in a DISPLAY_COLS-wide grid.
 * ═══════════════════════════════════════════════════════════════ */
void displaySlotMap(const ParkingSlot slots[])
{
    printf("\n");
    printf("  Legend:  [S##: FREE]        = available\n");
    printf("           [S##: <PLATE>]     = occupied\n");

    printSlotSection(slots, 0, MAX_2W_SLOTS,
                     "--- TWO-WHEELER BAYS (S01-S20) ---");

    printSlotSection(slots, MAX_2W_SLOTS, MAX_TOTAL_SLOTS,
                     "--- FOUR-WHEELER BAYS (S21-S30) ---");

    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════
 * loadSlots
 *
 * Reads MAX_TOTAL_SLOTS ParkingSlot structs from SLOTS_FILE.
 * If the file does not exist (first run), the default FREE state
 * set by initSlots() is preserved — no error is raised.
 * ═══════════════════════════════════════════════════════════════ */
void loadSlots(ParkingSlot slots[])
{
    FILE *fp = fopen(SLOTS_FILE, "rb");
    if (!fp) {
        /* First run — no saved state yet; initSlots() already ran  */
        return;
    }

    /* Read exactly MAX_TOTAL_SLOTS records                         */
    if (fread(slots, sizeof(ParkingSlot), MAX_TOTAL_SLOTS, fp)
            != (size_t)MAX_TOTAL_SLOTS) {
        /* Partial / corrupt file — reinitialise cleanly            */
        printf("  [WARNING] Slot data file is incomplete."
               " Starting with fresh slots.\n");
        initSlots(slots);
    }

    fclose(fp);
}

/* ═══════════════════════════════════════════════════════════════
 * saveSlots
 *
 * Writes the entire slot array (MAX_TOTAL_SLOTS records) to
 * SLOTS_FILE in binary format, overwriting the previous file.
 * Called on every vehicle entry/exit and at program exit.
 * ═══════════════════════════════════════════════════════════════ */
void saveSlots(const ParkingSlot slots[])
{
    FILE *fp = fopen(SLOTS_FILE, "wb");
    if (!fp) {
        printf("  [ERROR] Cannot open \"%s\" for writing."
               " Slot data not saved.\n", SLOTS_FILE);
        return;
    }

    if (fwrite(slots, sizeof(ParkingSlot), MAX_TOTAL_SLOTS, fp)
            != (size_t)MAX_TOTAL_SLOTS) {
        printf("  [WARNING] Slot data may not have been saved completely.\n");
    }

    fclose(fp);
}
