//
// Created by 21612 on 2025/7/9.
//
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
int main(){
    srand(time(0));
    int answer = rand()%100+1;
    int inp,count = 0;
    printf("guess a number\n");
    do {
        count++;
        scanf("%d",&inp);
        if (inp > answer){
            printf("toooo big!\n");
        } else if(inp < answer ){
            printf("toooo small!\n");
        }
    } while (inp != answer);
    printf("fk,you got the correct answer in %d times",count);
    return 0;
}