#include <pthread.h> // you need to use unix/linux for this not windows
#include <stdio.h>
#include <stdlib.h>
#include "hashdb.h"
#include "rwlocks.h"
#include <string.h>
#include <stdint.h>
FILE *output_file;
int thread_count = 0;


void* execute_command(void *args);
void log_command(const char *command, const char *parameters);
void log_lock_acquisition(const char *lock_type);
void log_lock_release(const char *lock_type);
void parse_command(const char *line);
void read_commands( const char *filename);


void* execute_command(void *args) {
    char *command = (char *)args;
    parse_command(command);
    free(command);
    return NULL;
}

void log_command(const char *command, const char *parameters) {
    fprintf(output_file, "%s,%s\n", command, parameters);
}


void parse_command(const char *line) {
    char line_copy[100]; // Create a copy of the line
    strcpy(line_copy, line);
    char command[100];
    char name[50];
    uint32_t salary;
    char *token = strtok(line_copy, ",");
    strcpy(command, token);
    token = strtok(NULL, ",");
    strcpy(name, token);
    token = strtok(NULL, ",");
    salary = atoi(token);
    //printf( "%s %s %d \n",command,name,salary);
    

    if (strcmp(command, "insert") == 0) {
        char parameters[100];
        snprintf(parameters, sizeof(parameters), "%s,%u", name, salary);
        log_command("INSERT", parameters);
        insert(name, salary);
        
        
    } else if (strcmp(command, "delete") == 0) {
        log_command("DELETE", name);
        delete(name);
    } else if (strcmp(command, "search") == 0) {
        log_command("SEARCH", name);
        hashRecord *h = search(name);
        fprintf(output_file, "%d,%s,%d\n", h->hash, h->name, h->salary);
    } else if (strcmp(command, "print") == 0) {
        printHashTable(output_file);
    } else {
        fprintf(output_file, "Invalid command\n");
    }
}

void read_commands( const char *filename) {
    
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
    fprintf(output_file, "Running %d threads\n", num_threads);
    // Initialize the number of threads
    if (num_threads <= 0) {
        fprintf(output_file, "Invalid number of threads\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    rwlock_init(&mutex);
    setFile(output_file);
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
    fprintf(output_file, "Number of lock acquisitions: %d\n", getLockA());
    fprintf(output_file, "Number of lock releases: %d\n", getLockR());
    fprintf(output_file, "Final Table:\n");
    printHashTable(output_file);
    free(threads);
    fclose(file);
}
int main() {
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file\n");
        exit(EXIT_FAILURE);
    }
    
    read_commands("commands.txt");
    fclose(output_file);
    return 0;
}