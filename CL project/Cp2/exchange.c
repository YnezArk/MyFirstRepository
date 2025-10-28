//
// Created by 21612 on 2025/7/8.
//
#include "stdio.h"
int main(){
    int a = 6;
    int b = 5;
    int c;
    c = a;
    a = b;
    b = c;
    printf("\na=%d \nb=%d\n",a,b);
    fflush(stdout);
    return 666;
}