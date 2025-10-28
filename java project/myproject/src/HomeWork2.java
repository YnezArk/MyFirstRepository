/*
   Created by YinXueZhou on 2025/10/10
*/
public class HomeWork2 {
    public  static void main(String args[]) {
        boolean[] isPrime = new boolean[101];       //0~100共101个序号
        //第一步，将所有数初始化为true，即假设全部都是素数
        for( int i=2 ; i<=100 ; i++ ) {
            isPrime[i] = true;
        }
        //第二步，去掉所有的非素数
        for( int i=2 ; i<=10 ; i++ ) {              //事实上，只需要用到2 3 5 7四个因数
            for(int beishu = 2*i; beishu<=100 ; beishu += i ) {//beishu 即 倍数
                isPrime[beishu] = false;
                                                    /*从2i开始，去掉2 3 5 7的倍数，即
                                                      去掉2+2 2+2+2 2+2+2+2 ...
                                                      去掉6 9 12 15...
                                                      去掉10 15 20 25...*/
            }
        }
        //第三步，用增强for输出结果
        int num = 0;
        for(boolean situation : isPrime ) {
            if(situation) {                         //当situation为true时
                System.out.print(num + " ");
            }
            num++;
        }
    }
}
