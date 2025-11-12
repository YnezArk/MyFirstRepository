/*
    Author: Ynez Ark
    Date: 2025/11/4 10:09
*/


#include <stdio.h>

int isPrimeF(int num){
    if(num != 1){
        for(int i = 2;i<num ;i++ ){
            if (num % i == 0){
                return 0;
            }
        }
        return 1;
    } else{
        return 0;
    }}
int main() {
    for (int i = 30; i <= 70; i++) {
        if (isPrimeF(i)){
            printf("%d是质数\n",i);
        } else {
            printf("%d是不质数\n",i);
        }
    }
}