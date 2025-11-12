package stimulate_bank;

/**
 * Author: Ynez Ark
 * Date: 2025/11/6 12:17
 */
public class BankTest {
    public static void main(String[] args) {
        // 创建银行
        Bank bank = new Bank("川农银行");
        System.out.println("=== 欢迎使用 " + bank.getBankName() + " 系统 ===");

        // 开设账户
        System.out.println("\n--- 开设账户 ---");
        bank.openAccount("张三", "123456", 1000);
        bank.openAccount("李四", "654321", 2000);
        bank.openAccount("王五", "111111", 500);

        // 模拟存钱操作
        System.out.println("\n--- 模拟存钱操作 ---");
        Account zhangSan = bank.findAccount("张三");
        if (zhangSan != null) {
            zhangSan.deposit(500);
        }

        Account liSi = bank.findAccount("李四");
        if (liSi != null) {
            liSi.deposit(1000);
        }

        // 模拟取钱操作
        System.out.println("\n--- 模拟取钱操作 ---");
        if (zhangSan != null) {
            zhangSan.withdraw(200);
            zhangSan.withdraw(2000);  // 测试余额不足情况
        }

        if (liSi != null) {
            liSi.withdraw(500);
        }

        // 显示余额
        System.out.println("\n--- 显示余额 ---");
        Account wangWu = bank.findAccount("王五");
        if (wangWu != null) {
            wangWu.showBalance();
        }

        if (zhangSan != null) {
            zhangSan.showBalance();
        }

        if (liSi != null) {
            liSi.showBalance();
        }

        // 显示所有账户信息
        bank.displayAllAccounts();

        // 测试错误情况
        System.out.println("\n--- 测试错误情况 ---");
        bank.openAccount("张三", "999999", 3000);  // 重复开户
        Account notExist = bank.findAccount("赵六");  // 不存在的账户
        if (notExist == null) {
            System.out.println("账户赵六不存在");
        }
    }
}
