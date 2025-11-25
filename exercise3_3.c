#include <stdio.h>

void Swap(int *x, int *y)
{
int temp;
temp = *x;
*x = *y;
*y = temp;
}

void Transpose(int n,int a[n][n]) {
    for (int i = 0;i < n;i++) {
        for (int j = i + 1;j < n;j++) {
            Swap(&a[i][j],&a[j][i]);
        }
    }
}

void PrintMatrix(int n,int a[n][n])
{
	int i, j;
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			printf("%4d", a[i][j]);
		}
	    printf("\n");
	}
}

int main() {
    int n = 0;
    printf("Input n:");
    scanf("%d",&n);
    int a[n][n];
    printf("Input %d*%d matrix a:\n",n);
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			scanf("%d",&a[i][j]);
		}
	}
    PrintMatrix(n,a);
}