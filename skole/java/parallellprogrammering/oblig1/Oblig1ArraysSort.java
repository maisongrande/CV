import java.util.Random;
import java.util.Arrays;

public class Oblig1ArraysSort implements Timeable {
    protected static final int UPPER_LIMIT = 40;
    protected static final int LOWER_LIMIT = 0;
    protected boolean done;
    protected int[] array;


    public Oblig1ArraysSort(int[] a) {
        this.array = a;
    }

    @Override
    public void execute() {
        Arrays.sort(array);
        for (int i = 0; i < UPPER_LIMIT; i++) {
            ArrayFactory.swap(array, i, ((array.length-1)-i));
        }
    }

    @Override
    public String toString() {
        return "Oblig1ArraysSort hits: "+array.length+" ";
    }

    public int[] getResult() { return array; }

    public static void usage() {
        System.out.println("Please provide array size");
        System.exit(1);
    }

    public static void main(String ... args) {
        if (args.length != 1) { usage(); }
        else {
            int size = Integer.parseInt(args[0]);
            Random rand = new Random(1349);
            int[] a = new int[size];

            for (int i = 0; i < size; i++) {
                a[i] = rand.nextInt(size*10);
                if (size < 100) System.out.print(a[i] + " ");
            }

            Oblig1ArraysSort ob = new Oblig1ArraysSort(a);
            System.out.println(new StopWatch(ob));
            System.out.println();

            for (int i = 0; i < UPPER_LIMIT; i++) {
                System.out.print(a[i]+" ");
                if (i == 39) { System.out.print("\t\t"); }
            }
            System.out.println();
        }
    }
}
