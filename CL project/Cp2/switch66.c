//
// Created by 21612 on 2025/7/9.
//
#include "stdio.h"
int main(){
    int type,grades;
    char level;
    printf("input grades\n");
    scanf("%d",&grades);
    type = grades/10;
    switch (type) {
        case 10:
        case 9:
            level = "A";
            break;
        case 8:
            level = "B";
            break;
        case 7:
            level = "C";
            break;
        case 6:
            level = "D";
            break;
        case 5:
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            level = (char) "E";
            break;
        default:
            printf("not available");
            break;
    }
    printf("level is %c",level);
    return 0;
}