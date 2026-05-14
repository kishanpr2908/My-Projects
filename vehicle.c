/*
 * vehicle.c
 * ---------
 * Vehicle Management — full implementation.
 *
 * Responsibilities:
 *   - Add / remove / search vehicle records
 *   - Display a formatted table of parked vehicles
 *   - Persist records to and from data/vehicles.dat
 *
 * Parking Management System
 */

#include <ctype.h>
#include "../include/vehicle.h"
#include "../include/utils.h"

/* Path to the binary persistence file (relative to project root) */
#define VEHICLES_FILE "data/vehicles.dat"

/* ═══════════════════════════════════════════════════════════════
 * Internal helper — case-insensitive plate comparison.
 * Returns 0 if equal, non-zero otherwise.
 * ═══════════════════════════════════════════════════════════════ */
static int plateCmp(const char *a, const char *b)
{
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b))
            return 1;
        a++;
        b++;
    }
    return (*a != *b);   /* both must end at the same position     */
}

/* ═══════════════════════════════════════════════════════════════
 * addVehicle
 *
 * Validates inputs, checks for duplicate active plates, then
 * writes a new Vehicle record into the array and increments *count.
 *
 * Returns: index of the new vehicle on success, -1 on failure.
 * ═══════════════════════════════════════════════════════════════ */
int addVehicle(Vehicle vehicles[], int *count,
               const char *plate, const char *owner, VehicleType type)
{
    int i;
    Vehicle *v;

    /* ── Guard: registry capacity ────────────────────────────── */
    if (*count >= MAX_VEHICLES) {
        printf("  [ERROR] Vehicle registry is full (max %d entries).\n",
               MAX_VEHICLES);
        return -1;
    }

    /* ── Guard: non-empty strings ────────────────────────────── */
    if (!plate || plate[0] == '\0') {
        printf("  [ERROR] Plate number cannot be empty.\n");
        return -1;
    }
    if (!owner || owner[0] == '\0') {
        printf("  [ERROR] Owner name cannot be empty.\n");
        return -1;
    }

    /* ── Guard: plate format (alphanumeric + hyphens) ─────────── */
    if (!validatePlate(plate)) {
        printf("  [ERROR] Invalid plate \"%s\"."
               " Use only letters, digits, and hyphens.\n", plate);
        return -1;
    }

    /* ── Guard: duplicate active plate ───────────────────────── */
    for (i = 0; i < *count; i++) {
        if (vehicles[i].isParked &&
            plateCmp(vehicles[i].plate, plate) == 0) {
            printf("  [ERROR] Vehicle \"%s\" is already in the parking lot.\n",
                   plate);
            return -1;
        }
    }

    /* ── Populate new Vehicle record ──────────────────────────── */
    v = &vehicles[*count];
    memset(v, 0, sizeof(Vehicle));

    strncpy(v->plate,     plate, PLATE_LEN - 1);
    strncpy(v->ownerName, owner, NAME_LEN  - 1);
    v->plate[PLATE_LEN - 1]    = '\0';
    v->ownerName[NAME_LEN - 1] = '\0';

    toUpperCase(v->plate);      /* normalise plate to uppercase     */
    trimWhitespace(v->ownerName);

    v->type      = type;
    v->entryTime = time(NULL);
    v->exitTime  = 0;
    v->waitTime  = 0.0;
    v->slotId    = -1;          /* slot assigned separately by slots module */
    v->isParked  = TRUE;

    (*count)++;
    return (*count) - 1;        /* index of the newly added vehicle */
}

/* ═══════════════════════════════════════════════════════════════
 * removeVehicle
 *
 * Marks the matching (currently parked) vehicle as exited by
 * recording the exit timestamp and clearing the isParked flag.
 * The record is kept in the array for billing history.
 *
 * Returns: 1 on success, 0 if no matching parked vehicle found.
 * ═══════════════════════════════════════════════════════════════ */
int removeVehicle(Vehicle vehicles[], int count, const char *plate)
{
    int i;
    for (i = 0; i < count; i++) {
        if (vehicles[i].isParked &&
            plateCmp(vehicles[i].plate, plate) == 0) {
            vehicles[i].exitTime = time(NULL);
            vehicles[i].isParked = FALSE;
            vehicles[i].slotId   = -1;
            return 1;
        }
    }
    return 0;   /* not found among currently parked vehicles       */
}

/* ═══════════════════════════════════════════════════════════════
 * findVehicle
 *
 * Linear scan for a currently parked vehicle by plate number.
 * Comparison is case-insensitive.
 *
 * Returns: pointer to the matching Vehicle, or NULL if not found.
 * ═══════════════════════════════════════════════════════════════ */
Vehicle *findVehicle(Vehicle vehicles[], int count, const char *plate)
{
    int i;
    for (i = 0; i < count; i++) {
        if (vehicles[i].isParked &&
            plateCmp(vehicles[i].plate, plate) == 0)
            return &vehicles[i];
    }
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════
 * displayVehicles
 *
 * Prints a formatted table of all vehicles that are currently
 * occupying a parking slot or waiting in the queue.
 * Exited vehicles are not shown.
 * ═══════════════════════════════════════════════════════════════ */
void displayVehicles(const Vehicle vehicles[], int count)
{
    char entryBuf[24];
    int  parkedCount = 0;
    int  i;

    /* Count active vehicles first so we can handle the empty case  */
    for (i = 0; i < count; i++)
        if (vehicles[i].isParked) parkedCount++;

    if (parkedCount == 0) {
        printf("\n  No vehicles currently parked.\n\n");
        return;
    }

    /* ── Table header ────────────────────────────────────────── */
    printf("\n");
    printDivider(80);
    printf("  %-12s  %-24s  %-12s  %-20s  %s\n",
           "PLATE", "OWNER NAME", "TYPE", "ENTRY TIME", "SLOT");
    printDivider(80);

    /* ── Table rows ──────────────────────────────────────────── */
    for (i = 0; i < count; i++) {
        const Vehicle *v = &vehicles[i];
        if (!v->isParked) continue;

        formatTimestamp(v->entryTime, entryBuf, sizeof(entryBuf));

        printf("  %-12s  %-24s  %-12s  %-20s  ",
               v->plate,
               v->ownerName,
               (v->type == TWO_WHEELER) ? "2-Wheeler" : "4-Wheeler",
               entryBuf);

        if (v->slotId >= 0)
            printf("S%02d\n", v->slotId + 1);
        else
            printf("(in queue)\n");
    }

    /* ── Footer ──────────────────────────────────────────────── */
    printDivider(80);
    printf("  Total vehicles on premises: %d\n\n", parkedCount);
}

/* ═══════════════════════════════════════════════════════════════
 * loadVehicles
 *
 * Reads binary Vehicle records from VEHICLES_FILE into the array.
 * If the file does not exist (first run), silently returns 0.
 *
 * Returns: number of records loaded.
 * ═══════════════════════════════════════════════════════════════ */
int loadVehicles(Vehicle vehicles[])
{
    FILE   *fp;
    int     count = 0;
    Vehicle v;

    fp = fopen(VEHICLES_FILE, "rb");
    if (!fp) {
        /* Normal on first run — no data file yet */
        return 0;
    }

    while (count < MAX_VEHICLES &&
           fread(&v, sizeof(Vehicle), 1, fp) == 1) {
        vehicles[count++] = v;
    }

    fclose(fp);
    return count;
}

/* ═══════════════════════════════════════════════════════════════
 * saveVehicles
 *
 * Writes all Vehicle records (including historical exits) to
 * VEHICLES_FILE in binary format, overwriting the previous file.
 * ═══════════════════════════════════════════════════════════════ */
void saveVehicles(const Vehicle vehicles[], int count)
{
    FILE *fp;

    fp = fopen(VEHICLES_FILE, "wb");
    if (!fp) {
        printf("  [ERROR] Cannot open \"%s\" for writing."
               " Data not saved.\n", VEHICLES_FILE);
        return;
    }

    if (fwrite(vehicles, sizeof(Vehicle), (size_t)count, fp)
            != (size_t)count) {
        printf("  [WARNING] Not all vehicle records were saved correctly.\n");
    }

    fclose(fp);
}
