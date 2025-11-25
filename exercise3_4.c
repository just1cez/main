#include <stdio.h>

unsigned int MyStrlen(char p[10000]) {
    unsigned int len;
    len=0;
    for (int i = 0;p[len] != '\0';i++) {
        len++;
    }
    return len;
}

int main() {
    char a[10000] = {};
    printf("请输入一个字符串:");
    scanf("%s",a);
    printf("%u",MyStrlen(a));
}