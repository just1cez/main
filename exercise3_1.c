#include <stdio.h>
#include <stdlib.h>



int main() {
    float scores[40];
    float sum = 0;
    float average = 0;
    int count_1 = 0,count_2 = 0;
    float score = 0;
    while (score >= 0 || count_1 > 40) {
        printf("请输入第%d个学生的成绩(输入负值结束):",count_1 + 1);
        scanf("%f",&score);
        scores[count_1] = score;
        sum += score;
        count_1++;
    }
    sum -= score;
    average = sum / (count_1 - 1);
    for (int j = 0;j < count_1;j++) {
        if (scores[j] > average) {
            count_2++;
        }
    }
    printf("平均分为:%f\n",average);
    printf("成绩高于平均分的学生人数:%d",count_2);
}