//
// Created by 21612 on 2025/8/12.
//
#include "stdio.h"
int main(){
    int n,digit;
    printf("input a number\n");
    scanf("%d",&n);
    int min=1,max,pow;
    for(int i=1;i<=n-1;i++){
        min *= 10;
    }
    max = min*10;
    printf("min=%d,max=%d\n",min,max);
    for(int i = min;i<max;i++){
        int temp = i;
        int sum = 0;
        do {
            digit = temp%10;
            temp /=10;
            pow = 1;
            for (int k = 1; k <= n; k++) {
                pow *= digit;
            }
            sum += pow;
        } while(temp>0);
        if(sum == i){
            printf("%d\n",i);
        }
    }
}