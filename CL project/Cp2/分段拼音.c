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
            case 0: printf("��"); break;
            case 1: printf("һ"); break;
            case 2: printf("��"); break;
            case 3: printf("��"); break;
            case 4: printf("��"); break;
            case 5: printf("��"); break;
            case 6: printf("��"); break;
            case 7: printf("��"); break;
            case 8: printf("��"); break;
            case 9: printf("��"); break;
        }
        if(temp>9){ printf(" ");}
        temp %= divisor;
    }
}