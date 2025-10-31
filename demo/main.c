// File: demo/main.c

#include "my_allocator.h"
#include <stdio.h>
#include <string.h>

typedef struct node {
    int data;
    struct node *next;
} Node;

void demo_malloc_free();
void demo_calloc();
void demo_realloc();

int main() {
    printf("--- Allocator Demo Start ---\n");

    allocator_init();

    demo_malloc_free();
    demo_calloc();
    demo_realloc();

    allocator_destroy();
    printf("--- Allocator Demo End ---\n");
    return 0;
}

void demo_malloc_free() {
    printf("--- Malloc & Free Demo Start ---\n");

    printf("Allocating 3 nodes...\n");
    Node *head = (Node *) my_malloc(sizeof(Node));
    Node *second = (Node *) my_malloc(sizeof(Node));
    Node *third = (Node *) my_malloc(sizeof(Node));

    if (head == NULL || second == NULL || third == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    head->data = 10;
    head->next = second;
    second->data = 20;
    second->next = third;
    third->data = 30;
    third->next = NULL;

    printf("Traversing the linked list...\n");
    Node *current = head;
    while (current != NULL) {
        printf("Data: %d\n", current->data);
        current = current->next;
    }

    printf("Freeing nodes...\n");
    my_free(head);
    my_free(second);
    my_free(third);

    printf("--- Malloc & Free Demo End ---\n");
}

void demo_calloc() {
    printf("--- Calloc Demo Start ---\n");
    size_t count = 10;
    size_t size = sizeof(int);

    printf("Allocating %zu integers...\n", count);
    int *array = (int *) my_calloc(count, size);
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    int zeroed = 1;
    for (size_t i = 0; i < count; i++) {
        if (array[i] != 0) {
            zeroed = 0;
            break;
        }
    }

    if (zeroed) {
        printf("  Success: Memory is zeroed.\n");
    } else {
        printf("  Failure: Memory is not zeroed.\n");
    }

    my_free(array);
    printf("--- Calloc Demo End ---\n");
}

void demo_realloc() {
    printf("---Realloc Demo Start ---\n");

    // Initital allocation
    printf("Allocating 10 integers...\n");
    char *buffer = (char *) my_realloc(NULL, 10);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    strcpy(buffer, "Hello");
    printf(" Buffer content: %s\n", buffer);

    printf("Growing buffer to 50 bytes...\n");
    char *grown_buffer = (char *) my_realloc(buffer, 50);
    if (grown_buffer == NULL) {
        fprintf(stderr, "Realloc (grow) failed!\n");
        my_free(buffer);
        return;
    }

    printf(" Grown Buffer content: %s\n", grown_buffer);

    printf("Shrinking buffer to 20 bytes...\n");
    char *shrunk_buffer = (char *) my_realloc(grown_buffer, 20);
    if (shrunk_buffer == NULL) {
        fprintf(stderr, "Realloc (shrink) failed!\n");
        my_free(grown_buffer);
        return;
    }

    printf(" Shrunk Buffer content: %s\n", shrunk_buffer);

    printf("Freeing final buffer...\n");
    my_free(shrunk_buffer);

    printf("Allocating 10 bytes and freeing with realloc...\n");
    void *temp_ptr = my_malloc(10);
    if (temp_ptr) {
        my_realloc(temp_ptr, 0);
        printf(" Block freed successfully.\n");
    }
    printf("---Realloc Demo End ---\n");
}