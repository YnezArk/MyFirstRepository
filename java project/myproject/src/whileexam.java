public class whileexam {
    public static void main(String[] args) {

        String copyFromMe = "Copy this string until you " +
                "encounter the letter 'g'.";
        StringBuffer copyToMe = new StringBuffer();

        int i = 0;
        char c = copyFromMe.charAt(i);

        while (c != 'n') {
            copyToMe.append(c);
            c = copyFromMe.charAt(++i);
        }
        System.out.println(copyToMe);
        System.out.print(i);
    }
}
class Date {
    int year;
    int month;
    int day;

    void SetDate(int year, int month, int day) {

    }
}