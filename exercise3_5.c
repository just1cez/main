#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* week[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    printf("请输入英文的星期:");
    char day[100] = {};
    scanf("%s",day);
    for (int i = 0;i < 7;i++) {
        if (strcmp(day,week[i]) == 0) {
            printf("%d\n",i);
            break;
        }
            if (i == 6) {
            printf("-1\n");
        }
    }
}
