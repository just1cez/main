#include <stdio.h>

int lcm(int a,int b) {
    int result = (a >= b)?a:b;
    while (!(result % a == 0 && result % b == 0)) {
        result++;
    }
    return result;
}

int main() {
    int a = 0,b = 0;
    printf("请输入两个数(用空格分隔):\n");
    scanf("%d %d",&a,&b);
    int val = lcm(a,b);
    printf("最小公倍数为:%d\n",val);
}