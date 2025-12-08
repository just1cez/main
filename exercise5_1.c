#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stack{
    char letter;
    struct Stack* next;
} Stack;

void push(Stack* head, char letter) {
    Stack* new = (Stack*)malloc(sizeof(Stack));
    new->letter = letter;
    new->next = head->next;
    head->next = new;
}

void pop(Stack* head) {
    if (head->next == NULL) {
        printf("栈为空\n");
        return;
    }
    Stack* temp = head->next;
    printf("%c", temp->letter);
    head->next = temp->next;
    free(temp);
}

int main() {
    Stack* head = (Stack*)malloc(sizeof(Stack));
    head -> next = NULL;
    push(head,'H');
    push(head,'e');
    push(head,'l');
    push(head,'l');
    push(head,'o');
    for (int i = 0; i < 5;i++) {
        pop(head);
    }
}