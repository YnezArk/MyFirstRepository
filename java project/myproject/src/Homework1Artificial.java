/*
   Created by YinXueZhou on 2025/10/4
*/

public class Homework1Artificial {
    public static void main(String[] args) {
        System.out.println("验证题目中的两个等式：");
        System.out.println("==================================");

        // 验证第一个等式：3^3 + 4^3 + 5^3 = 6^3
        verifyEquation1();

        // 验证第二个等式：6^3 + 7^3 + ... + 69^3 = 180^3
        verifyEquation2();

        System.out.println("\n寻找更多类似的等式：");
        System.out.println("==================================");

        // 寻找更多连续整数的立方和等于另一个整数的立方
        findMoreEquations(1000);
    }

    // 验证第一个等式：3^3 + 4^3 + 5^3 = 6^3
    public static void verifyEquation1() {
        long leftSum = cube(3) + cube(4) + cube(5);
        long rightValue = cube(6);

        System.out.println("第一个等式验证：");
        System.out.println("3^3 + 4^3 + 5^3 = " + cube(3) + " + " + cube(4) + " + " + cube(5) + " = " + leftSum);
        System.out.println("6^3 = " + rightValue);
        System.out.println("是否成立: " + (leftSum == rightValue));
        System.out.println();
    }

    // 验证第二个等式：6^3 + 7^3 + ... + 69^3 = 180^3
    public static void verifyEquation2() {
        long leftSum = 0;
        for (int i = 6; i <= 69; i++) {
            leftSum += cube(i);
        }
        long rightValue = cube(180);

        System.out.println("第二个等式验证：");
        System.out.println("6^3 + 7^3 + ... + 69^3 = " + leftSum);
        System.out.println("180^3 = " + rightValue);
        System.out.println("是否成立: " + (leftSum == rightValue));
        System.out.println();
    }

    // 寻找更多连续整数的立方和等于另一个整数的立方
    public static void findMoreEquations(int maxN) {
        int foundCount = 0;

        // 遍历所有可能的连续序列
        for (int start = 1; start < maxN; start++) {
            for (int end = start + 2; end < maxN; end++) { // 至少3个数
                long sum = 0;

                // 计算从start到end的立方和
                for (int i = start; i <= end; i++) {
                    sum += cube(i);
                    if (sum < 0) break; // 防止溢出
                }

                if (sum < 0) continue;

                // 检查sum是否是一个整数的立方
                long k = (long) Math.round(Math.cbrt(sum));
                if (k * k * k == sum && k > end) {
                    foundCount++;
                    System.out.println("发现第 " + foundCount + " 个等式:");
                    System.out.print(start + "^3");
                    for (int i = start + 1; i <= end; i++) {
                        System.out.print(" + " + i + "^3");
                    }
                    System.out.println(" = " + k + "^3");
                    System.out.println("验证: " + sum + " = " + (k * k * k));
                    System.out.println();

                    if (foundCount >= 10) { // 限制输出数量
                        return;
                    }
                }
            }
        }

        if (foundCount == 0) {
            System.out.println("在范围内未找到更多等式");
        }
    }

    // 计算立方
    public static long cube(int n) {
        return (long) n * n * n;
    }
}