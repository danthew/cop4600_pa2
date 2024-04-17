#include "hashdb.h"
#include "rwlocks.h"

#include <stdio.h>
#include <stdlib.h>

// head of hash table list
hashRecord *hashTable = NULL;

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

    hashRecord *current = hashTable;
    hashRecord *prev = NULL;

    // search for existing key and update if found
    while (current != NULL)
    {
        if (current->hash == hash && strcmp(current->name, name) == 0)
        {
            current->salary = salary;         // update the existing record
            rwlock_release_writelock(&mutex); // release the write lock
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
}

void delete(char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    rwlock_acquire_writelock(&mutex);

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
}

hashRecord* search(char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    rwlock_acquire_readlock(&mutex);

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

    return result;
}

void printHashTable()
{
    rwlock_acquire_readlock(&mutex);

    hashRecord *current = hashTable;
    while (current != NULL)
    {
        printf("%u, %s, %u\n", current->hash, current->name, current->salary);
        current = current->next;
    }

    rwlock_release_readlock(&mutex);
}
