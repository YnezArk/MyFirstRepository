package day02;

/**
 * Author: Ynez Ark
 * Date: 2025/10/30 21:58
 */
public class CopyOFRange {
    public static void main(String[] args) {
    int[] arr={1,2,3,4,5,6,7,8,9,10};
    int[] copy = copyOfRange(arr,1,6);
        for (int j : copy) {
            System.out.print(j + ",");
        }
    }
    public static int[] copyOfRange(int[] arr, int from, int to) {
    int[]  res=new int[to-from+1];
    for (int i=0;i<=to-from;i++){
        res[i]=arr[from+i-1];
    }
    return res;
    }
}
