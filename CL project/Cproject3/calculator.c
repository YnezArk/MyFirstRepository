/*
    Author: Ynez Ark
    Date: 2025/10/30 19:50
*/
#include "stdio.h"
#include "stdlib.h "

int main(){
    int num1, num2;
    char op;
    printf("输入算式\n");
    scanf("%d %c %d",&num1,&op,&num2);
    switch (op) {
        case '+':
            printf("%d %c %d = %d",num1,op,num2,num1+num2);
            break;
        case '-':printf("%d %c %d = %d",num1,op,num2,num1-num2);
            break;
        case '*':printf("%d %c %d = %d",num1,op,num2,num1*num2);
            break;
        case '/':
            if(num2 != 0) printf("%d %c %d = %f",num1,op,num2,(num1*1.0/num2*1.0));
            else printf("除数不能为零!");
            break;
        default:
            printf("不存在的运算符");

    }

    printf("\n");
    system("pause");   // 关键：等待用户按键
    return 0;
}
