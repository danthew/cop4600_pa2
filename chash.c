#include <pthread.h> // you need to use unix/linux for this not windows
#include "hashdb.h"
#include "rwlocks.h"

void* execute_command(void *args);
void log_command(const char *command, const char *parameters);
void log_lock_acquisition(const char *lock_type);
void log_lock_release(const char *lock_type);
void parse_command(const char *line);
void read_commands(const char *filename);

FILE *output_file;
int thread_count = 0;
int lockA =0;
int lockR =0;

void* execute_command(void *args) {
    char *command = (char *)args;
    parse_command(command);
    free(command);
    return NULL;
}

// Logging functions
void log_command(const char *command, const char *parameters) {
    fprintf(output_file, "%s,%s\n", command, parameters);
}

void log_lock_acquisition(const char *lock_type) {
    fprintf(output_file, "%s LOCK ACQUIRED\n", lock_type);
    lockA++;
}

void log_lock_release(const char *lock_type) {
    fprintf(output_file, "%s LOCK RELEASED\n", lock_type);
    lockR++;
}

// Parsing functions
void parse_command(const char *line) {
    char command[100];
    char name[50];
    uint32_t salary;
    
    sscanf(line, "%s,%[^,],%u", command, name, &salary);

    if (strcmp(command, "insert") == 0) {
        insert(name, salary)
        char parameters[100];
        snprintf(parameters, sizeof(parameters), "%s,%u", name, salary);
        log_command("INSERT", parameters);
        
    } else if (strcmp(command, "delete") == 0) {
        log_command("DELETE", name);
        delete(name)
    } else if (strcmp(command, "search") == 0) {
        log_command("SEARCH", name);
        search(name)
    } else if (strcmp(command, "print") == 0) {
        // Print command
        log_command("PRINT", "0");
        printHashTable()
    } else {
        fprintf(output_file, "Invalid command\n");
    }
}

// Reading commands from file
void read_commands(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(output_file, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    char line[100];
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(output_file, "Error reading first line\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int num_threads;
    if (sscanf(line, "threads,%d,0", &num_threads) != 1) {
        fprintf(output_file, "Error parsing number of threads\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Initialize the number of threads
    if (num_threads <= 0) {
        fprintf(output_file, "Invalid number of threads\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;
        char *command = strdup(line);
        if (pthread_create(&threads[thread_count], NULL, execute_command, command) != 0) {
            fprintf(output_file, "Error creating thread\n");
            free(command);
        }
        thread_count++;
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    fprintf(output_file, "Number of lock acquisitions: %d\n", lockA);
    fprintf(output_file, "Number of lock releases: %d\n", lockR);
    fprintf(output_file, "Final Table:\n");
    printHashTable()
    fclose(file);
}

int main() {
    output_file = fopen(output, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file\n");
        exit(EXIT_FAILURE);
    }
    read_commands("commands.txt");
    return 0;
}