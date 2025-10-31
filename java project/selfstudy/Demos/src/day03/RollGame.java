package day03;

import java.util.Arrays;
import java.util.Random;

/**
 * Author: Ynez Ark
 * Date: 2025/10/31 10:36
 */
public class RollGame {
    public static void main(String[] args) {
        int [] awards = {100,200,500,4,5};
        int [] result = new int[awards.length];
        Random r = new Random();
        for(int i = 0;i<5;) {
            int randomIndex = r.nextInt(awards.length);
            int test = awards[randomIndex];
            boolean flag = contain(result,test);
            if (!flag) {
                result[i] = test;
                i++;
            }
        }
        for (int c : result) {
            System.out.println(c);
        }
    }
    public static boolean contain(int arr[], int test){
        for (int c : arr) {
            if (c == test)
                return true;
        }
        return false;
    }
}
