#include <stdio.h>
void move(char A,char C) {
    printf("Move disk from %c to %c\n",A,C);
}

void Hanoi(int n,char A,char B,char C) {
    if (n == 1) {
        move(A,C);
    }
    else { 
        Hanoi(n - 1,A,C,B);
        move(A,C);
        Hanoi(n - 1,B,A,C);
    }
}

int main() {
    int n = 0;
    printf("输入n的值:");
    scanf("%d",&n);
    Hanoi(n,'A','B','C');
}