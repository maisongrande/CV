import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.*;
/* Baserer seg på model2-koden utlever av Arne Maus v/UIO inf2440 */

public class Oblig3 {
    final static boolean DEBUG = false;
    final static int SEED = 1349;
    final String navn = "MultiRadix";
    final static int NUM_BIT = 6; //7;8;9;
    int[] globalB;
    int[] allMax;
    int[] randomArray;
    int[][] allCount;
    int[] sumCount;

    CyclicBarrier vent,ferdig,synk;
    int nThreads;
    int antKjerner;
    int numIter;
    int nLow,nStep,nHigh;
    int problemSize;
    String filnavn;
    int med;
    double [] seqTime;
    double [] parTime;


    /*****************GLOBALE VOLATILE FELLES VARIABLE************************/
    volatile boolean stop = false;
    /*************************************************************************/


    /** for også utskrift på fil(Skriving tile fil ikke implementert) */
    synchronized void println(String s) {
        System.out.println(s);
    }

    /** for også utskrift på fil */
    synchronized void print(String s) {
        System.out.print(s);
    }

    /** initieringen i main-tråden */
    void initier(String [] args) {
        nLow    = Integer.parseInt(args[0]);
        nStep   = Integer.parseInt(args[1]);
        nHigh   = Integer.parseInt(args[2]);
        numIter = Integer.parseInt(args[3]);
        //filnavn = args[4];

        antKjerner  = Runtime.getRuntime().availableProcessors();
        nThreads    = antKjerner;

        seqTime     = new double[numIter];
        parTime     = new double[numIter];
        allMax      = new int [nThreads];
        allCount    = new int[nThreads][];
        vent        = new CyclicBarrier(nThreads+1);
        ferdig      = new CyclicBarrier(nThreads+1);
        synk        = new CyclicBarrier(nThreads);

        //Start traader:
        for (int i = 0; i< nThreads; i++)
            new Thread(new RadixRunner(i)).start();
    }

    void utforTest (String [] args) {
        initier(args);
        println("Test av "+ navn+" med "+antKjerner+" kjerner, og "+nThreads
                +" traader, Median av: "+numIter+" iterasjoner");
        println("\nn\t   sekv.tid(ms)"
                +" para.tid(ms)    Speedup ");

        for (problemSize = nHigh;
                problemSize >= nLow;
                problemSize=problemSize/nStep)
        {
            for (med = 0; med < numIter; med++) {
                genererRandomArray();


                long t = System.nanoTime();
                parallelRadixSorter();
                parTime[med] =(System.nanoTime()-t)/1000000.0;

                if (DEBUG) {
                    System.out.println("Parallell:");
                    for (int i : randomArray) {
                        System.out.print(i+" ");
                    }
                    System.out.println("\n\n");
                }

                genererRandomArray();
                RadixSeq.randomArray = randomArray;
                t = System.nanoTime();
                RadixSeq.radixM(randomArray);
                seqTime[med] =(System.nanoTime()-t)/1000000.0;

                if (DEBUG) {
                    System.out.println("Sekvensiell");
                    for (int i : randomArray) {
                        System.out.print(i+" ");
                    }
                    System.out.println("\n\n");
                }
            }

            System.out.printf("%-10d %-12.3f %-15.3f %-13.4f\n",
                    problemSize,
                    median(seqTime,numIter), median(parTime,numIter),
                    median(seqTime,numIter)/median(parTime,numIter));
        }
        exit();
    }

    /** terminate parallel threads*/
    void exit() {
        stop = true;
        try { vent.await(); ferdig.await(); }
        catch (Exception e) { return; }
    }

    /** Wrapper funksjon for a starte traadene. */
    void parallelRadixSorter() {
        try {
            vent.await();
            ferdig.await();
        }
        catch (InterruptedException|BrokenBarrierException e)
        { return; }
    }


    /** Parallell Radix Runner */
    class RadixRunner implements Runnable {
        int id, segment, startAt, stopAt;
        int myMask, myShift;
        int[] myCount, bit;

        RadixRunner(int id) {
            this.id = id;
        }

        /** Generell Algoritme for aa dele opp et problem */
        void splitProblem(int problemSize) {
            segment = problemSize / nThreads;
            startAt = id * segment;
            stopAt = (id+1) * segment;
            if (id == nThreads-1)
                stopAt = problemSize;
        }


        /** B: freqCount - teller opp forekomster av siffer lokal count */

        void freqCount(int[] a) {
            splitProblem(problemSize);
            myCount = new int[myMask+1];
            for (int i = startAt; i < stopAt; i++) {
                myCount[(a[i]>>>myShift) & myMask]++;
            }
            allCount[id] = myCount;
        }

        /** B: Summerer lokal count over i global count */
        void sumFreqCount() {
            splitProblem(sumCount.length);
            for (int i = startAt; i < stopAt; i++) {
                for (int j = 0; j < nThreads; j++) {
                    sumCount[i] += allCount[j][i];
                }
            }
        }

        /** C: Lager indekstabell inn i b basert på sifferveriden i a */
        void addUpDigits(int[] a) {
            myCount = new int[myMask+1];
            splitProblem(problemSize);
            int index = 0;

            for (int i = startAt; i < stopAt; i++) {
                int number = ((a[i]>>>myShift) & myMask);

                for (int j = 0; j < number; j++) {
                    index += sumCount[j];
                }
                for (int k = 0; k < id; k++) {
                    index += allCount[k][number];
                }
                myCount[number] = index;
                index = 0;
            }
    }

        /** D: Flytter tallene over til array i sortert og riktig rekkefolge */
        void moveNumbers(int[] a, int[] b) {
            for (int i = startAt; i < stopAt; i++) {
                b[myCount[(a[i] >>> myShift) & myMask]++] = a[i];
            }
        }

        /** Run metode: */
        public void run() {
            while (!stop) {
                try { vent.await(); }
                catch (InterruptedException|BrokenBarrierException e)
                { return; }
                if (!stop) {

                    radixMParallel(randomArray);
                    if (id==0) { Tester.testSortering(randomArray); }

                }
                try { ferdig.await(); }
                catch (InterruptedException|BrokenBarrierException e)
                { return; }
            }
        }

        /** radixM - MultiRadix - Parallell */
        void radixMParallel(int[] a) {
            /***DEL A - Finn Maks Element ***/
            int n = a.length;
            splitProblem(problemSize);
            allMax[id] = Tools.findMax(a,startAt,stopAt);
            if (id == 0) { globalB = new int[n]; }

            try { synk.await(); }
            catch (InterruptedException|BrokenBarrierException e)
            { return; }

            int b[] = globalB;
            int sum = 0;
            int numBit = 2;

            int max = Tools.findMax(allMax, 0, nThreads);

            while (max >= (1L << numBit)) {
                numBit++;
            }


            int numDigits = Math.max(1, numBit/Oblig3.NUM_BIT);
            bit = new int[numDigits];
            int rest = numBit % numDigits;

            for (int i = 0; i < bit.length; i++) {
                bit[i] = numBit / numDigits;
                if ((rest--) > 0) {
                    bit[i]++;
                }
            }

            for (int i = 0; i < bit.length; i++) {
                radixSort(a, b, bit[i], sum);
                sum += bit[i];
                int[] t = a;
                a = b;
                b = t;
            }

            if (id==0) {
                System.arraycopy(a,0,randomArray,0,randomArray.length);
            }
        }

        void radixSort(int[] a, int[] b, int maskLen, int shift) {
            myMask = (1<<maskLen) - 1;
            myShift = shift;
            if (id == 0) { sumCount = new int[myMask+1]; }

            /***DEL B***/
            freqCount(a);
            try { synk.await(); }
            catch (InterruptedException|BrokenBarrierException e)
            { return; }
            sumFreqCount();
            try { synk.await(); }
            catch (InterruptedException|BrokenBarrierException e)
            { return; }

            /***DEL C***/
            addUpDigits(a);

            /***DEL D***/
            moveNumbers(a,b);
            try { synk.await(); }// Synk foer Retur
            catch (InterruptedException|BrokenBarrierException e)
            { return; }
        }
    }

    synchronized void printArray(int id, int[] a) {
        System.out.print("Id:("+id+") ");
        for (int i : a) {
            System.out.print(i+" ");
        }
        System.out.println();
    }


    /** sort double-array a to find median */
    double median(double a[], int right) {
        int i,k;
        double t;

        for (k = 1 ; k < right; k++) {
            t = a[k] ;
            i = k;
            while ((a[i-1] ) > t ) {
                a[i] = a[i-1];
                if (--i == 0) break;
            }
            a[i] = t;
        }
        return (a[a.length/2]);
    }

    /** Genrerer uniformt fordelt randomisert array */
    void genererRandomArray() {
        randomArray = new int[problemSize];
        Random r = new Random(SEED);
        for (int i = 0; i < problemSize; i++) {
            randomArray[i] = r.nextInt(Math.max(problemSize-1, 0));
        }
    }

    public static void main (String [] args) {
        if (args.length != 4)	{
            System.out.println("use: >java Modell2 <nLow> <nStep> <nHigh>"
                    +" <num iterations>");
        } else {
            new Oblig3().utforTest(args);
        }
    }
}

/** Arne Maus sin implementasjon av RadixM
 * Trukket ut av Oblig3 klassen for oversiktlighet */
class RadixSeq {
    public static int[] randomArray;

    /** radixM - MultiRadix */
    static int[] radixM(int a[]) {
        int numBit = 2;
        int sum = 0;
        int n = a.length;
        int[] bit;

        int max = Tools.findMax(a, 0, n);

        while (max >= (1L << numBit)) {
            numBit++;
        }

        int numDigits = Math.max(1, numBit/Oblig3.NUM_BIT);
        bit = new int[numDigits];
        int rest = numBit % numDigits;

        for (int i = 0; i < bit.length; i++) {
            bit[i] = numBit / numDigits;
            if ((rest--) > 0) {
                bit[i]++;
            }
        }
        int[] t, b = new int[n];

        for (int i = 0; i < bit.length; i++) {
            radixSort(a, b, bit[i], sum);
            sum += bit[i];
            t = a;
            a = b;
            b = t;
        }

        if ((bit.length & 1) != 0)
            System.arraycopy(a,0,b,0,a.length);
        Tester.testSortering(randomArray);
        return a;
    }

    /** radixSort */
    static void radixSort(int[] a, int[] b, int maskLen, int shift) {
        int mask = (1<<maskLen) - 1;
        int accumVal = 0;
        int n = a.length;
        int[] count = new int[mask+1];

        for (int i = 0; i < n; i++) {
            count[(a[i]>>>shift) & mask]++;
        }

        int j;
        for (int i = 0; i <= mask; i++) {
            j = count[i];
            count[i] = accumVal;
            accumVal += j;
        }

        for (int i = 0; i < n; i++) {
            b[count[(a[i]>>>shift) & mask]++] = a[i];
        }
    }

}

class Tools {

    /** Finn max element i array */
    static int findMax(int[] a, int start, int end) {
        int max = a[start];
        for (int i = start; i < end; i++)
            if (a[i] > max)
                max = a[i];
        return max;
    }

}

class Tester {
    static void testSortering(int[] a) {
        for (int i = 0; i < a.length-1; i++) {
            if (a[i] > a[i+1]) {
                System.out.println("FEIL: array["+i+"] > array["+(i+1)+"]");
                break;
            }
        }
    }



}
