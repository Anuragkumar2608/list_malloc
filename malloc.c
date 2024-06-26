#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define REQ_SIZE 4096

void *first_block = NULL;

int pages_allocated = 0;
unsigned int curr = 0;

struct block{
    size_t size;
    struct block *next;
    int free;
};

struct block *last_block = NULL;

#define BLOCK_SIZE sizeof(struct block)

void *get_free_page(size_t size){
    void *ptr;
    ptr = sbrk((((size+BLOCK_SIZE)/REQ_SIZE)+1)*REQ_SIZE);
    if(ptr == (void *)-1)return NULL;
    pages_allocated += ((size+BLOCK_SIZE)/REQ_SIZE)+1;
    return ptr;
}

struct block *find_free_block(size_t size){
    struct block *current = first_block;
    while(current && !(current->free && current->size >= size)){
        last_block = current;
        current = current->next;
    }
    return current;
}

struct block *get_space(size_t size){
    struct block *new_block;
    void *request;
    if((curr + size + BLOCK_SIZE) > pages_allocated * REQ_SIZE){
        new_block = get_free_page(size);
    }else{
        new_block = (struct block *)((char*)first_block + curr);
    }
    if(request == (void*) -1){
        return NULL;      //Failure on sbrk
    }
    curr += (size + BLOCK_SIZE);
    if(last_block){
        last_block->next = new_block;
    }
    new_block->size = size;
    new_block->next = NULL;
    new_block->free = 0;
    return new_block;
}

void split_block(struct block *curr_block, size_t size){
    if(curr_block->size > size + BLOCK_SIZE){
        void *ptr = (char*)(curr_block) + BLOCK_SIZE + size;
        struct block *new_block = ptr;
        new_block->size = curr_block->size - size - BLOCK_SIZE;
        new_block->next = curr_block->next;
        new_block->free = 1;
        curr_block->next = new_block;
    }
}

void *malloc(size_t size){
    struct block *req_block;
    if(size <= 0)return NULL;
    if(!first_block){
        req_block = get_space(size);
        if(!req_block){
            printf("malloc failed");
            return NULL;
        }
        first_block = (void *)req_block;
    }else{
        req_block = find_free_block(size);
        if(!req_block){
            req_block = get_space(size);
            if(!req_block){
                printf("malloc failed");
                return NULL;
            }
        }else{
            split_block(req_block, size);
            req_block->free = 0;
        }
    }
    return (req_block+1);
}

struct block *get_block_ptr(void *ptr){
    return (struct block *)ptr - 1;
}

void free(void *ptr){
    if(!ptr){
        return;
    }
    struct block *free_ptr = get_block_ptr(ptr);
    free_ptr->free = 1;
}

struct tint{
    int arr[2000];
};

int main(){
    int *intptr = (int *)malloc(sizeof(int));
    struct block *ptr;
    *intptr = 2;
    printf("intptr:%p\n",intptr);
    free(intptr);
    ptr = (struct block*)((char *)intptr - BLOCK_SIZE);
    printf("ptr->free:%d\n",ptr->free);
    intptr = malloc(sizeof(int));
    struct tint *tintptr = malloc(sizeof(struct tint));
    printf("tintptr:%p\n",tintptr);
    free(tintptr);
    ptr = (struct block*)((char *)tintptr - BLOCK_SIZE);
    printf("ptr->free:%d\n",ptr->free);
    intptr = malloc(sizeof(int));
    printf("intptr:%p\n",intptr);
    printf("ptr->free:%d\n",ptr->free);
    intptr = malloc(sizeof(int));
    printf("intptr:%p\n",intptr);
    intptr = malloc(sizeof(int));
    printf("intptr:%p\n",intptr);
    return 0;
}
