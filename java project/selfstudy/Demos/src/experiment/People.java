package experiment;

public abstract class People {
 protected int age;
 protected String name,gender;
 public People(int age, String name,String gender) {
	 this.age = age;
	 this.name = name;
	 this.gender = gender;
 }
 public int getAge(){
	 return age;
 }
 public String getName(){
	 return name;
 }
 public String getGender(){
	 return gender;
 }
 public People setAge(int age) {
	 this.age = age;
	 return this;
 }
 public People setName(String name) {
	 this.name = name;
	 return this;
 }
 public People setGender(String gender) {
	 this.gender = gender;
	 return this;
 }
 public abstract void eat();
 public abstract void speak();
}
