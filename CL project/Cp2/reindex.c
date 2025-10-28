//
// Created by 21612 on 2025/7/22.
//
#include "stdio.h"
int reindexF(int x){
    int ret, digit = 0;
    while (x>0){
        digit = x%10;
        ret = ret * 10 + digit;
        x /= 10;
    }
    return ret;
}
int main(){
    
    int x;
    printf("input your number\n");
    scanf("%d",&x);
    printf("%d", reindexF(x));
}