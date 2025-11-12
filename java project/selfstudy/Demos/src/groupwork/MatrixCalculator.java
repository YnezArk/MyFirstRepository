package groupwork;

import java.util.Scanner;

public class MatrixCalculator {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Matrix currentMatrix = null;

        while (true) {
            System.out.println("\n--- 矩阵计算器 ---");
            if (currentMatrix != null) {
                System.out.println("当前矩阵:");
                System.out.println(currentMatrix);
            } else {
                System.out.println("当前没有矩阵。请先输入一个。");
            }

            System.out.println("请选择操作:");
            System.out.println("1. 输入新矩阵");
            System.out.println("2. 计算转置矩阵");
            System.out.println("3. 计算行列式");
            System.out.println("4. 计算逆矩阵");
            System.out.println("5. 退出");
            System.out.print("请输入选项: ");

            String choice = scanner.nextLine();

            try {
                switch (choice) {
                    case "1":
                        currentMatrix = inputMatrix(scanner);
                        System.out.println("矩阵输入成功:");
                        System.out.println(currentMatrix);
                        break;
                    case "2":
                        if (currentMatrix == null) {
                            System.out.println("请先输入矩阵！");
                            break;
                        }
                        Matrix transpose = currentMatrix.transpose();
                        System.out.println("转置矩阵:");
                        System.out.println(transpose);
                        break;
                    case "3":
                        if (currentMatrix == null) {
                            System.out.println("请先输入矩阵！");
                            break;
                        }
                        double det = currentMatrix.determinant();
                        System.out.printf("矩阵行列式: %.2f\n", det);
                        break;
                    case "4":
                        if (currentMatrix == null) {
                            System.out.println("请先输入矩阵！");
                            break;
                        }
                        Matrix inverse = currentMatrix.inverse();
                        System.out.println("逆矩阵:");
                        System.out.println(inverse);
                        break;
                    case "5":
                        System.out.println("感谢使用，再见！");
                        scanner.close();
                        return; // 退出程序
                    default:
                        System.out.println("无效选项，请输入 1-5 之间的数字。");
                }
            } catch (Exception e) {
                // 捕获计算过程中可能出现的异常（如非方阵、奇异矩阵等）
                System.out.println("发生错误: " + e.getMessage());
            }
        }
    }

    /**
     * 核心输入方法
     * 实现了 "空格切换列，回车切换行" 的要求
     * @param scanner 共享的 Scanner 实例
     * @return 用户输入的 Matrix 对象
     */
    private static Matrix inputMatrix(Scanner scanner) {
        int rows = 0;
        int cols = 0;

        // 1. 获取矩阵维度
        while (true) {
            try {
                System.out.print("请输入矩阵的行数 (rows): ");
                rows = Integer.parseInt(scanner.nextLine());
                System.out.print("请输入矩阵的列数 (cols): ");
                cols = Integer.parseInt(scanner.nextLine());
                if (rows > 0 && cols > 0) {
                    break;
                }
                System.out.println("行数和列数必须为正整数。");
            } catch (NumberFormatException e) {
                System.out.println("输入无效，请输入一个数字。");
            }
        }

        double[][] data = new double[rows][cols];
        System.out.println("请输入 " + rows + "x" + cols + " 矩阵数据:");
        System.out.println("(使用 [空格] 分隔同一行的数字, 使用 [回车] 切换到下一行)");

        // 2. 循环读取每一行
        for (int i = 0; i < rows; i++) {
            System.out.print("第 " + (i + 1) + " 行: ");
            String line = scanner.nextLine();

            // 使用正则表达式 "\\s+" 匹配一个或多个空格，作为分隔符
            String[] values = line.trim().split("\\s+");

            for (int j = 0; j < cols; j++) {
                if (j < values.length && !values[j].isEmpty()) {
                    try {
                        data[i][j] = Double.parseDouble(values[j]);
                    } catch (NumberFormatException e) {
                        System.out.println("在 (" + (i+1) + "," + (j+1) + ") 位置输入无效，已记为 0.0");
                        data[i][j] = 0.0;
                    }
                } else {
                    // 如果用户输入的数字不够，自动补 0
                    System.out.println("在 (" + (i+1) + "," + (j+1) + ") 位置未输入，已记为 0.0");
                    data[i][j] = 0.0;
                }
            }
        }

        return new Matrix(data);
    }
}