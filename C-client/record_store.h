/**
 * @file    record_store.h
 * @brief   Persistent (filesystem-backed) storage for test records, per
 *          spec: TEST-ID, date/time sent, test length in seconds, result.
 *          Stored as CSV so records survive across program runs and can
 *          be inspected outside the program too.
 */

#ifndef RECORD_STORE_H_
#define RECORD_STORE_H_

#include <stdint.h>

#define TIMESTAMP_LEN 32
#define PERIPHERAL_NAME_LEN 16
#define FILE_PATH_LEN 256

/** @brief One persisted test record. */
typedef struct {
    uint32_t test_id;
    char     timestamp[TIMESTAMP_LEN];      /*local time the test was sent*/
    char     peripheral_name[PERIPHERAL_NAME_LEN];
    double   duration_seconds;              /*time from send to response*/
    int      success;                       /*1 = SUCCESS, 0 = FAILURE */
} TestRecord;

/** @brief Handle for a CSV-backed record file. */
typedef struct {
    char file_path[FILE_PATH_LEN];
} RecordStore;

/**
 * @brief Opens the backing CSV file, writing a header row
 *        if it doesn't exist yet.
 */
void record_store_init(RecordStore *store, const char *file_path);

/** @brief appends one record to the file.*/
void record_store_append(const RecordStore *store, const TestRecord *record);

/** @brief Empties the record store, leaving only the header row. */
void record_store_empty(const RecordStore *store);

/** @brief Prints every record as a formatted table. */
void record_store_print_all(const RecordStore *store);

/** @brief Prints only the record matching test_id, or a not-found message. */
void record_store_print_one(const RecordStore *store, uint32_t test_id);

/**
 * @brief Finds the highest test_id already saved in the store.
 * @return The highest existing test_id, or 0 if the store is empty.
 *         Callers typically start the next test_id at this value + 1,
 *         so IDs stay low/readable across program runs instead of
 *         restarting from a large timestamp-sized number each time.
 */
uint32_t record_store_max_test_id(const RecordStore *store);

#endif /* RECORD_STORE_H_ */