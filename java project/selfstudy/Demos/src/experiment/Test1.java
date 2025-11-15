package experiment;

public class Test1 {
 public static void main(String[] args) {
	Employee employee1 = new Employee(20, "李斯特","男",4445);
	System.out.println("年龄："+employee1.getAge());
	System.out.println("姓名："+employee1.getName());
	System.out.println("性别："+employee1.getGender());
	System.out.println("ID："+employee1.getId());
	employee1.setAge(60);
	System.out.println("set之后年龄："+employee1.getAge());
	employee1.setName("张得美");
	System.out.println("set之后名字："+employee1.getName());
	employee1.setName("女");
	System.out.println("set之后性别："+employee1.getName());
	employee1.setId(114514);
	System.out.println("set之后ID："+employee1.getId());
	employee1.eat();
	employee1.speak();
	
 }
}
