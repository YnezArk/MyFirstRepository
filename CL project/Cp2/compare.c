//
// Created by 21612 on 2025/7/9.
//
#include "stdio.h"
int main(){
    int a,b,c,max;
    printf("input 3 number\n");
    scanf("%d %d %d",&a,&b,&c);
    if (a > b){
        if(a > c){
            max = a;
        }else{
           max = c;
        }
    } else if (b < c){
            max = c;
        } else{
           max = b;
        }
    printf("max is %d",max);

}