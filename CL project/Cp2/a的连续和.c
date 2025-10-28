//
// Created by FunnyBoat on 2025/8/21.
//
#include "stdio.h"
int main(){
    int a,n;
    int sum = 0,sum0;
    printf("input a and n\n");
    scanf("%d %d",&a,&n);
    for(int i = 1;i<=n;i++){
        sum0 = 0;
        for (int j = 1; j <=i ; ++j) {
            int ten =1;
            for(int k = j-1;k>=0;k--){
            if(k>0){ten *= 10;}}
            int temp = a;
            sum0 += (temp * ten);
        }
        sum += sum0;
    }
    printf("sum is %d",sum);
}