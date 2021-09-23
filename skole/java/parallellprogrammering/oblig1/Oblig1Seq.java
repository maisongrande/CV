import java.util.Random;
import java.util.Arrays;

public class Oblig1Seq implements Timeable {
    protected static final int UPPER_LIMIT = 40;
    protected static final int LOWER_LIMIT = 0;
    protected boolean done;
    protected int[] array;

    public Oblig1Seq(int[] a) {
        this.array = a;
    }

    @Override
    public void execute() {
        done = false;
        ArrayFactory.insertionSortRev(array, 0, UPPER_LIMIT);

        while (!done) {
            int maxIndx = ArrayFactory.arrayIndexMaxBlock(array,
                    UPPER_LIMIT,
                    array.length-UPPER_LIMIT,
                    array[UPPER_LIMIT-1]);

            if (array[UPPER_LIMIT-1] <= array[maxIndx]) {
                ArrayFactory.swap(array,UPPER_LIMIT-1,maxIndx);
                ArrayFactory.insert(array, array[UPPER_LIMIT-1], UPPER_LIMIT-1);
            } else {
                done = true;
            }
        }
    }

    public int[] getResult() {
        return Arrays.copyOfRange(array,0,UPPER_LIMIT);
    }

    @Override
    public String toString() {
        return "Oblig1seq hits: "+array.length+" ";
    }

    public boolean runTest() {
        int[] a = Arrays.copyOf(array, array.length);
        Arrays.sort(a);
        for (int i = 0; i < UPPER_LIMIT; i++) {
            if (array[i] != a[(array.length-1)-i]) {
                System.out.println(array[i] + " != " + a[(array.length-1)-i]);
                return false;
            }
        }
        return true;
    }

    public static void usage() {
        System.out.println("Please provide array size");
        System.exit(1);
    }

    public static void main(String...args) {
        if (args.length != 1) { usage(); }
        else {
            int size = Integer.parseInt(args[0]);
            Random rand = new Random(1349);
            int[] a = new int[size];

            for (int i = 0; i < size; i++) {
                a[i] = rand.nextInt(size*10);
                if (size < 100) { System.out.print(a[i]+" "); }
            }

            Oblig1Seq ob = new Oblig1Seq(a);
            System.out.println(new StopWatch(ob));
            System.out.println();

            for (int i = 0; i < UPPER_LIMIT; i++) {
                System.out.print(a[i]+" ");
                if (i == 39) { System.out.print("\t\t"); }
            }
            if (!ob.runTest()) {
                System.out.println("Arrays.sort()-test failed");
            }
            System.out.println();
        }
    }
}
