#include <stdio.h>

int count = 0;

int countSteps(int n) {
    int num = n;
    if (num == 1) {
        printf("1");
        return count;
    }
    if (num % 2) {
        printf("%d -> ",num);
        num = 3 * num + 1;
        countSteps(num);
        count++;
    }
    else {
        printf("%d -> ",num);
        num /= 2;
        countSteps(num);
        count++;
    }
    return count;
}

int main() {
    printf("变换过程:");
    printf("\n递归次数:%d",countSteps(10));
}