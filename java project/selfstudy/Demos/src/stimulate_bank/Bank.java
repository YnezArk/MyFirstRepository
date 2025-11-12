package stimulate_bank;

import java.util.ArrayList;

/**
 * Author: Ynez Ark
 * Date: 2025/11/6 12:15
 */
public class Bank {
    private String bankName;  // 银行名
    private ArrayList<Account> accounts;  // 账户列表（使用泛型）
    // 构造函数
    public Bank(String bankName) {
        this.bankName = bankName;
        this.accounts = new ArrayList<Account>();
    }

    // 开户功能
    public void openAccount(String username, String password, double initialBalance) {
        // 检查用户名是否已存在
        for (Account account : accounts) {
            if (account.getUsername() == username ){
                System.out.println("开户失败：用户名 " + username + " 已存在");
                return;
            }
        }
        Account newAccount = new Account(username, password, initialBalance);
        accounts.add(newAccount);
        System.out.println("开户成功！欢迎 " + username + " 加入 " + bankName);
    }

    // 查账户功能
    public Account findAccount(String username) {
        for (Account account : accounts) {
            if (account.getUsername().equals(username)) {
                return account;
            }
        }
        return null;  // 未找到账户
    }

    // 显示所有账户信息
    public void displayAllAccounts() {
        System.out.println("\n=== " + bankName + " 所有账户信息 ===");
        if (accounts.isEmpty()) {
            System.out.println("暂无账户");
        } else {
            for (int i = 0; i < accounts.size(); i++) {
                Account account = accounts.get(i);
                System.out.println((i + 1) + ". 用户名: " + account.getUsername() +
                        ", 余额: " + account.getBalance());
            }
        }
    }

    // Getter方法
    public String getBankName() { return bankName; }
    public ArrayList<Account> getAccounts() { return accounts; }
}
