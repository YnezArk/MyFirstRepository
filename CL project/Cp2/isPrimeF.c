//
// Created by 21612 on 2025/7/21.
//
#include "stdio.h"
int isPrime(int number){
    int i = 2;
    if (number <= 1){
        return 0;}
    check:
    if (i == number){
        return 1;}
    if (number % i == 0){
        return 0;}
    i++;
    goto check;
    end:
}
int isPrimeF2(int num){
    if(num != 1){
        for(int i = 2;i<num ;i++ ){
            if (num % i == 0){
                return 0;
            }
        }
        return 1;
    } else{
        return 0;
    }}
int main(void){
    int n = 0;
    printf("input a number\n");
    scanf("%d",&n);
    if (isPrimeF2(n)){
        printf("yes\n");
    } else {
        printf("no\n");
    }
    for (int i = 2; i <= 1000; ++i) {
        if (isPrimeF2(i)){
            printf("%d\n",i);
        }

    }
}