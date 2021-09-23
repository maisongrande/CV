import java.util.Random;
import java.util.Arrays;
import java.util.concurrent.*;

/***/
public class Oblig1Parallel implements Timeable {
    protected static final int UPPER_LIMIT = 40;
    protected static final int LOWER_LIMIT = 0;
    protected CyclicBarrier all, children;
    protected int globalMaxIndx;
    protected volatile boolean done;
    protected int[] array;
    protected int[] localMax;
    protected int numThreads;

    public Oblig1Parallel(int[] a) {
        this.array = a;
        numThreads = Runtime.getRuntime().availableProcessors();
        localMax = new int[numThreads];
    }

    @Override
    public void execute() {
        done = false;

        all = new CyclicBarrier(1+numThreads);
        children = new CyclicBarrier(numThreads);
        int segment = (array.length-40)/numThreads;

        ArrayFactory.insertionSortRev(array, 0, UPPER_LIMIT);

        for (int id = 0; id < numThreads; id++) {
            int start = segment*id;
            if (id == numThreads-1) {
                segment = array.length-start-UPPER_LIMIT;
            }
            (new Thread(new Runner(id, UPPER_LIMIT+start, segment))).start();
        }

        while (!done) {
            try { all.await(); }
            catch (BrokenBarrierException | InterruptedException ex)
            { return; }

            if (array[UPPER_LIMIT-1] < array[globalMaxIndx]) {
                ArrayFactory.swap(array, UPPER_LIMIT-1, globalMaxIndx);
                //ArrayFactory.insertionSortRev(array, 0, UPPER_LIMIT);
                ArrayFactory.insert(array, array[UPPER_LIMIT-1], UPPER_LIMIT-1);
            } else {
                done = true;
            }

            try { all.await(); }
            catch (BrokenBarrierException | InterruptedException ex)
            { return; }
        }
    }

    protected class Runner implements Runnable {
        private int id;
        private int start;
        private int segment;

        public Runner(int id, int start, int segment) {
            this.id = id;
            this.start = start;
            this.segment = segment;
        }

        @Override
        /** run
         * Each thread tries to find a new bigger number than what is
         * currently stored at index 39 in array. When all threads
         * are done searching, thread 0 finds the biggest of them and
         * stores it in global max. Main is woken up and does tests
         * while threads are waiting on main.
         * */
        public void run() {

            while (!done) {
                localMax[id] =
                    ArrayFactory.arrayIndexMaxBlock(array,
                            start,
                            segment,
                            array[UPPER_LIMIT-1]);

                segment = (start+segment)-localMax[id];
                start = localMax[id];

                try { children.await(); }
                catch (InterruptedException | BrokenBarrierException ex)
                { return; }

                if (id == 0) {
                    int maxIndx = localMax[id];

                    for (int i = 0; i < localMax.length; i++) {
                        if (array[localMax[i]] > array[maxIndx])
                            maxIndx = localMax[i];
                    }

                    globalMaxIndx = maxIndx;
                }

                try {
                    all.await();
                    all.await();
                } catch (BrokenBarrierException | InterruptedException ex)
                { return; }
            }
        }
    }

    public int[] getResult() {
        return Arrays.copyOfRange(array, 0, UPPER_LIMIT);
    }


    @Override
    public String toString() {
        return "Oblig1Parallel hits: "+array.length+" ";
    }

    public boolean runTest() {
        int[] a = Arrays.copyOf(array, array.length);
        Arrays.sort(a);
        for (int i = 0; i < UPPER_LIMIT; i++) {
            if (array[i] != a[(array.length-1)-i]) {
                System.out.println(array[i]
                        +" != "
                        +a[(array.length-1)-i]);
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
                if (size < 100) System.out.print(a[i]+" ");
            }

            Oblig1Parallel ob = new Oblig1Parallel(a);
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
