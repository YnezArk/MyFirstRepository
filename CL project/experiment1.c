/*
    Author: Ynez Ark
    Date: 2025/11/4 09:42
*/


#include <stdio.h>
#include <stdlib.h>

int main(void) {


    int sum = 0;
    printf("1到50之间的偶数和: ");
    for (int i = 1; i <= 50; i++) {
        if (i % 2 == 0) {
            sum += i;
        }
    }
    printf("%d ", sum);
}