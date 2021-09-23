import java.util.*;
import java.util.Arrays.*;
import java.io.*;
import java.util.concurrent.*;

public class EratosthenesSilParallelFactorize {
    ArrayList<Long> totalFactors;
    volatile int thread_prime;
    volatile boolean done = false;
    volatile boolean returnFromFact = false;
    volatile long toFactorize;

    volatile long product;

    CyclicBarrier alle, traader;
    int nThreads;

    final static byte ONES = (byte)0xFF;
    final static byte ZEROS = (byte)0x00;
    final int[] bitMask={1,2,4,8,16,32,64,128};
    final int[] bitMask2={255-1,255-2,255-4,255-8,255-16,255-32,255-64,255-128};
    double genTime;
    byte [] bitArr;
    int primeCnt;
    int maxNum;
    int maxOdd;


    EratosthenesSilParallelFactorize(int maxNum) {
        this.nThreads = Runtime.getRuntime().availableProcessors();
        this.maxNum = maxNum;
        this.maxOdd = (int)Math.ceil(Math.sqrt(maxNum));
        this.bitArr = new byte [(maxNum/16)+1];
        setAllPrime();
        long tic = System.nanoTime();
        generatePrimesByEratosthenes();
        //printAllPrimes();
        long toc = System.nanoTime();
        this.genTime = (toc-tic)/1000000.0;
        System.out.println(this);
        System.out.println("Parallell sieve tok "+genTime+" ms,");
        factorizeNumber();
    } // end konstruktor ErathostenesSil



    /** initialiserer alle bit i bitArray = 1
     *  men sett 0-bit til 0 da 3 er foerste kjente primtall.
     * */
    void setAllPrime() {
        for (int i = 0; i < bitArr.length; i++) {
            bitArr[i] = ONES;
        }
        bitArr[0] &= bitMask2[0];
    }



    /** returnerer true hvis tallet i er satt til 1 i bitArrayet. */
    boolean isPrime (int i) {
        if (i == 2) { return true; }
        if (i < 2 || i % 2 == 0) { return false; }
        if ((bitArr[i>>>4] & 1<<((i & 15)>>1)) == 0) { return false; }
        return true;
    }


    @Override
    public String toString() {
        primeCnt = countPrimesUpTo(maxNum);
        String retStr = "Genererte alle primtall <= "+maxNum+" paa: "
            + genTime+" milliSek \nmed Eratosthenes sil ("
            + ((double)genTime/primeCnt)+" milliSek/primTall.)\n"
            + "Antall primatall < "+maxNum+" er: "+primeCnt+" dvs "
            + (double)((primeCnt*100.0)/maxNum)+"%,\n\n";
        return retStr;
    }


    /** returnerer antall bit satt til 1 i bitArr aka antall primtall
     * opp til gitt nummer. */
    int countPrimesUpTo(int num) {
        int cnt = 0;
        for ( int i = 2; i < maxNum; i++)
            if (isPrime(i)) cnt++;
        return cnt;
    }


    /** printer bit-representasjon av bitArr. */
    public void printBytes() {
        for (int i = 8*bitArr.length-1; i >= 0; i--)
            System.out.printf("%s", (bitArr[i>>>3] & (bitMask[i&7]))>0?"1":"0");
        System.out.println();
    }


    void factorizeNumber() {
        totalFactors = new ArrayList<Long>();
        long myNum = (long)maxNum * maxNum;
        int n = 100;

        alle = new CyclicBarrier(nThreads+1);
        traader = new CyclicBarrier(nThreads);
        done = false;

        //int test = (int)Math.sqrt(maxNum)>>4;


        int segment = (bitArr.length
                /// (int)(Math.log(bitArr.length)/Math.log(2))
                / nThreads);

        //int segment = test/nThreads;
        for (int tid = 0; tid < nThreads; tid++) {
            int start = tid * segment;
            if (tid == nThreads-1)
                segment =bitArr.length - start;//((int)Math.sqrt(maxNum)>>4) - start;
            (new Thread(new FactorRunner(tid, start, segment))).start();
        }

        double time = 0;

        for (int i = 0; i < 100; i++) {
            long tic = System.nanoTime();
            long toBeFactorized = (myNum-n)+(long)i;
            System.out.print(toBeFactorized +" = ");

            toFactorize = toBeFactorized;

            try { alle.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }
            /*
             * Traadene finner faktorer her..
             */
            try { alle.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }

            if (totalFactors.isEmpty())
                totalFactors.add(toFactorize);

            Iterator<Long> resIter = totalFactors.iterator();
            while (resIter.hasNext()) {
                long res = resIter.next();
                System.out.print(res);
                if (resIter.hasNext()) {
                    System.out.print("*");
                }
            }
            System.out.println();
            totalFactors.clear();

            long toc = System.nanoTime();
            time += (toc-tic)/1000000.0;
        }

        done = true;
        try { alle.await(); }
        catch (InterruptedException | BrokenBarrierException ex) { return; }

        System.out.println("\n\n"+n
                +" faktoriseringer med utskrift beregnet paa: "
                +time+"ms. Dvs: "+(double)time/n+"ms. per faktorisering.");
    }

    synchronized void addFactorsToTotal(ArrayList<Long> res) {
        totalFactors.addAll(res);
    }


    private class FactorRunner implements Runnable {
        int id;
        int start;
        int segment;
        ArrayList<Long> myRes;

        FactorRunner(int id, int start, int segment) {
            this.id = id;
            this.start = start;
            this.segment = segment;
            System.out.println(id+" START: "+start+" SEGMENT: "+segment);
        }

        public void run() {

            while (!done) {
                try {

                    alle.await(); // Vent pa nytt tall fra main.

                    if (!done) { // factorer dette tallet.
                        myRes = factorize(toFactorize);
                        addFactorsToTotal(myRes);
                        myRes.clear();
                    }
                    alle.await(); // main venter her.

                }catch(InterruptedException|BrokenBarrierException ex) {
                    return;
                }
            }
        }

        /** faktoriserer gitt tall til sine primtallbestandigheter */
        ArrayList<Long> factorize (long num) {
            ArrayList<Long> fakt = new ArrayList<Long>();

            boolean foundOne = false;
            int startIdx = start*16 + 1;
            long endIdx = (id+1)*segment*16+1;
            long uLim = endIdx;
            long original = num;

            if (id == nThreads-1) {
                uLim = maxNum;
                endIdx = num;
            }

            if (id == 0) {
                while (num%2==0) {
                    fakt.add((long)2);
                    num /= 2;
                    uLim = (long)Math.sqrt(num);
                    foundOne = true;
                }
           }

            //System.out.println(id+" Start: "+startIdx+" up to: "+uLim);

            for (int oddNum = startIdx; oddNum < uLim ; oddNum += 2) {
                if (isPrime(oddNum)) {
                    if (num%oddNum == 0) {
                        fakt.add((long)oddNum);
                        num /= oddNum;
                        oddNum -= 2;
                        foundOne = true;
                        uLim = (long)Math.sqrt(num);
                    }
                }
            }

            if (original == num) {
                return fakt;
            }

           if (num > 1 && num < endIdx && num >= startIdx) {
               fakt.add(num);
           } else if (num > maxNum && foundOne) {
               for (int i=3; i < Math.sqrt(num); i+=2) {
                   if (isPrime(i)) {
                        if (num % i == 0) {
                            return fakt;
                        }
                   }
               }
               fakt.add(num);
           }

           return fakt;
        } // end factorize
    }


    /** faktoriserer gitt tall til sine primtallbestandigheter */
    ArrayList<Long> factorize (long num) {
        ArrayList <Long> fakt = new ArrayList <Long>();

        while (num%2==0) {
            fakt.add((long)2);
            num /= 2;
        }
        long oddNum = 1;
        long uLim = (long)Math.sqrt(num);

        for (int i = 0; oddNum <= uLim ; i++, oddNum += 2) {
            if ((bitArr[i>>3] & (bitMask[i&7]))>0) {
                if (num%oddNum == 0) {
                    fakt.add(oddNum);
                    num /= oddNum;
                    i = 0;
                    oddNum = 1;
                }
                uLim = (long)Math.sqrt(num);
            }
        }
        if (num > 1)
            fakt.add(num);
        return fakt;
    } // end factorize


    /** returns next prime number after number 'i' */
    int nextPrime(int num) {
        if (num < 2) { return 2; }
        int p = (num%2)==0?num+1:num+2;
        for (; !isPrime(p); p+=2);
        return p;
    } // end nextPrime



    /** printer ut alle primtall element i {2,3,4,....} */
    void printAllPrimes(){
        for ( int i = 2; i < maxNum; i++)
            if (isPrime(i)) { System.out.print(" "+i); }
        System.out.println();
    }



    void generatePrimesByEratosthenes() {
        Thread[] threads = new Thread[nThreads];

        for (int tid = 0; tid < nThreads; tid++) {
            byte[] cpy = new byte[(maxOdd)/16+1];
            for (int i = 0; i < cpy.length; i++) {
                cpy[i] = bitArr[i];
            }

            (threads[tid]=
             new Thread(new SieveRunner(tid, cpy, maxOdd))).start();
        }

        for (Thread t : threads)
            try { t.join(); }
        catch (InterruptedException ex)
        { return; }
    }

    /** SieveRunner
     *  Klassen tar seg av den parallelle utkryssningen av bitArr
     *  Deler opp bitArr i rettferdige segmenter, lagger et hjelpe-prim-array
     *  for saa aa mappe/krysse av for dette over i traadenes deler av bitArr.
     * */
    private class SieveRunner implements Runnable {
        int id;
        int segment;
        byte[] myPrimes;
        int genUpTo;

        public SieveRunner(int id, byte[] copyOfMain, int genUpTo) {
            this.id = id;
            this.genUpTo = genUpTo;
            this.segment = maxNum / nThreads;
            if (id == nThreads-1)
                segment = (maxNum - id*segment);
            myPrimes = copyOfMain;
            generatePrimes(1, genUpTo);
        }

        public void run() {
            for (int i = 0, oddNum = 1;
                    i < 8*myPrimes.length && oddNum < genUpTo*genUpTo;
                    i++, oddNum+=2)
            {

                if ((myPrimes[i>>3] & (bitMask[i&7])) > 0) {
                    int stop = stopAt(oddNum);
                    int start = startAt(oddNum);
                    if (start < 0) { start = 0; }
                    if (stop < 0) { stop = 0; }

                    //         if (id == nThreads-1 && stop==1)
                    //           System.out.println("Start: "+start+" stop: "+stop);
                    for (int j = start ; j < stop; j++) {
                        //if (id == nThreads-1)
                        //System.out.println("Cross out: "+myNextPrime(oddNum,j));
                        crossOut(bitArr, myNextPrime(oddNum, j));
                    }
                }
            }
        }

        private int startAt(int prime) {
            return (((id * segment) - prime * prime) / (2*prime)) + 1;
        }

        private int stopAt(int prime) {
            return ((((id+1) * segment) - prime * prime) / (2*prime)) + 1;
        }

        private int myNextPrime(int base, int n) {
            return (base * base) + 2 * base * n;
        }

        void generatePrimes(int nextOdd, int maxOddNum) {
            for (int i = 0; i < 8*myPrimes.length && nextOdd <= maxOddNum;
                    i++, nextOdd+=2)
            {
                if ((myPrimes[i>>3] & (bitMask[i&7])) > 0) {
                    for (int j = nextOdd*nextOdd; j < maxOddNum; j+=2*nextOdd) {
                        crossOut(myPrimes, j);
                    }
                }
            }
        }

        /** printer bit-representasjon av bitArr. */
        public void printBytes() {
            for (int i = 8*myPrimes.length-1; i >= 0; i--)
                System.out.printf("%s", (myPrimes[i>>>3] & (bitMask[i&7]))>0?"1":"0");
            System.out.println();
        }

        /** set as not prime- cross out (set to 0)  bit represening 'int i' */
        void crossOut(byte[] arr, int i) {
            try {
                arr[i>>4] &= bitMask2[((i & 15)>>1)];
            } catch (IndexOutOfBoundsException e) {
                // System.out.println(id+" OutOfBounds for nummer : "+i
                //       +" -- Array Size: "+bitArr.length
                //     +" -- Proevde aa krysse i index "+(i>>4));
                return;
            }
        }
    }

    public static void main(String...args) {

        int sqrtNumber = Integer.parseInt(args[0]);
        EratosthenesSilParallelFactorize esp =
            new EratosthenesSilParallelFactorize(sqrtNumber);
        long tic = System.nanoTime();
        //esp.printAllPrimes();
        System.out.println("\ntime: "+
                (double)(System.nanoTime() - tic)/1000000.0);
        System.out.println();
        /*
           long myNum = (long)sqrtNumber * sqrtNumber;
           int n = 100;
           tic = System.nanoTime();
           for (int i = 0; i < 100; i++) {
           System.out.print(((myNum-n)+(long)i)+" = ");
           Iterator<Long> resIter = esp.factorize((myNum-n)+(long)i).iterator();
           while (resIter.hasNext()) {
           long res = resIter.next();
           System.out.print(res);
           if (resIter.hasNext()) {
           System.out.print("*");
           }
           }
           System.out.println();
           }
           long toc = System.nanoTime();
           double time = (toc-tic)/1000000.0;
           System.out.println("\n\n"+n+" faktoriseringer med utskrift beregnet paa: "
           +time+"ms. Dvs: "+(double)time/n+"ms. per faktorisering.");
           *//*
                tic = System.nanoTime();
                esp.factorizeNumber();
                System.out.println("\ntime: "+
                (double)(System.nanoTime() - tic)/1000000.0);
                System.out.println();
                */
    }
}

