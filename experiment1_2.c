#include <stdio.h>
#include <stdlib.h>

int is_leap_year(int year) {
    if ((year % 4 == 0) && !(year % 100 == 0) || (year % 400 == 0)) {
        return 1;
    }
    else {
        return 0;
    }
}

int main() {
    int year,month,day;
    int days = 0;
    int days_in_month = 0;
    int day2 = 0;
    scanf("%4d-%2d-%2d",&year,&month,&day);
    int count = 0;
    if(year < 1990 || month >= 13 || month <= 0 || day <= 0 || day >= 32) {
        printf("Invalid input");
        exit(0);
    }
    for(int i = 1990;i < year;i++) {
        if(is_leap_year(i)) {
            count++;
        }
    }
    days += (365*(year - 1990) + count);
    switch(month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            days_in_month = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            days_in_month = 30;
            break;
        case 2:
            days_in_month = is_leap_year(year) ? 29:28;
            break;
        }
    if (days_in_month < day) {
        printf("Invalid input");
        exit(0);
    }
    for (int j = 1;j < month;j++) {
        switch(j) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                days_in_month = 31;
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                days_in_month = 30;
                break;
            case 2:
                days_in_month = is_leap_year(year) ? 29:28;
                break;
        }
        if(day > days_in_month) {
        printf("Invalid input");
        exit(0);
    }
        days += days_in_month;
    }
    days += day;
    switch (days % 5) {
        case 1:
        case 2:
        case 3:
            printf("working");
            break;
        case 4:
        case 0:
            printf("rest");
            break;
    }
}