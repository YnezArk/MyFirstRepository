//
// Created by 21612 on 2025/8/12.
//
#include "stdio.h"
int main(){
    for(int i = 1;i<=9;i++){
        for(int j = 1;j<=i;j++){
            printf("%d*%d=%d",j,i,i*j);
            if(i*j<10){
                printf("    ");
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
}