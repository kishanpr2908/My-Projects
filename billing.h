/*
 * billing.h
 * ---------
 * BillingRecord struct and Billing System declarations.
 *
 * Parking Management System
 */

#ifndef BILLING_H
#define BILLING_H

#include "common.h"

/* ── Billing record structure ──────────────────────────────────── */
/*
 * Stores a completed parking transaction.
 *
 * Fields:
 *   plate       - Vehicle licence plate
 *   ownerName   - Owner name
 *   type        - Vehicle type (determines rate tier)
 *   entryTime   - Parking entry timestamp
 *   exitTime    - Parking exit timestamp
 *   waitTime    - Time spent in waiting queue (seconds)
 *   hoursParked - Total hours billed (rounded up to next hour)
 *   totalFee    - Final charge in INR
 */
typedef struct {
    char        plate[PLATE_LEN];
    char        ownerName[NAME_LEN];
    VehicleType type;
    time_t      entryTime;
    time_t      exitTime;
    double      waitTime;
    double      hoursParked;
    double      totalFee;
} BillingRecord;

/* ── Function declarations ─────────────────────────────────────── */

/* Calculate the parking fee for a vehicle type and duration.
   durationSecs - total parking duration in seconds.
   Returns the fee in INR. */
double calculateFee(VehicleType type, double durationSecs);

/* Generate and print a formatted receipt to stdout and output/ dir. */
void generateReceipt(const BillingRecord *rec);

/* Append a completed BillingRecord to data/billing.dat. */
void saveBillingRecord(const BillingRecord *rec);

/* Print a revenue summary report (total transactions, total income). */
void showRevenueReport(void);

#endif /* BILLING_H */
