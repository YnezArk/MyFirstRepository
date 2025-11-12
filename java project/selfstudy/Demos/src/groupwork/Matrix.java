package groupwork;

/**
 * 矩阵类
 * 封装了二维数组，并提供了矩阵的基本运算方法。
 */
public class Matrix {

    private double[][] data;
    private int rows;
    private int cols;

    /**
     * 构造函数
     * @param data 包含矩阵数据的二维数组
     */
    public Matrix(double[][] data) {
        if (data == null || data.length == 0 || data[0].length == 0) {
            throw new IllegalArgumentException("Data cannot be null or empty.");
        }
        this.data = data;
        this.rows = data.length;
        this.cols = data[0].length;
    }

    /**
     * 1. 计算转置矩阵
     * @return一个新的 Matrix 对象，它是原矩阵的转置
     */
    public Matrix transpose() {
        double[][] transposeData = new double[cols][rows];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                transposeData[j][i] = data[i][j];
            }
        }
        return new Matrix(transposeData);
    }

    /**
     * 2. 计算行列式（使用代数余子式展开）
     * @return 行列式的值 (double)
     * @throws IllegalStateException 如果矩阵不是方阵
     */
    public double determinant() {
        if (rows != cols) {
            throw new IllegalStateException("Determinant can only be calculated for square matrices.");
        }
        return calculateDeterminant(this.data);
    }

    // 递归辅助方法，用于计算行列式
    private double calculateDeterminant(double[][] matrix) {
        int n = matrix.length;

        // 基本情况：1x1 矩阵
        if (n == 1) {
            return matrix[0][0];
        }

        // 基本情况：2x2 矩阵
        if (n == 2) {
            return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        }

        // 递归情况：n x n 矩阵（沿第一行展开）
        double det = 0;
        for (int j = 0; j < n; j++) {
            det += Math.pow(-1, j) * matrix[0][j] * calculateDeterminant(getSubmatrix(matrix, 0, j));
        }
        return det;
    }

    /**
     * 3. 计算逆矩阵
     * 逆矩阵 = (1 / 行列式) * 伴随矩阵
     * 伴随矩阵 = 代数余子式矩阵的转置
     * @return 一个新的 Matrix 对象，它是原矩阵的逆
     * @throws IllegalStateException 如果矩阵不是方阵
     * @throws ArithmeticException 如果矩阵是奇异矩阵（行列式为0），没有逆矩阵
     */
    public Matrix inverse() {
        double det = determinant(); // determinant() 方法会检查是否为方阵

        if (Math.abs(det) < 1e-10) { // 检查行列式是否为 0
            throw new ArithmeticException("Matrix is singular (determinant is 0), no inverse exists.");
        }

        int n = rows;
        double[][] cofactorMatrix = new double[n][n];

        // 1. 计算代数余子式矩阵
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double[][] submatrix = getSubmatrix(this.data, i, j);
                cofactorMatrix[i][j] = Math.pow(-1, i + j) * calculateDeterminant(submatrix);
            }
        }

        // 2. 构造伴随矩阵（代数余子式矩阵的转置）
        Matrix adjugateMatrix = new Matrix(cofactorMatrix).transpose();

        // 3. 伴随矩阵 乘以 (1 / 行列式)
        double[][] inverseData = new double[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                inverseData[i][j] = adjugateMatrix.data[i][j] / det;
            }
        }

        return new Matrix(inverseData);
    }

    /**
     * 辅助方法：获取子矩阵（移除指定行和列）
     * @param matrix 原始矩阵数据
     * @param rowToRemove 要移除的行
     * @param colToRemove 要移除的列
     * @return 子矩阵的二维数组
     */
    private double[][] getSubmatrix(double[][] matrix, int rowToRemove, int colToRemove) {
        int n = matrix.length;
        double[][] submatrix = new double[n - 1][n - 1];
        int r = -1;
        for (int i = 0; i < n; i++) {
            if (i == rowToRemove) continue;
            r++;
            int c = -1;
            for (int j = 0; j < n; j++) {
                if (j == colToRemove) continue;
                c++;
                submatrix[r][c] = matrix[i][j];
            }
        }
        return submatrix;
    }

    /**
     * 重写 toString 方法，用于美观地打印矩阵
     */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                // 格式化输出，保留两位小数，并用制表符（\t）分隔
                sb.append(String.format("%.2f\t", data[i][j]));
            }
            sb.append("\n"); // 每行结束后换行
        }
        return sb.toString();
    }
}