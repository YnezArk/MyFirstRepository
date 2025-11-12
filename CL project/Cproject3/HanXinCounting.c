/*
    Author: Ynez Ark
    Date: 2025/11/4 23:05
*/


#include <stdio.h>
#include <stdlib.h>

int main(void) {
    system("chcp 65001 > nul");
    int count = 0;
    while (true) {
        if ((count % 5 == 1) && (count % 6 == 5) && (count % 7 == 4) && (count % 11 == 10)) {
            printf("韩信士兵数：%d\n", count);
            break;
        } else {
            count++;
        }
    }


    printf("\n");
    system("pause");
    return 0;
}