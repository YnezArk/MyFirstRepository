//
// Created by 21612 on 2025/7/8.
//
#include "stdio.h"
int main(){
    int number,count,sum = 0;
    printf("input numbers\n");
    scanf("%d",&number);
    while (number != -1){
        count++;
        sum += number;
        scanf("%d",&number);
    }
    printf("average is %.2f",1.0*sum/count);
    return 666;
}