// 排序方法         稳定性	    时间复杂度（最好）	时间复杂度（最坏）	时间复杂度（平均）
// 直接插入排序     稳定	    O(n)	            O(n²)	            O(n²)
// 冒泡排序         稳定	    O(n)	            O(n²)	            O(n²)
// 简单选择排序     不稳定	    O(n²)	            O(n²)	            O(n²)
// 二路归并排序     稳定	    O(n log n)	        O(n log n)	        O(n log n)




// 二、【编程题】实现二路归并排序（Merge Sort）并统计比较次数
// 给定数组：int arr[12] = {38, 27, 43, 3, 9, 82, 10, 55, 19, 4, 7, 15}; 请你实现 二路归并排序（Merge Sort），要求如下：
// 1.使用递归的归并排序（分治策略）
// 2.归并阶段（merge 函数）中：
// 当进行一次关键字比较（if(a <= b)）时，必须计数
// 3.程序应包含 2 个函数：
// void merge(int arr[], int temp[], int left, int mid, int right);
// void mergeSort(int arr[], int temp[], int left, int right);
// 4.主程序应：
// 调用 mergeSort
// 最终输出排序后的数组
// 输出总比较次数
#include <stdio.h>

int compare_count = 0;   // 比较次数（全局计数器）

// 归并操作：将 arr[left..mid] 和 arr[mid+1..right] 合并到 temp[] 再复制回 arr[]
void merge(int arr[], int temp[], int left, int mid, int right) {

    int i = left;
    int j = mid + 1;
    int k = left;

    // TODO: 归并两段有序区间
    // 需要在每次比较时执行： compare_count++;

    // while(i <= mid && j <= right) { ... }
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k] = arr[i];
            k++;
            i++;
        } else {
            temp[k] = arr[j];
            k++;
            j++;
        }
        compare_count++;
    }

    // TODO: 处理剩余元素
    // while (i <= mid) ...
    // while (j <= right) ...
    while (i <= mid) {
        temp[k] = arr[i];
        k++;
        i++;
    }
    while (j <= right) {
        temp[k] = arr[j];
        k++;
        j++;
    }

    // TODO: 将 temp[left..right] 复制回 arr[]
    for (int p = left; p <= right; p++) {
        arr[p] = temp[p];
    }
}


// 二路归并排序
void mergeSort(int arr[], int temp[], int left, int right) {

    if (left >= right) return;

    int mid = (left + right) / 2;

    // TODO: 对左半区递归
    mergeSort(arr,temp,left,mid);
    // TODO: 对右半区递归
    mergeSort(arr,temp,mid+1,right);
    // TODO: 归并左右两半区
    merge(arr,temp,left,mid,right);
}


int main() {

    int arr[12] = {38, 27, 43, 3, 9, 82, 10, 55, 19, 4, 7, 15};
    int n = 12;
    int temp[12];

    mergeSort(arr, temp, 0, n - 1);

    printf("排序结果:");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n比较次数:%d\n", compare_count);

    return 0;
}



