/*
    Author: Ynez Ark
    Date: 2025/11/12 16:11
*/


#include <stdio.h>

int main(void) {
    int number;
    printf("请输入行数：\n");
    scanf("%d",&number);
    for (int i = 1; i <= number; i++) {
        for (int j = 1; j <= number-i; j++) {
            printf(" ");
        }
        for (int j = 1; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}