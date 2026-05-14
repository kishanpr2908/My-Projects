/*
 * billing.c
 * ---------
 * Billing System — full implementation.
 *
 * Fee tiers (from common.h):
 *   2-Wheeler : INR 20 base (first hour) + INR 10 per additional hour
 *   4-Wheeler : INR 50 base (first hour) + INR 30 per additional hour
 *
 * Duration is always rounded UP to the next whole hour before billing.
 * Minimum charge is always the base rate (even for < 1 hour stays).
 *
 * Responsibilities:
 *   - calculateFee()       : compute charge from duration + vehicle type
 *   - generateReceipt()    : print receipt to stdout + write to output/
 *   - saveBillingRecord()  : append record to data/billing.dat
 *   - showRevenueReport()  : read all records, print summary analytics
 *
 * Parking Management System
 */

#include <math.h>
#include <string.h>
#include "../include/billing.h"
#include "../include/utils.h"

/* File paths */
#define BILLING_FILE   "data/billing.dat"
#define RECEIPT_DIR    "output/"

/* Receipt line width */
#define RW 54

/* ═══════════════════════════════════════════════════════════════
 * calculateFee
 *
 * Formula:
 *   billedHours = ceil(durationSecs / 3600.0)   (min = 1)
 *   fee = baseRate + max(0, billedHours - 1) * perHourRate
 *
 * Returns: total fee in INR (double).
 * ═══════════════════════════════════════════════════════════════ */
double calculateFee(VehicleType type, double durationSecs)
{
    double baseRate, perHrRate;
    double billedHours;
    double fee;

    if (type == TWO_WHEELER) {
        baseRate   = RATE_2W_BASE;
        perHrRate  = RATE_2W_PER_HR;
    } else {
        baseRate   = RATE_4W_BASE;
        perHrRate  = RATE_4W_PER_HR;
    }

    /* Minimum 1 hour; always ceil so a 61-minute stay = 2 hours   */
    billedHours = ceil(durationSecs / 3600.0);
    if (billedHours < 1.0) billedHours = 1.0;

    fee = baseRate + (billedHours - 1.0) * perHrRate;
    return fee;
}

/* ═══════════════════════════════════════════════════════════════
 * printReceiptLine  — internal helper
 * Writes one formatted receipt line to both FILE* targets.
 * ═══════════════════════════════════════════════════════════════ */
static void printReceiptLine(FILE *fp, const char *line)
{
    /* Print to stdout */
    printf("%s\n", line);
    /* Mirror to file if provided */
    if (fp)
        fprintf(fp, "%s\n", line);
}

/* ═══════════════════════════════════════════════════════════════
 * generateReceipt
 *
 * Prints a formatted parking receipt to:
 *   1. stdout  (always)
 *   2. output/receipt_<PLATE>_<exitTimestamp>.txt  (file copy)
 *
 * Shows: vehicle details, entry/exit times, duration,
 *        billing tier breakdown, wait time, and total amount due.
 * ═══════════════════════════════════════════════════════════════ */
void generateReceipt(const BillingRecord *rec)
{
    char  entryBuf[24], exitBuf[24];
    char  receiptPath[128];
    char  line[RW + 4];
    FILE *fp = NULL;

    double durationSecs  = difftime(rec->exitTime, rec->entryTime);
    double billedHours   = ceil(durationSecs / 3600.0);
    if (billedHours < 1.0) billedHours = 1.0;

    double baseRate      = (rec->type == TWO_WHEELER)
                            ? RATE_2W_BASE  : RATE_4W_BASE;
    double perHrRate     = (rec->type == TWO_WHEELER)
                            ? RATE_2W_PER_HR : RATE_4W_PER_HR;
    double extraHours    = (billedHours > 1.0) ? (billedHours - 1.0) : 0.0;

    long   durationMin   = (long)(durationSecs / 60.0);
    long   durationHr    = durationMin / 60;
    long   durationRemMin= durationMin % 60;

    /* Format timestamps */
    formatTimestamp(rec->entryTime, entryBuf, sizeof(entryBuf));
    formatTimestamp(rec->exitTime,  exitBuf,  sizeof(exitBuf));

    /* Build receipt file path: output/receipt_MH12AB_1714900000.txt */
    snprintf(receiptPath, sizeof(receiptPath),
             "%sreceipt_%s_%ld.txt",
             RECEIPT_DIR, rec->plate, (long)rec->exitTime);

    /* Open the receipt file (non-fatal if it fails) */
    fp = fopen(receiptPath, "w");
    if (!fp)
        printf("  [WARNING] Could not write receipt file to \"%s\".\n",
               receiptPath);

    /* ── Receipt body ────────────────────────────────────────── */
    printf("\n");
    if (fp) fprintf(fp, "\n");

    /* Top border */
    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "======================================================");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  |%*s|", RW, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |      PARKING MANAGEMENT SYSTEM%-*s|",
             RW - 31, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |               PARKING RECEIPT%-*s|",
             RW - 30, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  |%*s|", RW, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "------------------------------------------------------");
    printReceiptLine(fp, line);

    /* Vehicle info */
    snprintf(line, sizeof(line),
             "  |  Plate     : %-*s|", RW - 14, rec->plate);
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |  Owner     : %-*s|", RW - 14, rec->ownerName);
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |  Type      : %-*s|", RW - 14,
             (rec->type == TWO_WHEELER) ? "2-Wheeler" : "4-Wheeler");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "------------------------------------------------------");
    printReceiptLine(fp, line);

    /* Time info */
    snprintf(line, sizeof(line),
             "  |  Entry     : %-*s|", RW - 14, entryBuf);
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |  Exit      : %-*s|", RW - 14, exitBuf);
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |  Duration  : %ldh %02ldm%-*s|",
             durationHr, durationRemMin,
             RW - 22, "");
    printReceiptLine(fp, line);

    if (rec->waitTime > 0.0) {
        long wm = (long)(rec->waitTime / 60.0);
        long wh = wm / 60;
        long wr = wm % 60;
        if (wh > 0)
            snprintf(line, sizeof(line),
                     "  |  Wait Time : %ldh %02ldm%-*s|",
                     wh, wr, RW - 22, "");
        else
            snprintf(line, sizeof(line),
                     "  |  Wait Time : %ldm%-*s|",
                     wm, RW - 18, "");
        printReceiptLine(fp, line);
    }

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "------------------------------------------------------");
    printReceiptLine(fp, line);

    /* Fee breakdown */
    snprintf(line, sizeof(line),
             "  |  Billing Breakdown:%-*s|", RW - 20, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |    Base rate (1st hr)   : INR %6.2f%-*s|",
             baseRate, RW - 38, "");
    printReceiptLine(fp, line);

    if (extraHours > 0.0) {
        snprintf(line, sizeof(line),
                 "  |    Extra  %.0fh x INR %.2f  : INR %6.2f%-*s|",
                 extraHours, perHrRate, extraHours * perHrRate,
                 RW - 46, "");
        printReceiptLine(fp, line);
    }

    snprintf(line, sizeof(line),
             "  |    Billed hours          : %.0f hour(s)%-*s|",
             billedHours, RW - 38, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "======================================================");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |  TOTAL AMOUNT DUE        : INR %8.2f%-*s|",
             rec->totalFee, RW - 40, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "======================================================");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line),
             "  |       Thank you! Drive safely.%-*s|",
             RW - 31, "");
    printReceiptLine(fp, line);

    snprintf(line, sizeof(line), "  +%.*s+",
             RW, "======================================================");
    printReceiptLine(fp, line);

    printf("\n");
    if (fp) {
        fprintf(fp, "\n");
        fclose(fp);
        printf("  [OK] Receipt saved to: %s\n", receiptPath);
    }
}

/* ═══════════════════════════════════════════════════════════════
 * saveBillingRecord
 *
 * Appends one BillingRecord struct to BILLING_FILE in binary
 * append mode so historical records accumulate over time.
 * ═══════════════════════════════════════════════════════════════ */
void saveBillingRecord(const BillingRecord *rec)
{
    FILE *fp = fopen(BILLING_FILE, "ab");
    if (!fp) {
        printf("  [ERROR] Cannot open \"%s\" for writing."
               " Billing record not saved.\n", BILLING_FILE);
        return;
    }

    if (fwrite(rec, sizeof(BillingRecord), 1, fp) != 1)
        printf("  [WARNING] Billing record may not have been saved.\n");

    fclose(fp);
}

/* ═══════════════════════════════════════════════════════════════
 * showRevenueReport
 *
 * Reads every BillingRecord from BILLING_FILE and displays:
 *   - Total transactions
 *   - Breakdown by vehicle type (2W vs 4W)
 *   - Total revenue, average fee, highest single fee
 *   - A paginated transaction log (most recent 10)
 * ═══════════════════════════════════════════════════════════════ */
void showRevenueReport(void)
{
    FILE          *fp;
    BillingRecord  rec;
    int    total       = 0;
    int    count2w     = 0,  count4w     = 0;
    double revenue     = 0.0;
    double revenue2w   = 0.0, revenue4w   = 0.0;
    double maxFee      = 0.0;
    char   maxPlate[PLATE_LEN];
    char   entryBuf[24], exitBuf[24];

    /* Circular buffer: keep last 10 records for the log           */
    BillingRecord last10[10];
    int           last10Count = 0;

    maxPlate[0] = '\0';

    fp = fopen(BILLING_FILE, "rb");
    if (!fp) {
        printf("\n  No billing records found. The lot is new!\n\n");
        return;
    }

    /* ── Accumulate statistics ───────────────────────────────── */
    while (fread(&rec, sizeof(BillingRecord), 1, fp) == 1) {
        total++;
        revenue += rec.totalFee;

        if (rec.type == TWO_WHEELER) {
            count2w++;
            revenue2w += rec.totalFee;
        } else {
            count4w++;
            revenue4w += rec.totalFee;
        }

        if (rec.totalFee > maxFee) {
            maxFee = rec.totalFee;
            strncpy(maxPlate, rec.plate, PLATE_LEN - 1);
            maxPlate[PLATE_LEN - 1] = '\0';
        }

        /* Slide the last-10 window */
        if (last10Count < 10) {
            last10[last10Count++] = rec;
        } else {
            /* Shift left by 1 to make room */
            int j;
            for (j = 0; j < 9; j++) last10[j] = last10[j + 1];
            last10[9] = rec;
        }
    }
    fclose(fp);

    if (total == 0) {
        printf("\n  No completed transactions yet.\n\n");
        return;
    }

    /* ── Print summary ────────────────────────────────────────── */
    printf("\n");
    printDivider(60);
    printf("  REVENUE SUMMARY\n");
    printDivider(60);
    printf("  Total Transactions : %d\n",     total);
    printf("  Total Revenue      : INR %.2f\n", revenue);
    printf("  Average Fee        : INR %.2f\n", revenue / total);
    printf("  Highest Single Fee : INR %.2f  (%s)\n", maxFee, maxPlate);
    printDivider(60);
    printf("  BY VEHICLE TYPE\n");
    printDivider(60);
    printf("  2-Wheelers  : %d transactions   INR %.2f\n",
           count2w, revenue2w);
    printf("  4-Wheelers  : %d transactions   INR %.2f\n",
           count4w, revenue4w);

    /* ── Print recent transaction log ───────────────────────── */
    printDivider(60);
    printf("  RECENT TRANSACTIONS (last %d)\n", last10Count);
    printDivider(60);
    printf("  %-12s  %-10s  %-20s  %s\n",
           "PLATE", "TYPE", "EXIT TIME", "FEE (INR)");
    printDivider(60);

    {
        int i;
        for (i = 0; i < last10Count; i++) {
            const BillingRecord *r = &last10[i];
            formatTimestamp(r->exitTime, exitBuf, sizeof(exitBuf));
            (void)entryBuf; /* suppress unused warning */
            printf("  %-12s  %-10s  %-20s  %.2f\n",
                   r->plate,
                   (r->type == TWO_WHEELER) ? "2-Wheeler" : "4-Wheeler",
                   exitBuf,
                   r->totalFee);
        }
    }

    printDivider(60);
    printf("\n");
}
