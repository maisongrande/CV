import java.util.Arrays;

public class Oblig1 implements Timeable {
    protected int[] a;
    protected double[] paraTime;
    protected double[] seqTime;
    protected double[] asTime;
    protected Oblig1Parallel obpara;
    protected Oblig1Seq  obseq;
    protected Oblig1ArraysSort  obas;
    protected int nIter;

    Oblig1(int[] array, int nIter) {
        this.a = array;
        this.nIter = nIter;
        paraTime = new double[nIter];
        seqTime = new double[nIter];
        asTime = new double[nIter];
    }

    @Override
    public void execute() {
        StopWatch sp;
        for (int i = 0; i < nIter; i++) {
            // Run sequential algorithm:
            obseq = new Oblig1Seq(Arrays.copyOf(a, a.length));
            sp = new StopWatch(obseq);
            System.out.println(sp);
            seqTime[i] = sp.getTime();

            // Run parallel algorithm:
            obpara = new Oblig1Parallel(Arrays.copyOf(a, a.length));
            sp = new StopWatch(obpara);
            System.out.println(sp);
            paraTime[i] = sp.getTime();

            // Run ArraysSort algorithm:
            obas = new Oblig1ArraysSort(Arrays.copyOf(a, a.length));
            sp = new StopWatch(obas);
            System.out.println(sp);
            asTime[i] = sp.getTime();

            System.out.println();
        }
    }

    @Override
    public String toString() {
        if (nIter > 1) {
            ArrayFactory.insertionSort(paraTime,0,nIter-1);
            ArrayFactory.insertionSort(seqTime,0,nIter-1);
            ArrayFactory.insertionSort(asTime,0,nIter-1);
        }

        int idx = nIter%2!=0&&nIter>1?(nIter/2+1):nIter/2;
        double seqMedian = seqTime[idx];
        double paraMedian = paraTime[idx];
        double asMedian = asTime[idx];
        double seqParaSpeedUp = seqMedian/paraMedian;
        double asParaSpeedUp = asMedian/paraMedian;

        String retstr = "ArraysSort median time: " + asMedian + " ms\n"
            +"Sequential median time: " + seqMedian + " ms\n"
            +"Parallel median time:   " + paraMedian + " ms\n"
            +"Speedup parallel/sequential: " + seqParaSpeedUp + "\n"
            +"Speedup parallel/arraysSort: " + asParaSpeedUp + "\n"
            +"Total time: ";
        return retstr;
    }

    public void testResult() {
        int[] a = obseq.getResult();
        int[] b = obpara.getResult();
        int[] c = obas.getResult();

        for (int i = 0; i < 40; i++) {
            if (a[i] != b[i] || a[i] != c[i]) {
                System.out.println("Comparing the 40 first "
                        +"elements in both arrays "
                        +"FAILED.\n");
                System.out.println(a[i]+" !=  "+b[i]+" || "+a[i]+" != "+c[i]);
                return;
            }
        }
    }

    public static void main(String...args) {
        if (args.length != 2) {
            System.out.println("usage: java Oblig1 <arraysize> <numTest>");
        } else {
            int size = Integer.parseInt(args[0]);
            int nIter = Integer.parseInt(args[1]);
            int seed = 42;
            int[] a = ArrayFactory.randomIntArray(size, size, seed);
            Oblig1 ob = new Oblig1(a, nIter);
            System.out.println(new StopWatch(ob));
            ob.testResult();
        }
    }
}
