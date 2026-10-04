/**
 * @file    main.c
 * @brief   P.C. Testing Program (Server side): CLI that sends test
 *          commands to the UUT over UDP, times the round trip, and
 *          persists each result via the record store.
 *
 * Usage:   ./uut_tester <uut_ip> [port]
 * Commands (typed at the "uut>" prompt):
 *   all/runAll
 *   run <timer|uart|spi|i2c|adc> <iterations> [pattern]
 *   list
 *   show <test_id>
 *   empty
 *   iter <number>
 *   help
 *   clear
 *   quit
 */

#define _POSIX_C_SOURCE 200809L /* for localtime_r, clock_gettime, CLOCK_MONOTONIC under -std=c11 */

#include "protocol.h"
#include "udp_client.h"
#include "record_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DEFAULT_PORT      1997
#define RECORDS_FILE      "test_records.csv"
#define LINE_BUF_LEN       512
#define TOKEN_LEN          64
#define DEFAULT_PATTERN    "TestPattern for all peripherals"
static uint8_t iter=5;            //default number of iterations for each test
/** @brief Formats the current local time as "YYYY-MM-DD HH:MM:SS" into buf. */
static void current_timestamp(char *buf, size_t buf_len)
{
    time_t t = time(NULL);
    struct tm local_tm;
    localtime_r(&t, &local_tm);
    strftime(buf, buf_len, "%Y-%m-%d %H:%M:%S", &local_tm);
}

/**
 * @brief Picks a response timeout generous enough for the worst case on
 *        the firmware side (iterations x its own per-iteration timeout),
 *        with a floor so short tests don't get an unreasonably tight window.
 */
static int timeout_for(uint8_t iterations)
{
    const int per_iteration_worst_case_ms = 1200; /* UUT's own timeout is ~1000ms/iteration */
    const int floor_ms = 3000;
    int computed = (int)iterations * per_iteration_worst_case_ms;
    return computed > floor_ms ? computed : floor_ms;
}

/**
 * @brief Runs one test: builds the command, sends it, times the round
 *        trip, and persists the outcome.
 */
static void run_test(UdpClient *client, RecordStore *store, uint32_t *next_test_id,
                      uint8_t peripheral, uint8_t iterations, const char *pattern)
{
    size_t pattern_len = strlen(pattern);
    if (pattern_len > MAX_PATTERN_LEN) {
        printf("Pattern too long (max %u bytes)\n", MAX_PATTERN_LEN);
        return;
    }

    TestHeader cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.test_id = (*next_test_id)++;
    cmd.peripheral = peripheral;
    cmd.iterations = iterations;
    cmd.pattern_length = (uint8_t)pattern_len;
    memcpy(cmd.bit_pattern, pattern, pattern_len);

    size_t wire_size = sizeof(cmd.test_id) 
                     + sizeof(cmd.peripheral) 
                     + sizeof(cmd.iterations) 
                     + sizeof(cmd.pattern_length) 
                     + cmd.pattern_length;

    char timestamp[TIMESTAMP_LEN];
    current_timestamp(timestamp, sizeof(timestamp));

    printf("Sending test_id=%u peripheral=%s iterations=%d pattern_len=%d\n",
           cmd.test_id, peripheral_id_to_name(peripheral), iterations, cmd.pattern_length);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    ResultPacket response;
    memset(&response, 0, sizeof(response));
    int got_reply = udp_client_send_and_wait(client, &cmd, wire_size, timeout_for(iterations), &response);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    int success = got_reply && response.test_id == cmd.test_id &&
                  response.test_result == TEST_RESULT_SUCCESS;

    if (!got_reply) {
        printf("No response within timeout.\n");
    } else if (response.test_id != cmd.test_id) {
        printf("Warning: response test_id mismatch (got %u)\n", response.test_id);
    } else {
        printf("Response: result=0x%02X %s  [%.3fs]\n",
               response.test_result, success ? "(SUCCESS)" : "(FAILURE)", elapsed);
    }

    TestRecord record;
    record.test_id = cmd.test_id;
    strncpy(record.timestamp, timestamp, TIMESTAMP_LEN - 1);
    record.timestamp[TIMESTAMP_LEN - 1] = '\0';
    strncpy(record.peripheral_name, peripheral_id_to_name(peripheral), PERIPHERAL_NAME_LEN - 1);
    record.peripheral_name[PERIPHERAL_NAME_LEN - 1] = '\0';
    record.duration_seconds = elapsed;
    record.success = success;

    record_store_append(store, &record);
}

/** @brief Runs every peripheral test in sequence, with the given iteration count. */
static void run_all_tests(UdpClient *client, RecordStore *store, uint32_t *next_test_id,
                           uint8_t iterations)
{
    printf("Running tests on ALL peripherals...\n");
    run_test(client, store, next_test_id, PERIPHERAL_TIMER, iterations, "");
    run_test(client, store, next_test_id, PERIPHERAL_UART,  iterations, DEFAULT_PATTERN);
    run_test(client, store, next_test_id, PERIPHERAL_SPI,   iterations, DEFAULT_PATTERN);
    run_test(client, store, next_test_id, PERIPHERAL_I2C,   iterations, DEFAULT_PATTERN);
    run_test(client, store, next_test_id, PERIPHERAL_ADC,   iterations, "");
}

static void print_help(void)
{
    printf(
        "Commands:\n"
        "  all/runAll           - run tests on all peripherals\n"
        "  run <timer|uart|spi|i2c|adc> <iterations> [pattern]\n"
        "  list                 - show all saved test records\n"
        "  show <test_id>       - show one saved test record\n"
        "  empty                - clear all saved test records\n"
        "  iter <number>        - set default number of iterations\n"
        "  help                 - show this message\n"
        "  clear                - clear the terminal\n"
        "  quit                 - exit\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {//
        fprintf(stderr, "Usage: %s <uut_ip> (target IP address)\n", argv[0]);
        return 1;
    }

    const char *uut_ip = argv[1];
    uint16_t port = DEFAULT_PORT;

    UdpClient client;
    if (udp_client_init(&client, uut_ip, port) != 0) {//check for successful initialization of UDP client
        fprintf(stderr, "Failed to initialize UDP client for %s:%u\n", uut_ip, port);
        return 1;
    }

    RecordStore store;
    record_store_init(&store, RECORDS_FILE);//initialize the record store with the specified file path

    /* Continue numbering from the highest test_id already on record, so IDs
     * stay low and easy to type into "show <id>" instead of restarting from
     * a large Unix-timestamp-sized number on every run. */
    uint32_t next_test_id = record_store_max_test_id(&store) + 1;

    printf("Connected to UUT at %s:%u (records saved to %s)\n", uut_ip, port, RECORDS_FILE);
    print_help();

    char line[LINE_BUF_LEN];
    while (1) {
        printf("uut:-> ");
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        line[strcspn(line, "\n")] = '\0'; //change \n to \0

        char cmd_word[TOKEN_LEN] = {0};
        int consumed = 0;
        if (sscanf(line, "%63s%n", cmd_word, &consumed) != 1) {
            continue; // prompt again
        }
        for (int i = 0; cmd_word[i] != '\0'; i++) {
            cmd_word[i] = (char)tolower((unsigned char)cmd_word[i]);
        }
        const char *rest = line + consumed;

        if (strcmp(cmd_word, "quit") == 0 || strcmp(cmd_word, "exit") == 0) {
            break;
        } else if (strcmp(cmd_word, "help") == 0) {
            print_help();
        } else if (strcmp(cmd_word, "clear") == 0) {
            puts("\033[2J\033[H");
        } else if (strcmp(cmd_word, "list") == 0) {
            record_store_print_all(&store);
        } else if (strcmp(cmd_word, "empty") == 0) {
            record_store_empty(&store);
        } else if ((strcmp(cmd_word, "iter") == 0)||(strcmp(cmd_word, "iterations") == 0)) {
            if(sscanf(rest, "%hhu", &iter) == 1) {
                printf("Default iterations set to %hhu\n", iter);
            } else {
                printf("Usage: iter <number>\n");
            }
        } else if (strcmp(cmd_word, "show") == 0) {
            uint32_t id;
            if (sscanf(rest, "%u", &id) == 1) {
                record_store_print_one(&store, id);
            } else {
                printf("Usage: show <test_id>\n");
            }
        } else if ((strcmp(cmd_word, "runall") == 0)||(strcmp(cmd_word, "all") == 0)) {
            run_all_tests(&client, &store, &next_test_id, iter);
            continue;
        } else if (strcmp(cmd_word, "run") == 0) {
            if (rest == NULL || rest[strspn(rest, " \t\r\n")] == '\0') {
                run_all_tests(&client, &store, &next_test_id, iter);
                continue;
            }
            char peripheral_name[TOKEN_LEN] = {0};
            int iterations_int = 0;
            int name_consumed = 0;
            if (sscanf(rest, "%63s%n", peripheral_name, &name_consumed) != 1) {
                printf("Usage: run <timer|uart|spi|i2c|adc> <iterations> [pattern]\n");
                continue;
            }
            const char *after_name = rest + name_consumed;
            int iter_consumed = 0;
            if (sscanf(after_name, "%d%n", &iterations_int, &iter_consumed) != 1) {
                printf("Usage: run <timer|uart|spi|i2c|adc> <iterations> [pattern]\n");
                continue;
            }
            
            const char *pattern = after_name + iter_consumed;
            while (*pattern == ' ') pattern++; // skip leading space before the pattern

            uint8_t peripheral_id;
            if (!peripheral_name_to_id(peripheral_name, &peripheral_id)) {
                printf("Unknown peripheral: %s\n", peripheral_name);
                continue;
            }
            if (iterations_int < 0 || iterations_int > 255) {
                printf("Iterations must be 0-255\n");
                continue;
            }

            run_test(&client, &store, &next_test_id, peripheral_id, (uint8_t)iterations_int, pattern);
        } else {
            printf("Unknown command. Type 'help' for a list.\n");
        }
    }

    udp_client_close(&client);
    return 0;
}