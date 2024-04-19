# Programming Assignment #2 - Concurrent Hash Table - Group 25
For Programming Assignment #2, we were tasked with implementing a concurrent hash table in the C programming language as an exercise in concurrency, which involved learning
how to implement multi-threading for a program on a specific data structure (in our case, a hash table). The main takeaway of this exercise is to learn how to prevent data
corruption or inconsistency on a shared resource.

## Features
- Insertion
- Deletion
- Search

## Contributors
- Mario Espaillat
- Nicolas Garcia
- Mansur Omary
- Shawnn Kristoffer Ongyiu
- Daniel Thew

## Compilation
Type in the following command into a terminal to compile all files related to the program into an executable:
```bash
make
```

## Execution
Type in the following command into a terminal to run the generated executable:
```bash
./chash
```

## References
- [Jenkins's one_at_a_time hash function](https://en.wikipedia.org/wiki/Jenkins_hash_function)
- [Reader-Writer Lock](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema)
- [common.h and common_threads.h](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/include)
- [Sample Reader-Writer locks implementation](https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c)
