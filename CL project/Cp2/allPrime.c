/*
    Author: Ynez Ark
    Date: 2025/11/4 10:09
*/


#include <stdio.h>

int isPrimeF2(int num){
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
    for (int i = 2; i <= 1000; i++) {
        if (isPrimeF2(i)){
            printf("%d  ",i);
        }
    }
}