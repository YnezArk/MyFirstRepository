import java.util.Scanner;
public class scannertest {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("Enter a number");
        int num = sc.nextInt();
        System.out.printf("%d * %d = %d\n",num,num,num*num);
    }
}
