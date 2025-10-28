//
// Created by 21612 on 2025/8/1.
//
#include "stdio.h"
int factorialF(int n) {
    if (n == 1) {
        return 1;
    } else {
        int ans;
        ans = n;
        while (n > 1) {
            ans *= (--n);
        }
        return ans;
    }
}
int factorialF2(int n){
    int result = 1;
    //int i = 1;
    for (int i = 2;i<=n;++i){
        result *= i;
    }
    return result;
}
int factorialF3(int n){
    int result = 1;
    for (;n > 1; n--){
        result *= n;
    }
    return result;
}
int main(void){
    int n =0;
    printf("输入数字计算阶乘\n");
    scanf("%d",&n);
    printf("阶乘是 %d",(factorialF3(n)));
}