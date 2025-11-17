#include <stdio.h>

int pows(int n,int k) {
    if (k == 1) {
        return n;
    }
    return n * pows(n,k-1);
}

int main() {
    int n = 0,k = 0;
    printf("请输入n,k(用空格分隔):\n");
    scanf("%d %d",&n,&k);
    int val = pows(n,k);
    printf("%d的%d次方为:%d\n",n,k,val);
}