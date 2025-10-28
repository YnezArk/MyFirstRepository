//
// Created by FunnyBoat on 2025/8/21.
//
#include "stdio.h"
int main(){
    int a,n;
    printf("input a and n\n");
    scanf("%d %d",&a,&n);
    int sum = 0;
    int temp = 0;
    for (int i = 0; i < n; ++i) {
        temp = temp*10+a;
        sum +=temp;
    }
    printf("%d",sum);
}