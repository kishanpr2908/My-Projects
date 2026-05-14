/*
 * common.h
 * --------
 * Global constants, shared enums, and standard library includes.
 * Every module includes this file first via its own header.
 *
 * Parking Management System
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Capacity limits ───────────────────────────────────────────── */
#define MAX_2W_SLOTS     20       /* Two-wheeler parking capacity  */
#define MAX_4W_SLOTS     10       /* Four-wheeler parking capacity  */
#define MAX_TOTAL_SLOTS  (MAX_2W_SLOTS + MAX_4W_SLOTS)
#define MAX_QUEUE_SIZE   15       /* Max vehicles on waiting list   */
#define MAX_VEHICLES     50       /* Total vehicle records in memory */
#define PLATE_LEN        12       /* Max licence plate string length */
#define NAME_LEN         50       /* Max owner name string length    */

/* ── Billing rates (in INR) ────────────────────────────────────── */
#define RATE_2W_BASE     20.0     /* Flat rate for first hour (2W)  */
#define RATE_2W_PER_HR   10.0     /* Per hour after first hour (2W) */
#define RATE_4W_BASE     50.0     /* Flat rate for first hour (4W)  */
#define RATE_4W_PER_HR   30.0     /* Per hour after first hour (4W) */

/* ── Vehicle type enum ─────────────────────────────────────────── */
typedef enum {
    TWO_WHEELER  = 1,
    FOUR_WHEELER = 2
} VehicleType;

/* ── Slot status enum ──────────────────────────────────────────── */
typedef enum {
    SLOT_FREE     = 0,
    SLOT_OCCUPIED = 1
} SlotStatus;

/* ── Boolean convenience ───────────────────────────────────────── */
typedef enum {
    FALSE = 0,
    TRUE  = 1
} Bool;

#endif /* COMMON_H */
