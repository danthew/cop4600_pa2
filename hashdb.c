#include "hashdb.h"
#include "rwlocks.h"

#include <stdio.h>

hashRecord *hashTable = NULL;

uint32_t jenkins_one_at_a_time_hash(char *key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length)
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
}

void delete(char *name)
{
}

uint32_t search(char *name)
{
}

void printHashTable()
{
}
