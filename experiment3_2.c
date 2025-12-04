#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int helper(int* nums, int start, int end, int turn) {
    if (start == end) {
        return turn * nums[start];
    }
    int scoreStart = turn * nums[start] + helper(nums, start + 1, end, -turn);
    int scoreEnd = turn * nums[end] + helper(nums, start, end - 1, -turn);
    if (turn == 1) {
    return scoreStart > scoreEnd ? scoreStart : scoreEnd;
    } 
    else {
    return scoreStart < scoreEnd ? scoreStart : scoreEnd;
    }

}

bool PredictTheWinner(int* nums, int len) {
    return helper(nums, 0, len - 1, 1) >= 0;
}

int main() {
    int n = 0;
    scanf("%d", &n);
    int* nums = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        scanf("%d", &nums[i]);
    }
    bool result = PredictTheWinner(nums, n);
    if (result) {
        printf("true");
    } else {
        printf("false");
    }
    free(nums);
    return 0;
}