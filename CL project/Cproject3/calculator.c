/*
    Author: Ynez Ark
    Date: 2025/10/30 19:50
*/
#include <stdlib.h>

#include "stdio.h"


int main(){
    system("chcp 65001 > nul");
    double num1, num2;
    char op;
    printf("输入算式\n");
    scanf("%lf %c %lf",&num1,&op,&num2);
    switch (op) {
        case '+':
            printf("%lf %c %lf = %lf",num1,op,num2,num1+num2);
            break;
        case '-':printf("%lf %c %lf = %lf",num1,op,num2,num1-num2);
            break;
        case '*':printf("%lf %c %lf = %lf",num1,op,num2,num1*num2);
            break;
        case '/':
            if(num2 != 0) printf("%lf %c %lf = %lf",num1,op,num2,(num1*1.0/num2));
            else printf("除数不能为零!");
            break;
        default:
            printf("不存在的运算符");

    }

     printf("\n");
    system("pause"); // 关键：等待用户按键
    return 0;
}
