
// This class should be defined in a separate Java file 
public abstract class Animal {
    private int age;

    // must be implemented by children
    public abstract void go();
}


interface Walkable {
    // must be implemented by children 
    public void walk();
}




// Test class 
public class Test extends Animal implements Walkable {

    @Override
    public void go() {
        System.out.println("go go");
    } //from Animal abs class

    @Override
    public void walk() {
        System.out.println("Walk, walk");
    } // from Walkable inf


    // Test DRIVER
    public static void main(String[] args) {
        System.out.println("Hell Worlds");

        Animal a = new Animal() {
            @Override
            public void go() {
                System.out.println("Haha");
            }
        };

        Animal t = new Test(); // has the Animal's characteristics only 
        t.go();
	//t.walk(); //error

        Walkable w = new Test(); // has the Walkable's traits only 
        w.walk();
	//w.go(); // error
    }
}

