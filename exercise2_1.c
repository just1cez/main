#include <stdio.h>

int add(int a,int b) {
    return a+b;
}

int main() {
    int a = 0,b = 0;
    printf("请输入两个加数(用空格分隔):\n");
    scanf("%d %d",&a,&b);
    int result = add(a,b);
    printf("和为:%d\n",result);
}