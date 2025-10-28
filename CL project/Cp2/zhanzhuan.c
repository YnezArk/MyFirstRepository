//
// Created by 21612 on 2025/8/5.
//
#include "stdio.h"
int main(){
    int a,b,t;
    printf("input 2 numbers\n");
    scanf("%d %d",&a,&b);
    while (b != 0){
        t = a % b;
        a= b;
        b= t;
    }
    printf("%d",a);
}