#include <stdio.h>
#include <stdlib.h>

int main() {
    int result = 0;
    char op = '1';
    int num = 0;
    scanf("%d",&result);
    while (1) {
        op = getchar();
        if (op == '=') {
            break;
        }
        scanf("%d",&num);
        switch(op) {
            case '+':
                result += num;
                break;
            case '-':
                result -= num;
                break;
            case '*':
                result *= num;
                break;
            case '/':
                result /= num;
                break;
            default:
                printf("错误的运算符:%c",op);
                exit(0);                 
        }
    }
    printf("%d",result);
}