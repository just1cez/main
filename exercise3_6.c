#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    char gender[10];
    int birthYear;
    int birthMonth;
    int birthDay;
    char school[50];
    char title[50];
    char job[50];
} person;

int main() {
    person info[5];
    char old[50];
    int year = 9999999;
    for (int i = 0; i < 5; i++) {
        printf("输入第%d个人员的信息:\n", i + 1);
        printf("姓名: ");
        scanf("%s", info[i].name);
        printf("性别: ");
        scanf("%s", info[i].gender);
        printf("出生年份: ");
        scanf("%d", &info[i].birthYear);
        printf("出生月份: ");
        scanf("%d", &info[i].birthMonth);
        printf("出生日: ");
        scanf("%d", &info[i].birthDay);
        printf("所在学院: ");
        scanf("%s", info[i].school);
        printf("职称: ");
        scanf("%s", info[i].title);
        printf("职务: ");
        scanf("%s", info[i].job);
    }
    for (int j = 0;j < 5;j++) {
        if (info[j].birthYear < year) {
            year = info[j].birthYear;
            strcpy(old,info[j].name);
        }
    }
    printf("年龄最大的人是: %s\n", old);
    return 0;
}