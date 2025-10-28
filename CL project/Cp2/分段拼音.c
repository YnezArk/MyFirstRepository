//
// Created by FunnyBoat on 2025/8/18.
//
#include "stdio.h"
int main(){
    int sign,num;
    int n = 1;
    printf("input a num\n");
    scanf("%d",&num);
    if(num < 0){
        printf("fu ");
        num *= -1;
    }
    int temp = num;
    do {
        temp /= 10;
        sign = temp;
        n++;
    } while (sign > 10);
    int result;
    temp = num;
    for (int i = n;i > 0; --i) {
        int divisor = 1;
        for (int j = 1; j < i; j++) {
            divisor *= 10;}
        result = (temp / divisor);
        switch (result) {
            case 0: printf("零"); break;
            case 1: printf("一"); break;
            case 2: printf("二"); break;
            case 3: printf("三"); break;
            case 4: printf("四"); break;
            case 5: printf("五"); break;
            case 6: printf("六"); break;
            case 7: printf("七"); break;
            case 8: printf("八"); break;
            case 9: printf("九"); break;
        }
        if(temp>9){ printf(" ");}
        temp %= divisor;
    }
}