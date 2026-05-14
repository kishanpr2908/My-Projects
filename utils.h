/*
 * utils.h
 * -------
 * Shared utility / helper function declarations.
 *
 * Parking Management System
 */

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

/* Format a time_t value into a readable string "DD-MM-YYYY HH:MM:SS".
   Result is written into buf (must be at least 20 chars). */
void formatTimestamp(time_t t, char *buf, size_t bufSize);

/* Return elapsed seconds between two time_t values. */
double elapsedSeconds(time_t start, time_t end);

/* Convert a string to uppercase in-place. */
void toUpperCase(char *str);

/* Trim leading and trailing whitespace from str in-place. */
void trimWhitespace(char *str);

/* Validate that a licence plate contains only alphanumeric chars.
   Returns 1 if valid, 0 otherwise. */
int  validatePlate(const char *plate);

/* Clear the terminal screen (cross-platform wrapper). */
void clearScreen(void);

/* Print a horizontal divider line of given width. */
void printDivider(int width);

/* Pause execution until the user presses Enter. */
void pressEnterToContinue(void);

#endif /* UTILS_H */
