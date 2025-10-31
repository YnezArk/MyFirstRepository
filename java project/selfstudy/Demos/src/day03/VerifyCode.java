package day03;

import java.util.Random;

/**
 * Author: Ynez Ark
 * Date: 2025/10/31 09:47
 */
public class VerifyCode {
    public static void main(String[] args) {
        for (int i = 0; i < 5; i++) {
            System.out.println(CodeGenerator());
        }

    }
    public static  String CodeGenerator() {
        String result = "";
        char[] alphabet = new char[52];
        for (int i = 0; i < alphabet.length; i++) {
            if (i <= 25)
                alphabet[i] = (char) (65 + i);
            else
                alphabet[i] = (char) (97 + i - 26);
        }
        Random rand = new Random();
        for (int i = 0; i < 4; i++) {
            int randomIndex = rand.nextInt(alphabet.length);
            result = result + alphabet[randomIndex];
        }
        int randomNumber = rand.nextInt(10);
        result = result + randomNumber;
        return result;
    }
}

