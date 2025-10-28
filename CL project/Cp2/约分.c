//
// Created by FunnyBoat on 2025/8/18.
//
#include "stdio.h"
int main(){
    int dividend,divisor;
    printf("input num1/num2\n");
    scanf("%d/%d",&dividend,&divisor);
    int a = dividend,b = divisor,t;
    while (b != 0){
        t = a % b;
        a= b;
        b= t;
    }
    printf("%d/%d",dividend /= a,divisor /= a);
}