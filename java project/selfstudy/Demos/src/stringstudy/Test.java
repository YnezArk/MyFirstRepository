package stringstudy;

import java.util.Locale;
import java.util.StringJoiner;

/**
 * Author: Ynez Ark
 * Date: 2025/11/13 19:48
 */
public class Test {
    public static void main(String[] args) {
        String arr[] = {"STr","stR"};
        String name = new String("HelloWorld");
        StringBuilder builder = new StringBuilder();
        builder.append(name).reverse();
        System.out.println(builder.toString());
        StringJoiner sj = new StringJoiner(", ","[","]");
        sj.add(name);
        for (String s : arr) {
            sj.add(s+"").add("aaa".toUpperCase(Locale.ROOT)).add("bbb".toUpperCase(Locale.ROOT));
        }

        System.out.println(sj.toString());
        System.out.printf("%c",97);
    }

}
