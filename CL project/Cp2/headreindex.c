//
// Created by 21612 on 2025/8/5.
//
#include "stdio.h"
int main(){
    int sign,num;
    int n = 1;
    printf("input a num\n");
    scanf("%d",&num);
    int temp = num;
    do {
        temp /= 10;
        sign = temp;
        n++;
    } while (sign > 10);
    printf("howmany:%d\n",n);
    int result;
    temp = num;
    for (int i = n;i > 0; --i) {
        int divisor = 1;
        for (int j = 1; j < i; j++) {
            divisor *= 10;}
        result = (temp / divisor);
        temp %= divisor;
        printf("%d ",result);
    }
}