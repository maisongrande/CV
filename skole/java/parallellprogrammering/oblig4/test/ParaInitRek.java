import java.util.concurrent.*;

class ParaInitRek {
    static final int MIN = 0, MAX = 1;
    static int numThreads;
    static CyclicBarrier vent, ferdig, traader;
    static PunktListe punkter;

    static IntList koHyll;
    static IntList [] fellesKohyll;

    static PunktListe [] endePunkter;
    static Punkt minEndepunkt;
    static Punkt maxEndepunkt;

    static PunktListe [] fellesPunkter;
    static volatile PunktListe fellesUnderLinje;
    static volatile PunktListe fellesOverLinje;
    int [] antUnderLinje;
    int [] antOverLinje;

    static PunktListe [] fellesStoerstAvstand;
    static Punkt lengstTilVenstre;
    static Punkt lengstTilHoyre;

    volatile boolean done;

    ParaInitRek(int nThreads) {
        this.numThreads = nThreads;

        vent = new CyclicBarrier(numThreads + 1);
        ferdig = new CyclicBarrier(numThreads + 1);
        traader = new CyclicBarrier(numThreads);

        endePunkter = new PunktListe[numThreads];
        fellesPunkter = new PunktListe[numThreads];
        fellesKohyll = new IntList[numThreads];

        antUnderLinje = new int[numThreads];
        antOverLinje = new int[numThreads];

        fellesStoerstAvstand = new PunktListe[numThreads];

        done = false;


        for (int traadId = 0;
                traadId < numThreads;
                traadId++)
        {
            (new Thread(new InitRunner(traadId))).start();
        }
    }

    class InitRunner implements Runnable {
        int id, startAt, stopAt, segment;
        Punkt minMax, minMin;
        IntList minKohyll;
        PunktListe mineOver, mineUnder;
        int antOver, antUnder;
        int minDybde;

        InitRunner(int id) {
            this.id = id;
            minDybde = 0;
            antOver = 0;
            antUnder = 0;
            minKohyll = new IntList();
        }

        @Override
        public void run() {
            do {

                try { vent.await(); }
                catch (InterruptedException | BrokenBarrierException ex)
                { return; }

                if (!done) {
                    minePunkter();
                    try { ferdig.await(); }
                    catch (InterruptedException | BrokenBarrierException ex)
                    { return; }
                }

            } while (!done);
        }

        void minePunkter() {
            init();
            finnLinje();
            punkterOverUnderLinje();
        }


        void init() {
            startAt = id * (punkter.size() / numThreads);
            segment = (id != (numThreads-1))
                ? punkter.size() / numThreads
                : punkter.size() - startAt;
            stopAt =  (id != (numThreads-1))
                ? (id + 1)*(punkter.size()/numThreads)
                : punkter.size();
        }


        void finnLinje() {
            endePunkter[id] = Punkter.finnExtremaler(punkter, startAt, stopAt);
            minMin = endePunkter[id].get(MIN);
            minMax = endePunkter[id].get(MAX);

            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }

            if (id == 0) {
                for (int i = 1; i < numThreads; i++) {
                    Punkt tmpMin = endePunkter[i].get(MIN);
                    Punkt tmpMax = endePunkter[i].get(MAX);
                    if (tmpMin.x < minMin.x) { minMin = tmpMin; }
                    if (tmpMax.x > minMax.x) { minMax = tmpMax; }
                }
                minEndepunkt = minMin;
                maxEndepunkt = minMax;
            }
            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }
        }

        void punkterOverUnderLinje() {
            mineOver = Punkter.overLinje(minEndepunkt, maxEndepunkt, punkter, startAt, stopAt);
            mineUnder = Punkter.paEllerUnderLinje(minEndepunkt, maxEndepunkt, punkter, startAt, stopAt);

            antOverLinje[id] = antOver = mineOver.size();
            antUnderLinje[id] = antUnder = mineUnder.size();

            fellesStoerstAvstand[id] = Punkter.minMaxAvstandFraLinje(minEndepunkt, maxEndepunkt, punkter, startAt, stopAt);
            minMin = fellesStoerstAvstand[id].get(MIN);
            minMax = fellesStoerstAvstand[id].get(MAX);

            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }

            if (id == 0) {
                //TODO MULIG DETTE SUGER RAEV!
                PunktListe liste = new PunktListe(numThreads);
                for (int i = 0; i < numThreads; i++) {
                    liste.add(fellesStoerstAvstand[i].get(MIN));
                    liste.add(fellesStoerstAvstand[i].get(MAX));
                }
                PunktListe tmp = Punkter.minMaxAvstandFraLinje(minEndepunkt, maxEndepunkt, liste, 0, 2 * numThreads);

                lengstTilHoyre = tmp.get(MIN);
                lengstTilVenstre =  tmp.get(MAX);

                int under = 0;
                for (int i : antUnderLinje) { under += i; }
                fellesUnderLinje = new PunktListe(under);

                int over = 0;
                for (int i : antOverLinje) { over += i; }
                fellesOverLinje = new PunktListe(over);
            }

            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }

            int minStartOver = 0;
            for (int i = 0; i < id; i++) {
                minStartOver += antOverLinje[i];
            }

            for (int i = 0; i <  antOver; i++) {
                fellesOverLinje.addAt(mineOver.get(i), minStartOver++);
            }

            int minStartUnder = 0;
            for (int i = 0; i < id; i++) {
                minStartUnder += antUnderLinje[i];
            }

            for (int i = 0; i <  antUnder; i++) {
                fellesUnderLinje.addAt(mineUnder.get(i), minStartUnder++);
            }

            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }
        }

    }

        static InitKohyllInfo initKohyll(PunktListe p) {
            punkter = p;

            try { vent.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return null; }

            try { ferdig.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return null; }

            InitKohyllInfo iki = new InitKohyllInfo();

            iki.punkterOverLinje = fellesOverLinje;
            iki.punkterUnderLinje = fellesUnderLinje;
            iki.minEndepunkt = minEndepunkt;
            iki.maxEndepunkt = maxEndepunkt;
            iki.lengstTilHoyre = lengstTilHoyre;
            iki.lengstTilVenstre = lengstTilVenstre;
            return iki;
        }

        synchronized void exit() {
            done = true;
            vent.reset();
            ferdig.reset();
        }
}

    class InitKohyllInfo {
        PunktListe  punkterOverLinje;
        PunktListe  punkterUnderLinje;
        Punkt       minEndepunkt;
        Punkt       maxEndepunkt;
        Punkt       lengstTilHoyre;
        Punkt       lengstTilVenstre;
    }
