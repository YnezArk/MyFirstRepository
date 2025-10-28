//Created by YinXueZhou on 2025-9-21
public class homework1 {
    public static void main(String[] args) {
        System.out.print("--------------1--------------\n");
        int sum11 =0;
        for( int i=3 ; i<=5 ; i++ ) {
            int temp = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp *= i;
            }
        sum11 += temp;
        }
        System.out.printf("3^3+4^3+5^3=%d\n", sum11);
        int sum12 =6*6*6;
        System.out.printf("6^3=%d\n", sum12);
        if(sum11 == sum12){
            System.out.print("3^3+4^3+5^3=6^3\n");
        }

        System.out.print("--------------2--------------\n");

        int sum21 =0;
        for( int i=6 ; i<=69 ; i++ ) {
            int temp2 = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp2 *= i;
            }
            sum21 += temp2;
        }
        System.out.printf("6^3+7^3+......+69^3=%d\n", sum21);
        int sum22 =180*180*180;
        System.out.printf("180^3=%d\n", sum22);
        if(sum21 == sum22){
            System.out.print("6^3+7^3+......+69^3=180^3\n");
        }

        System.out.print("--------------3--------------\n");


    for(int start =1;start<=40; start++ ) {
        int sum31 =0;
        for( int i=start ; i<=160 ; i++ ) {
            int temp3 = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp3 *= i;
            }
            sum31 += temp3;
            int ans = 1,result;
            do {
                result =(ans * ans * ans);
                if(result == sum31 && start != i){
                    System.out.printf("%d^3+...+%d^3 = %d^3\n", start, i, ans);
                    break;
                } else {
                    ans++;
                }
            } while(result <= sum31);
        }
    }
    }
}
