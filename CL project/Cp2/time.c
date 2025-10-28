//
// Created by 21612 on 2025/7/8.
//
#include "stdio.h"
int main(){
    int h1,min1;
    int h2,min2;
    printf("input your time\n");
    scanf("%d %d",&h1,&min1);
    scanf("%d %d",&h2,&min2);
    int t = (h2 * 60 +min2)-(h1 * 60 + min1);
    printf("timerange is %d hours %d minutes",t / 60,t % 60);
    return 0;
}