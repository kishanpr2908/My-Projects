/*
 * slots.h
 * -------
 * ParkingSlot struct definition and Slot Management declarations.
 *
 * Parking Management System
 */

#ifndef SLOTS_H
#define SLOTS_H

#include "common.h"

/* ── Parking slot structure ────────────────────────────────────── */
/*
 * Represents a single physical parking slot.
 *
 * Fields:
 *   slotId      - Unique slot index (0 … MAX_TOTAL_SLOTS-1)
 *   type        - TWO_WHEELER or FOUR_WHEELER (fixed at initialisation)
 *   status      - SLOT_FREE or SLOT_OCCUPIED
 *   plate       - Plate of the vehicle currently in this slot
 *                 (empty string when SLOT_FREE)
 */
typedef struct {
    int         slotId;
    VehicleType type;
    SlotStatus  status;
    char        plate[PLATE_LEN];   /* vehicle currently occupying  */
} ParkingSlot;

/* ── Slot pool (global array, defined in slots.c) ──────────────── */
/*
 * Layout:
 *   Indices  0 … MAX_2W_SLOTS-1          → two-wheeler slots
 *   Indices  MAX_2W_SLOTS … MAX_TOTAL_SLOTS-1 → four-wheeler slots
 */

/* ── Function declarations ─────────────────────────────────────── */

/* Initialise the slot array (called once at program start). */
void initSlots(ParkingSlot slots[]);

/* Find and assign the nearest free slot for the given vehicle type.
   Fills plate into the slot and marks it OCCUPIED.
   Returns assigned slotId, or -1 if no slot is available. */
int  assignSlot(ParkingSlot slots[], VehicleType type, const char *plate);

/* Release the slot occupied by the given plate number.
   Returns 1 on success, 0 if the plate was not found. */
int  releaseSlot(ParkingSlot slots[], const char *plate);

/* Count available (free) slots of a given type. */
int  getAvailableCount(const ParkingSlot slots[], VehicleType type);

/* Print a visual slot map showing FREE / OCCUPIED status. */
void displaySlotMap(const ParkingSlot slots[]);

/* Load slot state from data/slots.dat. */
void loadSlots(ParkingSlot slots[]);

/* Persist current slot state to data/slots.dat. */
void saveSlots(const ParkingSlot slots[]);

#endif /* SLOTS_H */
