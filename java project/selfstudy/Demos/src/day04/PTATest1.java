package day04;

import java.util.Scanner;

/**
 * Author: Ynez Ark
 * Date: 2025/11/1 09:19
 */
class Person{
    private String  name ;
    private boolean gender;
    private int age;
    private int id;
    static int amount=0;
    Person(){
        System.out.println("This is constructor");
    }
    public Person(String name, int age, boolean gender){
        this.name = name;
        this.gender = gender;
        this.age = age;
    }

    public String getName() {
        return name;
    }

    public int getAge() {
        return age;
    }
    public boolean getGender() {
        return gender;
    }
    public int getId() {
        return id;
    }

    @Override
    public String toString() {
        String r = "Person [name=" + name + ", age=" + age + ", gender=" + gender + ", id=" + id + "]";
        return r;
    }
    static {
        System.out.println("This is static initialization block");
    }
    {

        id=amount;
        amount++;
        System.out.println("This is initialization block, id is "+id);
    }
}
public class PTATest1 {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int quantity = sc.nextInt();
        Person[] persons = new Person[quantity];
        for (int i = 0; i < persons.length; i++) {
            persons[i] = new Person(sc.next(), sc.nextInt(), sc.nextBoolean());
        }
        for (int i = quantity-1; i >= 0; i--) {
            System.out.println(persons[i].toString());
        }
        Person person = new Person();
        //这里无参构造的属性是private，直接打不出来，回去加了几个getter
        System.out.println(person.getName()+","+person.getAge()+","+person.getGender()+","+person.getId());
        System.out.println(person.toString());
    }
}
