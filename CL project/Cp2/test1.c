//
// Created by 21612 on 2025/8/12.
//
#include "stdio.h"
int main() {
    int num = 0;
    printf("input a number\n");
    scanf("%d", &num);
    int i, j, k;
    int cnt = 0;
    for (i = num; i <= num + 3; i++) {
        for (j = num; j <= num + 3; j++) {
            for (k = num; k <= num + 3; k++) {
                if (i != j && i != k && j != k) {
                    cnt++;
                    printf("%d%d%d", i, j, k);
                    if (cnt == 6) {
                        printf("\n");
                        cnt = 0;
                    } else {
                        printf(" ");
                    }
                }
            }
        }
    }
}