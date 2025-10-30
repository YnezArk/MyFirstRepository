package day02;


/**
 * Author: Ynez Ark
 * Date: 2025/10/30 21:40
 */
public class FindMax {
    public static void main(String[] args) {
        int[] arr={1,5,8,7,9,1,14,44,57};
        int max=findMax(arr);
        System.out.println(max);
    }
    public static int findMax(int[] arr){
        int max=arr[0];
        for(int i=0;i<arr.length;i++){
            if(arr[i]>max){
                max=arr[i];
            }
        }
        return max;
    }
}

