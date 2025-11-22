package javaExperiment;

import java.util.Scanner;

/**
 * Author: Ynez Ark
 * Date: 2025/11/22 15:12
 */
public class ThrowTest {
    public static void main(String[] args) {
        Week week = new Week();
        Scanner sc = new Scanner(System.in);
        System.out.println("请输入数字1-7");

        boolean flag = true;
        String result ="";
        int index=-1;
        String user_input = null;
        do {
            user_input=sc.nextLine();
            try {
                index=Integer.parseInt(user_input)-1;
                if(index<0 || index>6){
                    System.out.println("只能输入1-7数字");
                    flag = false;
                } else {flag =  true;}
            } catch (NumberFormatException e) {
                System.out.println("只能输入数字");
                flag = false;
            }
        } while(!flag);
        result = week.getWeek(index);
        System.out.println("结果是"+result);
        sc.close();
    }
}
