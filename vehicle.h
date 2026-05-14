/*
 * vehicle.h
 * ---------
 * Vehicle struct definition and Vehicle Management declarations.
 *
 * Parking Management System
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

/* ── Vehicle structure ─────────────────────────────────────────── */
/*
 * Represents one vehicle in the system.
 *
 * Fields:
 *   plate       - Licence plate number (unique identifier)
 *   ownerName   - Name of the vehicle owner
 *   type        - TWO_WHEELER or FOUR_WHEELER
 *   entryTime   - Unix timestamp when the vehicle entered
 *   exitTime    - Unix timestamp when the vehicle exited (0 if still parked)
 *   waitTime    - Seconds spent in the waiting queue (0 if assigned directly)
 *   slotId      - Index of the assigned parking slot (-1 if in queue)
 *   isParked    - TRUE if currently occupying a slot
 */
typedef struct {
    char        plate[PLATE_LEN];
    char        ownerName[NAME_LEN];
    VehicleType type;
    time_t      entryTime;
    time_t      exitTime;
    double      waitTime;       /* seconds waited in queue          */
    int         slotId;         /* -1 = not yet assigned            */
    Bool        isParked;
} Vehicle;

/* ── Function declarations ─────────────────────────────────────── */

/* Add a new vehicle to the master vehicle array.
   Returns index of the new vehicle, or -1 on failure. */
int  addVehicle(Vehicle vehicles[], int *count,
                const char *plate, const char *owner, VehicleType type);

/* Remove (mark as exited) a vehicle by plate number.
   Returns 1 on success, 0 if not found. */
int  removeVehicle(Vehicle vehicles[], int count, const char *plate);

/* Search for a vehicle by plate number.
   Returns pointer to the Vehicle, or NULL if not found. */
Vehicle *findVehicle(Vehicle vehicles[], int count, const char *plate);

/* Print a formatted table of all currently parked vehicles. */
void displayVehicles(const Vehicle vehicles[], int count);

/* Load vehicle records from data/vehicles.dat into the array.
   Returns the number of records loaded. */
int  loadVehicles(Vehicle vehicles[]);

/* Save all vehicle records from the array to data/vehicles.dat. */
void saveVehicles(const Vehicle vehicles[], int count);

#endif /* VEHICLE_H */
