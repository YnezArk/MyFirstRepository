package day04;

import java.util.Scanner;

/**
 * Author: Ynez Ark
 * Date: 2025/11/1 10:21
 */
class Student {
    private String name;
    private String gender;
    private int score;
    private boolean award;
    Student(String name, String gender, int score, boolean award) {
        this.name = name;
        this.gender = gender;
        this.score = score;
        this.award = award;
    }

    @Override
    public String toString() {
        return "Student [name=" + name + ", gender=" + gender + ", score=" + score + ", award=" + award + "]";
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setGender(String gender) {
        this.gender = gender;
    }
    public void setScore(int score) {
        this.score = score;
    }
    public void setAward(boolean award) {
        this.award = award;
    }

    public String getName() {
        return name;
    }
    public String getGender() {
        return gender;
    }
    public int getScore() {
        return score;
    }
    public boolean getAward() {
        return award;
    }
}

public class PTATest2 {
    public static void main(String[] args) {
        Scanner inputInformation = new Scanner(System.in);
        Student student = new Student(inputInformation.next(),inputInformation.next(),inputInformation.nextInt(),inputInformation.nextBoolean());;
        System.out.print(student.toString());
    }
}
