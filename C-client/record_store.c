/**
 * @file    record_store.c
 * @brief   Implementation of the CSV-backed record store.
 */

#include "record_store.h"
#include <stdio.h>
#include <string.h>

static const char *CSV_HEADER = "test_id,timestamp,peripheral,duration_seconds,result";

void record_store_init(RecordStore *store, const char *file_path)
{
    strncpy(store->file_path, file_path, FILE_PATH_LEN - 1);
    store->file_path[FILE_PATH_LEN - 1] = '\0';

    /* Only write the header if the file doesn't exist yet or is empty -
     * otherwise "init" on every run would wipe previously saved records. */
    FILE *check = fopen(store->file_path, "r");
    int needs_header = 1;
    if (check != NULL) {
        int c = fgetc(check);
        if (c != EOF) {
            needs_header = 0;
        }
        fclose(check);
    }

    if (needs_header) {
        FILE *out = fopen(store->file_path, "w");
        if (out != NULL) {
            fprintf(out, "%s\n", CSV_HEADER);
            fclose(out);
        }
    }
}

void record_store_append(const RecordStore *store, const TestRecord *record)
{
    FILE *out = fopen(store->file_path, "a");
    if (out == NULL) {
        return;
    }
    fprintf(out, "%u,%s,%s,%.3f,%s\n",
            record->test_id,
            record->timestamp,
            record->peripheral_name,
            record->duration_seconds,
            record->success ? "SUCCESS" : "FAILURE");
    fclose(out);
}

/**
 * @brief Reads one CSV data line into a TestRecord.
 * @return 1 on a successfully parsed line, 0 if the line was malformed
 *         (in which case it is skipped rather than crashing the program).
 */
static int parse_line(const char *line, TestRecord *record)
{
    char result_str[16];
    int parsed = sscanf(line, "%u,%31[^,],%15[^,],%lf,%15s",
                         &record->test_id,
                         record->timestamp,
                         record->peripheral_name,
                         &record->duration_seconds,
                         result_str);
    if (parsed != 5) {
        return 0;
    }
    record->success = (strcmp(result_str, "SUCCESS") == 0);
    return 1;
}

void record_store_print_all(const RecordStore *store)
{
    FILE *in = fopen(store->file_path, "r");
    if (in == NULL) {
        printf("No test records yet.\n");
        return;
    }

    char line[512];
    int has_data = 0;
    if (fgets(line, sizeof(line), in) == NULL) { fclose(in); printf("No test records yet.\n"); return; } /* skip header */

    printf("%-12s%-22s%-10s%-12s%s\n", "TEST-ID", "TIMESTAMP", "PERIPH", "DURATION(s)", "RESULT");
    for (int i = 0; i < 70; i++) putchar('-');
    putchar('\n');

    while (fgets(line, sizeof(line), in) != NULL) {
        TestRecord r;
        if (!parse_line(line, &r)) continue; /* skip malformed lines rather than crash */
        has_data = 1;
        printf("%-12u%-22s%-10s%-12.3f%s\n",
               r.test_id, r.timestamp, r.peripheral_name, r.duration_seconds,
               r.success ? "SUCCESS" : "FAILURE");
    }
    fclose(in);

    if (!has_data) {
        printf("(no records found)\n");
    }
}

uint32_t record_store_max_test_id(const RecordStore *store)
{
    FILE *in = fopen(store->file_path, "r");
    if (in == NULL) {
        return 0;
    }

    char line[512];
    uint32_t max_id = 0;
    if (fgets(line, sizeof(line), in) != NULL) { /* skip header */
        while (fgets(line, sizeof(line), in) != NULL) {
            TestRecord r;
            if (!parse_line(line, &r)) continue;
            if (r.test_id > max_id) {
                max_id = r.test_id;
            }
        }
    }
    fclose(in);
    return max_id;
}

void record_store_empty(const RecordStore *store)
{
    FILE *out = fopen(store->file_path, "w");
        if (out != NULL) {
            fprintf(out, "%s\n", CSV_HEADER);
            fclose(out);
        }
}

void record_store_print_one(const RecordStore *store, uint32_t test_id)
{
    FILE *in = fopen(store->file_path, "r");
    if (in == NULL) {
        printf("No test records yet.\n");
        return;
    }

    char line[512];
    int found = 0;
    if (fgets(line, sizeof(line), in) == NULL) { fclose(in); printf("No test records yet.\n"); return; } /* skip header */

    while (fgets(line, sizeof(line), in) != NULL) {
        TestRecord r;
        if (!parse_line(line, &r)) continue;
        if (r.test_id == test_id) {
            printf("Test-ID:    %u\n", r.test_id);
            printf("Timestamp:  %s\n", r.timestamp);
            printf("Peripheral: %s\n", r.peripheral_name);
            printf("Duration:   %.3fs\n", r.duration_seconds);
            printf("Result:     %s\n", r.success ? "SUCCESS" : "FAILURE");
            found = 1;
        }
    }
    fclose(in);

    if (!found) {
        printf("No record found for test-id %u\n", test_id);
    }
}