/*
 * utils.c
 * -------
 * Shared Utility Helpers — implementation.
 *
 * Parking Management System
 */

#include "../include/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── formatTimestamp ──────────────────────────────────────────── */
void formatTimestamp(time_t t, char *buf, size_t bufSize)
{
    struct tm *tm_info = localtime(&t);
    if (tm_info != NULL) {
        strftime(buf, bufSize, "%d-%m-%Y %H:%M:%S", tm_info);
    } else {
        if (bufSize > 0) buf[0] = '\0';
    }
}

/* ── elapsedSeconds ───────────────────────────────────────────── */
double elapsedSeconds(time_t start, time_t end)
{
    return difftime(end, start);
}

/* ── toUpperCase ──────────────────────────────────────────────── */
void toUpperCase(char *str)
{
    if (!str) return;
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

/* ── trimWhitespace ───────────────────────────────────────────── */
void trimWhitespace(char *str)
{
    char *start;
    char *end;

    if (!str) return;

    start = str;
    /* Find first non-whitespace character */
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /* If all spaces or empty */
    if (*start == '\0') {
        *str = '\0';
        return;
    }

    /* Shift string to the beginning if there were leading spaces */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    /* Trim trailing spaces */
    end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
}

/* ── validatePlate ────────────────────────────────────────────── */
int validatePlate(const char *plate)
{
    if (!plate || *plate == '\0') {
        return 0;
    }
    
    while (*plate) {
        if (!isalnum((unsigned char)*plate) && *plate != '-') {
            return 0;
        }
        plate++;
    }
    return 1;
}

/* ── clearScreen ──────────────────────────────────────────────── */
void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ── printDivider ─────────────────────────────────────────────── */
void printDivider(int width)
{
    int i;
    for (i = 0; i < width; i++) {
        putchar('-');
    }
    putchar('\n');
}

/* ── pressEnterToContinue ─────────────────────────────────────── */
void pressEnterToContinue(void)
{
    int c;
    printf("\n  Press Enter to continue...");
    fflush(stdout);
    
    /* Wait until the user presses Enter (or EOF is encountered) */
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Discard characters until newline */
    }
}
