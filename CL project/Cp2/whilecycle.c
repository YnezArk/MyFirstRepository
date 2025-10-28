//
// Created by 21612 on 2025/7/9.
//
#include "stdio.h"
int main(){
    int x,n = 0;
    printf("input a number\n");
    scanf("%d",&x);
    while (x > 0){
        n++;
        x/=10;
    }
    printf("length is %d",n);
}