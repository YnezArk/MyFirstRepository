package bymyself;

import java.util.Random;

/**
    Author: YinXueZhou
    Date: 2025/10/23 23:47
*/
public class RandomIndex {
    public static void main(String[] args) {
        int[] arr = {1,2,3,4,5};
        Random index = new Random();
        for (int i = 0; i < arr.length; i++) {
            int randomIndex = index.nextInt(arr.length);
            int temp = arr[i];
            arr[i] = arr[randomIndex];
            arr[randomIndex] = temp;
        }
    for(int i:arr){
        System.out.printf("%d  ",i);
    }
    }
}
