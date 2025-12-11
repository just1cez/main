#include <stdio.h>
#include <string.h>

int main() {
    char s1[50],s2[50];
    scanf("%s",s1);
    scanf("%s",s2);
    int n1[50] = {0},n2[50] = {0};
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    for (int i = 0;i < len1;i++) {
        n1[len1 - 1 - i] = s1[i] - '0';
    }
    for (int j = 0;j < len2;j++) {
        n2[len2 - 1 - j] = s2[j] - '0';
    }
    int len = (len1 > len2? len1 : len2);
    int upper = 0;
    int sum[51] = {0};
    for (int i = 0;i < len;i++) {
        int temp = n1[i] + n2[i] + upper;
        sum[i] = temp % 10;
        upper = temp / 10;
    }
    if (upper) {
        len ++;
        sum[len - 1] = upper;
    }
    for (int p = len - 1;p >= 0;p--) {
        printf("%d",sum[p]);
    }
}