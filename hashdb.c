#include "hashdb.h"
#include "rwlocks.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// head of hash table list
hashRecord *hashTable = NULL;
FILE *out;
uint32_t lockA = 0;
uint32_t lockR = 0;
uint32_t getLockA(){
    return lockA;
}
uint32_t getLockR(){
    return lockR;
}
void setFile(FILE *output_file){
    out = output_file;
}

uint32_t jenkins_one_at_a_time_hash(char *key)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != strlen(key))
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

void insert(char *name, uint32_t salary)
{
    
    uint32_t hash = jenkins_one_at_a_time_hash(name); // compute hash

    // acquire write lock before modifying hash table to
    // prevent other threads from reading/writing
    rwlock_acquire_writelock(&mutex);
    fprintf(out, "WRITE LOCK ACQUIRED\n");
    lockA++;
    hashRecord *current = hashTable;
    hashRecord *prev = NULL;

    // search for existing key and update if found
    while (current != NULL)
    {
        if (current->hash == hash && strcmp(current->name, name) == 0)
        {
            current->salary = salary;         // update the existing record
            rwlock_release_writelock(&mutex); // release the write lock
            fprintf(out, "WRITE LOCK RELEASED\n");
            lockR++;
            return;
        }
        prev = current;
        current = current->next;
    }

    // create new record if not found
    hashRecord *newRecord = malloc(sizeof(hashRecord));
    if (!newRecord)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    strcpy(newRecord->name, name);
    newRecord->salary = salary;
    newRecord->hash = hash;
    newRecord->next = NULL;

    // if prev is NULL list is empty and this is first node
    if (prev == NULL)
    {
        hashTable = newRecord;
    }
    else
    {
        prev->next = newRecord;
    }

    // release write lock
    rwlock_release_writelock(&mutex);
    fprintf(out, "WRITE LOCK RELEASED\n");
    lockR++;
}

void delete(char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    rwlock_acquire_writelock(&mutex);
    fprintf(out, "WRITE LOCK ACQUIRED\n");
    lockA++;


    hashRecord **current = &hashTable; // pointer to the pointer to the head of the list
    while (*current != NULL)
    {
        hashRecord *entry = *current;
        if (entry->hash == hash && strcmp(entry->name, name) == 0)
        {
            *current = entry->next; // remove node from list
            free(entry);
            break;
        }
        current = &entry->next; // move to next node
    }

    rwlock_release_writelock(&mutex);
    fprintf(out, "WRITE LOCK RELEASED\n");
    lockR++;
}

hashRecord* search(char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    rwlock_acquire_readlock(&mutex);
    fprintf(out, "READ LOCK ACQUIRED\n");
    lockA++;

    // search logic
    hashRecord *current = hashTable;
    hashRecord *result = NULL;

    while (current != NULL)
    {
        if (current->hash == hash && strcmp(current->name, name) == 0)
        {
            result = current;
            break;
        }

        current = current->next;
    }

    rwlock_release_readlock(&mutex);
    fprintf(out, "READ LOCK RELEASED\n");
    lockR++;

    return result;
}

void printHashTable()
{    
    rwlock_acquire_readlock(&mutex);
    fprintf(out, "READ LOCK ACQUIRED\n");
    lockA++;

    int hashTableLength = 0;
    hashRecord *current = hashTable;
    while (current != NULL)
    {
        // fprintf(out, "%u, %s, %u\n", current->hash, current->name, current->salary);
        hashTableLength++;
        current = current->next;
    }

    hashRecord records[hashTableLength];
    hashRecord *current2 = hashTable;
    for (int i = 0; current2 != NULL; i++) {
        records[i] = *current2;
        current2 = current2->next;
    }

    for (int i = 0; i < hashTableLength; i++) {
        for (int j = i + 1; j < hashTableLength; j++) {
            if (records[i].hash > records[j].hash) {
                hashRecord temp = records[i];
                records[i] = records[j];
                records[j] = temp;
            }
        }
    }

    for (int i = 0; i < hashTableLength; i++)
    {
        fprintf(out, "%u, %s, %u\n", records[i].hash, records[i].name, records[i].salary);
    }

    rwlock_release_readlock(&mutex);
    fprintf(out, "READ LOCK RELEASED\n");
    lockR++;
}
