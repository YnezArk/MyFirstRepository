/**
    Author: YinXueZhou
    Date: 2025/10/23 21:29
*/
package bymyself;

import java.util.Random;
import java.util.Scanner;

public class GuessRandom {
    public static void main(String[] args) {
        Random rand = new Random();
        int answer = rand.nextInt(100)+1;
        int times = 0;

        do {
            System.out.print("请输入1-100数字：");
            Scanner sc = new Scanner(System.in);
            int guessNumber = sc.nextInt();
            times++;
            if(answer == guessNumber) {
                System.out.printf("猜对了，用的次数：%d",times);
                return;
            } else if(answer < guessNumber) {
                System.out.print("猜大了\n");
            }  else {
                System.out.print("猜小了\n");
            }
        } while(true);
    }
}
