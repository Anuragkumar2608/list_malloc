Malloc Implementation using Linked List

malloc is a library call used to allocate dynamic memory.

This is a simple malloc implementation using linked list of metadata.

Since, call to free only requires a void * pointer, the malloc function call must store enough information so that when free is called, it can free up the dynamically allocated memory appropriately. The memory that can be returned back to the OS must be contiguous and in this implementation, the freed memory isn't necessarily at the edge of the allocated memory or contiguous, hence we keep a int variable to mark the struct as free.

We use the sbrk system call to allocate memory on the heap, whenever the call to malloc is made with the required size, we call sbrk(0) to get the current address and then we call sbrk(sizeof(block) + size) to get the required space for the block and then we return block + 1 as the return pointer from which memory can manipulated.

we also implement a split function which splits larger sized free blocks into smaller blocks when a free block is allocated for a much smaller size than required.