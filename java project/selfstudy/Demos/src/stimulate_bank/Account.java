package stimulate_bank;

/**
 * Author: Ynez Ark
 * Date: 2025/11/6 12:00
 */
public class Account {
    private String username;
    private String password;
    private double balance;
    public Account(String username, String password, double balance) {
        this.username = username;
        this.password = password;
        this.balance = balance;
    }
    public void deposit(double amount) {
        if (amount > 0) {
            this.balance += amount;
            System.out.println(this.username+ "存款" +amount + "成功，当前余额： " + this.balance);
        } else {
            System.out.println("金额必须大于零");
        }
    }
    public void withdraw(double amount) {
        if (amount > 0) {
            if (this.balance >= amount) {
                this.balance -= amount;
                System.out.println(this.username+ "取款" +amount + "成功，当前余额： " + this.balance);
            } else {
                System.out.println("余额不足");
            }

        } else {
            System.out.println("取款金额必须大于零");
        }
    }
    public void showBalance() {
        System.out.println("用户"+this.username +"当前余额："+this.balance);
    }
    public String getUsername() {
        return username;
    }
    public void setUsername(String username) {
        this.username = username;
    }
    public String getPassword() {
        return password;
    }
    public void setPassword(String password) {
        this.password = password;
    }
    public double getBalance() {
        return balance;
    }
}
