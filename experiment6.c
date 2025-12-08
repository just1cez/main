#include <stdio.h>
#include <stdlib.h>

float max(int* scores,int size) {
    int num = 0;
    num = scores[0];
    for(int i = 0;i < size;i++) {
        if (scores[i] > num) {
            num = scores[i];
        }
    }
    return num;
}

float min(int* scores,int size) {
    int num = 0;
    num = scores[0];
    for(int i = 0;i < size;i++) {   
        if (scores[i] < num) {
            num = scores[i];
        }
    }
    return num;
}

float calculate(int* scores,int size) {
    int sum = 0;
    for (int i = 0;i < size;i++) {
        sum += scores[i];
    }
    sum = sum - min(scores,size) - max(scores,size);
    sum /= (size - 2);
    return sum;
}

int main() {
    int m,n;
    int index = 0;
    scanf("%d %d",&n,&m);
    int* score = (int*)malloc(sizeof(int) * m);
    int* scores = (int*)malloc(sizeof(int) * n);
    for (index;index < n;index++) {
        for (int j = 0;j < m;j++) {
            scanf("%d",&score[j]);
        }
        scores[index] = calculate(score,m);
    }
    printf("%.2f",max(scores,n));
}