public class test1 {
    public static void main(String[] args) {
        System.out.print("第一题：\n");
        int sumFirst =0;
        for( int i=3 ; i<=5 ; i++ ) {
            int temp = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp *= i;
            }
        sumFirst += temp;
        }

        int answerFirst =6*6*6;

        if(sumFirst == answerFirst){
            System.out.print("3^3+4^3+5^3=6^3\n");
        }

        System.out.print("第二题：\n");

        int sunSecond =0;
        for( int i=6 ; i<=69 ; i++ ) {
            int temp2 = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp2 *= i;
            }
            sunSecond += temp2;
        }

        int answerSecond =180*180*180;

        if(sunSecond == answerSecond){
            System.out.print("6^3+7^3+......+69^3=180^3\n");
        }

        System.out.print("第三题：\n");


    for(int start =1;start<=40; start++ ) {
        int sumThird =0;
        for( int i=start ; i<=100 ; i++ ) {
            int temp3 = 1;
            for( int j=1 ; j<=3 ; j++ ) {
                temp3 *= i;
            }
            sumThird += temp3;
            int ans = 1,result;
            do {
                result =(ans * ans * ans);
                if(result == sumThird && start != i){
                    System.out.printf("%d^3+...+%d^3 = %d^3\n", start, i, ans);
                    break;
                } else {
                    ans++;
                }
            } while(result <= sumThird);
        }
    }
    }
}
