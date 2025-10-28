package bymyself;

/**
    Author: YinXueZhou
    Date: 2025/10/23 22:34
*/
public class Swap {
    public static void main(String[] args) {
        int[] array = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (int i = 0, j = array.length - 1; i < j; i++, j--) {
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
        for(int i: array) {
            System.out.print(i);
        }
    }
}