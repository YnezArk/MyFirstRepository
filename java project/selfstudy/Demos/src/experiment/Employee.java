package experiment;

public class Employee extends People{
private int id;
public Employee(int age, String name,String gender, int id) {
	super(age, name, gender);
	this.age = age;
	this.name = name;
	this.gender = gender;
	this.id = id;
}
public int getId() {
	return id;
}
public Employee setId(int id) {
	this.id = id;
	return this;
}
@Override
public void eat() {
	System.out.println("Employee eat");
}
@Override
public void speak() {
	System.out.println("Employee speak");
}
}
