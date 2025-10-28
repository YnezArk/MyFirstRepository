/*
   Created by YinXueZhou on 2025/10/17
*/
/*简介：
* 有两个类，都是抽象的，其中Manbaout类继承了Bird类
* 只是借用一下Bird的成员，跟鸟没有关系
* fly不知道怎么实现，用print代替
*使用了setter，getter，static，final，abstract，@override
*/
/*
实际输出:
名称:猫猫头,速度:10
名称:Blue,速度:15
名称:猪猪侠,速度:1000,说话！：GG Bond
名称:钢铁侠,速度:10000,说话！：I am Iron Man
*/
//接口
interface Flyable{
    void fly();
}
//抽象 鸟 的类
abstract class Bird implements Flyable{
    private final String name;//名字不可变的final
    private int speed;//速度可变
    public Bird(String name, int speed) {
        this.name = name;
        this.speed = speed;
    }
    public String getName() {//getter 获取名字的方法
        return name;
    }
    public int getSpeed() {//getter 获取速度的方法
        return speed;
    }
    public int setSpeed(int speed) {//setter 设定速度的方法
        this.speed = speed;
        return speed;
    }
    @Override
    public void fly() {
        System.out.printf("名称:%s,速度:%d\n",getName(),getSpeed());
    }
}
//曼巴out 类
abstract class Manbaout extends Bird{
    private final String saying;
    public Manbaout(String name, int speed,String saying) {
        super(name, speed);
        this.saying = saying;
    }
    public String getSaying() {
        return saying;
    }
    @Override
    public void fly() {
        System.out.printf("名称:%s,速度:%d,",getName(),getSpeed());
        System.out.println("说话！："+getSaying());//这里重写的fly增加了说话
    }
}
//实例1：猫头鹰
class Owl extends Bird {
    public Owl(String name, int speed) {
        super(name, speed);
    }
}
//实例2：鹦鹉
class Parrot extends Bird{
    public Parrot(String name, int speed) {
        super(name, speed);
    }
    }
//实例3：猪猪侠
class PigBoy extends Manbaout{
    public PigBoy(String name, int speed, String saying) {
        super(name, speed, saying);
    }
}
//实例4：钢铁侠
class IronMan extends Manbaout{
    public IronMan(String name, int speed, String saying) {
        super(name, speed, saying);
    }
}
public class FlyWorld {
    public   static void main(String args[]) {
    Owl player1 = new Owl("猫猫头",10);        //实例化
    Parrot player2 = new Parrot("Blue",15);
    PigBoy player3 = new PigBoy("猪猪侠",20,"GG Bond");
    IronMan player4 = new IronMan("钢铁侠",10000,"I am Iron Man");
    player3.setSpeed(1000);//调用setter
    player1.fly();                              //调用飞行方法，也许可以用升级版for循环
    player2.fly();
    player3.fly();
    player4.fly();
    }
}
