// 请用C语言实现：铁路购票系统座位分配算法，用于处理一节车厢的座位分配。
// 规则如下：假设一节车厢有20排，每一排有5个座位，用A、B、C、D、F表示，第一排就是1A、1B、1C、1E、1F，
// 第二排就是2A、2B、2C、2D、2F，以此类推，购票时，每次最多购买5张，
// 座位分配策略师：如果这几张票能安排在同一排相邻的座位，则安排在编号最小的相邻座位，
// 否则，安排在编号最小的几个空座位中（不考虑是否相邻）
#include <stdio.h>

#define ROW 20
#define COL 5

char seatName[COL] = {'A', 'B', 'C', 'D', 'F'};

int seat[ROW][COL] = {0};

void allocate(int n) {
    int found = 0;
    // 第一部分：寻找同一排里连续 n 个空位
    for (int r = 0; r < ROW; r++) {

        int freeCount = 0;      // 当前已连续空座数量
        int startCol = 0;       // 连续空座的开始位置

        for (int c = 0; c < COL; c++) {
            if (seat[r][c] == 0) {
                freeCount++;
                if (freeCount == 1) {
                    startCol = c;   // 连续空座开始位置
                }

                if (freeCount == n) {
                    // 找到了连续 n 个座位！
                    for (int k = 0; k < n; k++) {
                        seat[r][startCol + k] = 1;
                        if (k == n - 1) {
                            printf("%d%c", r + 1, seatName[startCol + k]);
                        }
                        else {
                            printf("%d%c ", r + 1, seatName[startCol + k]);
                        }
                    }
                    printf("\n");
                    found = 1;
                    return;
                }
            } 
            else {
                freeCount = 0;
            }
        }
    }

    // 第二部分：没找到连续的 → 找最小编号座位(先行后列遍历空座位)
    if (!found) {

        int assigned = 0;

        for (int r = 0; r < ROW; r++) {
            for (int c = 0; c < COL; c++) {

                if (seat[r][c] == 0) {
                    seat[r][c] = 1;
                    if (assigned == n - 1) {
                        printf("%d%c", r + 1, seatName[c]);
                    }
                    else {
                        printf("%d%c ", r + 1, seatName[c]);
                    }
                    assigned++;
                    if (assigned == n) {
                        printf("\n");
                        return;
                    }
                }
            }
        }
    }
}

int main() {
    int count = 0;
    scanf("%d",&count);
    for (int i= 0; i < count;i++) {
        int n = 0;
        scanf("%d", &n);
        allocate(n);
    }
    return 0;
}