//
// Created by 21612 on 2025/7/7.
//

#include <stdlib.h>
#include "stdio.h"
int main(void)
{
    system("chcp 65001 > nul");   // 65001 即 UTF-8 代码页
    int price = 0;
    int amount = 0;
    printf("请输入价格：\n");
    scanf("%d",&price);
    printf("请输入票面：\n");
    scanf("%d",&amount);
    int change = amount - price;
    if (change < 0){
        printf("钱不够,还差%d元",-change);
    } else{ if(change == 0){
            printf("刚刚好");
    } else
    printf("找%d元",change);
    }
    return 0;

}