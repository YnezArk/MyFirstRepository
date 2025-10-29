/*
    Author: Ynez Ark
    Date: 2025/10/29 23:31
*/
/*
    Author: Ynez Ark
    Date: 2025/10/29 23:04
*/
#include <stdio.h>
#include <windows.h>

int square_sum(int x, int y, int z)
{
    return x * x + y * y + z * z;
}
int main() {
    printf("current console CP = %u\n", GetConsoleOutputCP());
    printf("中文\n");








    printf("中文1\n");

    int a, b, c;

    printf("请输入三个整数（用空格分隔）：");
    scanf("%d %d %d", &a, &b, &c);
    printf("平方和为：%d\n", square_sum(a, b, c));
    return 0;
}