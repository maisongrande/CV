import java.util.*;
import java.io.*;
import java.util.Arrays.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

class EratosthenesSil {
    final static byte ONES = (byte)0xFF;
    final int[] bitMask={1,2,4,8,16,32,64,128};
    final int[] bitMask2={255-1,255-2,255-4,255-8,255-16,255-32,255-64,255-128};
    byte [] bitArr;
    double genTime;
    int primeCnt;
    int maxNum;
    int maxOdd;


    /** Initierer objektet og setter alle bit til 1 i bitArr*/
    EratosthenesSil (int maxNum) {
        this.maxNum = maxNum;
        this.maxOdd = (int)Math.ceil(Math.sqrt(maxNum));
        this.bitArr = new byte [(maxNum/16)+1];
        setAllPrime();
    }


    /** initialiserer alle bit i bitArray = 1 */
    void setAllPrime() {
        for (int i = 0; i < bitArr.length; i++) {
            bitArr[i] = ONES;
        }
    }


    /** set as not prime- cross out (set to 0)  bit represening 'int i' */
    void crossOut(int i) {
        bitArr[i>>4] &= bitMask2[((i & 15)>>1)];
    }


    /** returnerer true hvis tallet i er satt til 1 i bitArrayet. */
    boolean isPrime (int i) {
        if (i == 2) { return true; }
        if (i < 2 || i % 2 == 0) { return false; }
        if ((bitArr[i>>4] & 1<<((i & 15)>>1)) == 0) { return false; }
        return true;
    }


    @Override
    public String toString() {
        primeCnt = countPrimesUpTo(maxNum);
        String retStr = "Genererte alle primtall <= "+maxNum+" paa: "
            + genTime+" milliSek \nmed Eratosthenes sil ("
            + ((double)genTime/primeCnt)+" milliSek/primTall.)\n"
            + "Antall primtall < "+maxNum+" er: "+primeCnt+" dvs "
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


    /** faktoriserer gitt tall til sine primtallbestandigheter */
    ArrayList<Long> factorize (long num) {
        ArrayList <Long> fakt = new ArrayList <Long>();
        long oddNum = 3;
        long uLim = (long)Math.sqrt(num);

        while (num%2==0) {
            fakt.add((long)2);
            num /= 2;
            uLim = (long)Math.sqrt(num);
        }

        for (int i = 1; oddNum <= uLim ; i++, oddNum += 2) {
            if ((bitArr[i>>3] & (bitMask[i&7]))>0) {
                if (num%oddNum == 0) {
                    fakt.add(oddNum);
                    num /= oddNum;
                    i -= 1;
                    oddNum -= 2;
                    uLim = (long)Math.sqrt(num);
                }
            }
        }
        if (num > 1)
            fakt.add(num);
        return fakt;
    }


    /** Skriver ut faktorene til (num - n) opp til num */
    double printNLastFactorsOf(int n, int num) {
        long myNum = (long)num * num;
        long tic = System.nanoTime();

        for (int i = 0; i < 100; i++) {
            System.out.print(((myNum-n)+(long)i)+" = ");
            Iterator<Long> resIter = factorize((myNum-n)+(long)i).iterator();
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
        System.out.println("\n\n"+n
                +" faktoriseringer med utskrift beregnet paa: "
                +time+"ms. Dvs: "+(double)time/n+"ms. per faktorisering.");
        return time;
    }


    /** returns next prime number after number i */
    int nextPrime(int num) {
        if (num < 2) { return 2; }
        int p = (num%2)==0?num+1:num+2;
        for (; !isPrime(p); p+=2);
        return p;
    }


    /** printer ut alle primtall element i {2,3,4,....} */
    void printAllPrimes(){
        for ( int i = 2; i < maxNum; i++)
            if (isPrime(i)) System.out.print(" "+i);
        System.out.println();
    }


    /** krysser av alle  oddetall i bitArr[] som ikke er primtall */
    void generatePrimesByEratosthenes() {
        long tic = System.nanoTime();
        int oddNum = 1;
        crossOut(oddNum);      // 1 er ikke et primtall
        for (int i = 0; i < 8*bitArr.length && oddNum <= maxOdd; i++, oddNum+=2) {
            if ((bitArr[i>>3] & (bitMask[i&7])) > 0) {
                for (int j = oddNum*oddNum; j < maxNum; j+=2*oddNum) {
                    crossOut(j);
                }
            }
        }
        genTime = (System.nanoTime() - tic) / 1000000.0;
    }


    /** returnerer tiden det tok aa generere primtallene */
    public double sieveTime() {
        return genTime;
    }
}

class EratosthenesSilParallel {
    final static byte ONES = (byte)0xFF;
    final int[] bitMask={1,2,4,8,16,32,64,128};
    final int[] bitMask2={255-1,255-2,255-4,255-8,255-16,255-32,255-64,255-128};
    volatile boolean done = false;
    HashMap<Integer, ArrayList<Long>> myFactors;
    volatile long toFactorize;
    ArrayList<Long> totalFactors;
    CyclicBarrier alle, traader;
    int nThreads;
    double genTime;
    byte [] bitArr;
    int primeCnt;
    int maxNum;
    int maxOdd;


    /** Initierer objektet og setter alle bit til 1 i bitArr*/
    EratosthenesSilParallel(int maxNum) {
        this.nThreads = Runtime.getRuntime().availableProcessors();
        this.maxNum = maxNum;
        this.maxOdd = (int)Math.ceil(Math.sqrt(maxNum));
        this.bitArr = new byte [(maxNum/16)+1];
        setAllPrime();
    }


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
            + genTime+" milliSek \nmed Eratosthenes sil Parallell ("
            + ((double)genTime/primeCnt)+" milliSek/primTall.)\n"
            + "Antall primtall < "+maxNum+" er: "+primeCnt+" dvs "
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


    /** Faktoriserer de 100 siste tallene i kvadratet av maxNum */
    double factorizeNumber(int n) {
        long tic = System.nanoTime();
        totalFactors = new ArrayList<Long>();
        long myNum = (long)maxNum * maxNum;

        alle = new CyclicBarrier(nThreads+1);
        traader = new CyclicBarrier(nThreads);
        myFactors = new HashMap<Integer, ArrayList<Long>>(nThreads);

        done = false;

        for (int tid = 0; tid < nThreads; tid++)
            (new Thread(new FactorRunner(tid))).start();

        double time = (double)(System.nanoTime()-tic) / 1000000.0;

        try {
            for (int i = 0; i < 100; i++) {
                tic = System.nanoTime();
                long toBeFactorized = (myNum-n)+(long)i;
                System.out.print(toBeFactorized +" = ");
                toFactorize = divideByTwo(toBeFactorized);
                alle.await();
                /*
                 * Traadene finner faktorer her..
                 */
                alle.await();
                printFactors(totalFactors.iterator());
                totalFactors.clear();
                long toc = System.nanoTime();
                time += (toc-tic)/1000000.0;
            }
            alle.await();
            done = true;
            alle.await();
            System.out.println("\n\n"+n
                    +" faktoriseringer med utskrift beregnet paa: "
                    +time+"ms. Dvs: "+(double)time/n+"ms. per faktorisering.");
        } catch (InterruptedException | BrokenBarrierException ex)
        { return 0; }
        return time;
    }


    /**Saa lenge tallet er delelig paa 2...*/
    public long divideByTwo(long num) {
        while (num % 2 == 0) {
            num /= 2;
            totalFactors.add(2l);
        }
        return num;
    }


    /** Printer faktorene mottatt. */
    public void printFactors(Iterator<Long> resIter) {
        while (resIter.hasNext()) {
            long res = resIter.next();
            System.out.print(res);
            if (resIter.hasNext()) {
                System.out.print("*");
            }
        }
        System.out.println();
    }


    /** FactorRunner tar for seg faktoriseringen av tallene */
    private class FactorRunner implements Runnable {
        int id;
        int start;
        int end;
        long thisNumber;

        FactorRunner(int id) {
            this.id = id;
        }


        @Override
        public void run() {
            try {
                while (!done) {
                    myFactors.put(id, null);
                    alle.await(); // Vent pa nytt tall fra main.
                    assignSegment();
                    factorize(thisNumber);
                    traader.await();
                    if (id == 0) {
                        finalizeFactorisation();
                    }
                    alle.await(); // main venter her.
                }
            } catch(InterruptedException | BrokenBarrierException ex)
            { return; }
        }


        /** faktoriserer gitt tall til sine primtallbestandigheter */
        void factorize (long num) {
            ArrayList<Long> factors = new ArrayList<Long>();

            for (int oddNum = start; oddNum < end ; oddNum += 2) {
                if (isPrime(oddNum) && num%oddNum == 0) {
                        factors.add((long)oddNum);
                        num /= oddNum;
                        end = (int)Math.sqrt(num);
                        oddNum -= 2;
                }
            }
            myFactors.put(id, factors);
        }


        /** Segmenter deles ut og faktor-fetch utfoeres. */
        private void assignSegment() {
            thisNumber = toFactorize;
            int primesUpTo =  (int)Math.sqrt(thisNumber);

            start = primesUpTo - (primesUpTo >> id);
            start = id==0 ? 3 : start;
            start += start % 2 == 0 ? 1 : 0;

            end = primesUpTo - (primesUpTo >> (id+1));
            if (id == nThreads-1)
                end = primesUpTo;
        }


        /** Syntetiserer resultatet fra samtlige traader inn totalFactors  */
        void finalizeFactorisation() {
            for (int tid = 0; tid < nThreads; tid++) {
                ArrayList<Long> tmp = myFactors.get(tid);
                if (tmp == null)
                    continue;
                for (long l : tmp) {
                    thisNumber /= l;
                    totalFactors.add(l);
                }
            }
            if (thisNumber!=1) {
                totalFactors.add(thisNumber);
            }
        }
    }


    /** returns next prime number after number 'i' */
    int nextPrime(int num) {
        if (num < 2) { return 2; }
        int p = (num%2)==0?num+1:num+2;
        for (; !isPrime(p); p+=2);
        return p;
    }


    /** printer ut alle primtall element i {2,3,4,....} */
    void printAllPrimes(){
        for ( int i = 2; i < maxNum; i++)
            if (isPrime(i)) { System.out.print(" "+i); }
        System.out.println();
    }


    /** Genererer primtall parallellt opp til maxNum */
    public void generatePrimesByEratosthenes() {
        long tic = System.nanoTime();

        Thread[] threads = new Thread[nThreads];

        for (int tid = 0; tid < nThreads; tid++) {
            (threads[tid]= new Thread(new SieveRunner(tid, maxOdd))).start();
        }

        for (Thread t : threads)
            try { t.join(); }
        catch (InterruptedException ex)
        { return; }
        genTime = (System.nanoTime()-tic)/1000000.0;
    }


    /** SieveRunner
     *  Klassen tar seg av den parallelle utkryssningen av bitArr
     * */
    private class SieveRunner implements Runnable {
        int id;
        int segment;
        int genUpTo;

        public SieveRunner(int id, int genUpTo) {
            this.id = id;
            this.genUpTo = genUpTo;
            this.segment = maxNum / nThreads;
        }

        /** Genererer primtall fra starten av traadens segment til slutten av
         * segmentet. Dvs at alle traader begynner paa tallet 3 og hopper inn i
         * sitt segment basert pa kalkulert start- og stoppindeks. Foelger
         * dermed Eratosthenes sil*/
        @Override
        public void run() {

            for (int oddNum = 3; oddNum < genUpTo; oddNum+=2) {
                if (isPrime(oddNum)) {
                    generatePrimesFrom(oddNum, startAt(oddNum), stopAt(oddNum));
                }
            }
        }

        /** Baserer seg paa en observasjon om at det finnes en 1 til 1
         * korrespondanse mellom mellom et vilkaarlig tall og dens foerste
         * faktor innenfor et vilkaarlig segment. */
        private int startAt(int prime) {
            int retval = (int)Math.floor((double)((((id * segment)
                                - (prime * prime)) / (2.0*prime)))) + 1;
            return retval < 0 ? 0 : retval;
        }


        /** Kalkulerer verdien denne traaden skal stoppe ved basert pa hvilken
         * faktor som skal strykes ut og stoerrelsen paa traadens tildelte
         * segment. */
        private int stopAt(int prime) {
            int retval = (int)Math.floor((double)(((((id+1) * segment)
                                - (prime * prime)) / (2.0*prime)))) + 1;
            return retval < 0 ? 0 : retval;
        }


        /** Hvilket tall er det neste denne traaden skal krysse ut */
        private int myNextPrime(int base, int n) {
            return (base * base) + 2 * base * n;
        }

        /** Hver traad krysser ut samtlige faktorer av prime innenfor sitt
         * segment.*/
        void generatePrimesFrom(int prime, int start, int stop) {
            for (int j = start ; j < stop; j++) {
                crossOut(bitArr, myNextPrime(prime, j));
            }
        }

        /** set as not prime- cross out (set to 0)  bit represening 'int i' */
        void crossOut(byte[] arr, int i) {
            arr[i>>4] &= bitMask2[((i & 15)>>1)];
        }
    }

    /** returnerer tiden det tok aa generere primtallene*/
    public double sieveTime() {
        return genTime;
    }
}

class Oblig2 {
    boolean WRITE_RESULTS = false;
    int nIter;
    int nLow,nStep,nHigh;
    int maxNum;
    String fname;
    private EratosthenesSil es;
    private EratosthenesSilParallel esp;
    double[] seqTimeSieve, seqTimeFactor;
    double[] parTimeSieve, parTimeFactor;


    Oblig2(int primesFrom, int nStep, int primesDownTo, int nIter, String filename) {
        this.nHigh = primesFrom;
        this.nLow = primesDownTo;
        this.nStep = nStep;
        this.nIter = nIter;
        if (filename!=null) {
            this.fname = filename;
            WRITE_RESULTS = true;
        }
        execute();
    }


    void execute() {
        int num = nHigh;
        do {
            parTimeFactor = new double[nIter];
            seqTimeFactor = new double[nIter];
            parTimeSieve = new double[nIter];
            seqTimeSieve = new double[nIter];

            for (int i = 0; i < nIter; i++) {
                runSequential(i,num);
                runParallelImpl(i, num);
            }
            if (WRITE_RESULTS && nIter > 2) {
                write_results(num);
            }
            num/=nStep;
        } while (nLow <= num);

    }

    void runParallelImpl(int i, int num) {
        System.out.println("Initierer Parallell Sieve...");
        (esp = new EratosthenesSilParallel(num))
            .generatePrimesByEratosthenes();
        parTimeSieve[i] = esp.sieveTime();
        System.out.println(esp);
        parTimeFactor[i] = esp.factorizeNumber(100);
    }

    void runSequential(int i, int num) {
        System.out.println("Initierer Sekvensiell Sieve...");
        (es = new EratosthenesSil(num)).generatePrimesByEratosthenes();
        seqTimeSieve[i] = es.sieveTime();
        System.out.println(es);
        seqTimeFactor[i] = es.printNLastFactorsOf(100, num);
    }

    void write_results(int num) {
        PrintWriter pw=null;
        Arrays.sort(parTimeFactor);
        Arrays.sort(parTimeSieve);
        Arrays.sort(seqTimeFactor);
        Arrays.sort(seqTimeSieve);
        try {
            double seqMedianFact = seqTimeFactor[(seqTimeFactor.length/2)+1];
            double parMedianFact = parTimeFactor[(parTimeFactor.length/2)+1];
            double seqMedianSieve = seqTimeSieve[(seqTimeSieve.length/2)+1];
            double parMedianSieve = parTimeSieve[(parTimeSieve.length/2)+1];
            FileWriter fw = new FileWriter(new File(fname), true);
            pw = new PrintWriter(fw);
            pw.printf("Siev: %20d\tSekv: %-12.4f\tPara: %-12.4f\tSpeed-up: %.4f\n",
                    (long)num*num,
                    seqMedianSieve, parMedianSieve,
                    seqMedianSieve/parMedianSieve);
            pw.printf("Fakt: %20d\tSekv: %-12.4f\tPara: %-12.4f\tSpeed-up: %.4f\n",
                    (long)num*num,
                    seqMedianFact, parMedianFact,
                    seqMedianFact/parMedianFact);
        } catch (IOException ex) {
            return;
        } finally {
            if (pw!=null)
                pw.close();
        }
    }

    public static void main(String [] args) {
        try {
            new Oblig2(Integer.parseInt(args[0]),
                    Integer.parseInt(args[1]),
                    Integer.parseInt(args[2]),
                    Integer.parseInt(args[3]),
                    args.length<5?null:args[4]);
        } catch (NumberFormatException | ArrayIndexOutOfBoundsException ex) {
            System.out.println("use: >java Modell2 <nLow> <nStep> <nHigh>"
                    +" <num iterations> <fil>");
        }
    }
}
