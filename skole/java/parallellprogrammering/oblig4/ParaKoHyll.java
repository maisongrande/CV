import java.util.concurrent.*;

class ParaKoHyll {
    static final int MIN = 0, MAX = 1;
    static int numStartThreads;
    static int numThreads;
    static CyclicBarrier vent, ferdig, traader;
    static PunktListe punkter;
    static int maxDybde;

    static int [] antKohyllPunkter;
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

    ParaKoHyll(int nThreads) {
        this.numThreads = nThreads;
        maxDybde = (int)(Math.log((int)numThreads)/Math.log(2.0));

        vent = new CyclicBarrier(numThreads + 1);
        ferdig = new CyclicBarrier(numThreads + 1);
        traader = new CyclicBarrier(numThreads);

        endePunkter = new PunktListe[numThreads];
        fellesPunkter = new PunktListe[numThreads];
        fellesKohyll = new IntList[numThreads];
        antKohyllPunkter = new int[numThreads];

        antUnderLinje = new int[numThreads];
        antOverLinje = new int[numThreads];

        fellesStoerstAvstand = new PunktListe[numThreads];
        done = false;

        numStartThreads = 2;

        for (int traadId = 0;
                traadId < numThreads;
                traadId++)
        {
            (new Thread(new KoHyllRunner(traadId))).start();
        }
    }

    class KoHyllRunner implements Runnable {
        int id, startAt, stopAt, segment;
        Punkt minMax, minMin;
        IntList minKohyll;
        PunktListe mineOver, mineUnder;
        int antOver, antUnder;
        int minDybde;

        KoHyllRunner(int id) {
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

            switch (id) {
                case 0:
                    // 2. kvadrant
                    paraRek(minEndepunkt,
                            lengstTilVenstre,
                            Punkter.minMaxAvstandFraLinje(minEndepunkt,
                                lengstTilVenstre, fellesOverLinje).get(MAX),
                                fellesOverLinje, minKohyll);
                    paraRek(minEndepunkt,
                            lengstTilVenstre,
                            Punkter.minMaxAvstandFraLinje(minEndepunkt,
                                lengstTilVenstre, fellesOverLinje).get(MIN),
                                fellesOverLinje, minKohyll);
                    break;
                case 1:
                    // 1. Kvadrant
                    paraRek(lengstTilVenstre,
                            maxEndepunkt,
                            Punkter.minMaxAvstandFraLinje(lengstTilVenstre,
                                maxEndepunkt, fellesOverLinje).get(MAX),
                                fellesOverLinje, minKohyll);
                    paraRek(lengstTilVenstre,
                            maxEndepunkt,
                            Punkter.minMaxAvstandFraLinje(lengstTilVenstre,
                                maxEndepunkt, fellesOverLinje).get(MIN),
                                fellesOverLinje, minKohyll);
                    break;
                case 2:
                    // 4. Kvadrant
                    paraRek(minEndepunkt,
                            lengstTilHoyre,
                            Punkter.minMaxAvstandFraLinje(minEndepunkt,
                                lengstTilHoyre, fellesUnderLinje).get(MIN),
                                fellesUnderLinje, minKohyll);
                    paraRek(minEndepunkt,
                            lengstTilHoyre,
                            Punkter.minMaxAvstandFraLinje(minEndepunkt,
                                lengstTilHoyre, fellesUnderLinje).get(MAX),
                                fellesUnderLinje, minKohyll);
                    break;
                case 3:
                    // 3. Kvadrant
                    paraRek(lengstTilHoyre,
                            maxEndepunkt,
                            Punkter.minMaxAvstandFraLinje(lengstTilHoyre,
                                maxEndepunkt, fellesUnderLinje).get(MIN),
                                fellesUnderLinje, minKohyll);
                    paraRek(lengstTilHoyre,
                            maxEndepunkt,
                            Punkter.minMaxAvstandFraLinje(lengstTilHoyre,
                                maxEndepunkt, fellesUnderLinje).get(MAX),
                                fellesUnderLinje, minKohyll);
                default:
                    break;
            }

            fellesKohyll[id] = minKohyll;
            antKohyllPunkter[id] = minKohyll.size();

            try { traader.await(); }
            catch (InterruptedException | BrokenBarrierException ex)
            { return; }


            if (id == 0) {

                int kohyllSum = 0;
                for (int i = 0; i < numThreads; i++) {
                    kohyllSum += antKohyllPunkter[i];
                }
                koHyll = new IntList(kohyllSum);

                for (int i = 0; i < numThreads; i++) {
                    IntList tmp = fellesKohyll[i];
                    for (int k = 0; k < tmp.size(); k++) {
                        koHyll.add(tmp.get(k));
                    }
                }
            }

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

            //partisjonerOverUnder(minEndepunkt, maxEndepunkt, punkter, startAt, stopAt);

            //try { traader.await(); }
            //catch (InterruptedException | BrokenBarrierException ex)
            //{ return; }

            //if (id == 3) {
            //    System.out.print("Punkter: ");
            //    for (int i = 0; i < punkter.size(); i++) {
            //        Punkt p = punkter.get(i);
            //        System.out.print("("+p.x+", "+p.y+")");
            //        if ((i+1) % 5 == 0) { System.out.print(" | ");}
            //    }
            //    System.out.println();
            //}
            //try { traader.await(); }
            //catch (InterruptedException | BrokenBarrierException ex)
            //{ return; }
            //System.out.println(id+" Ant over: "+antOver+" Ant under: "+antUnder);
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

        void paraRek(Punkt A, Punkt B, Punkt C, PunktListe m, IntList minKohyll) {
            //System.out.println(id+": A:("+A.x+","+A.y+") B:("+B.x+","+B.y+") C:("+C.x+","+C.y+")");
            Punkter.sekvRek(A, B, C, m, minKohyll);
            if (maxDybde == minDybde) {
            } else {

            }
        }


        void partisjonerOverUnder(Punkt exMin, Punkt exMax, PunktListe p, int start, int stop) {
            double a = exMin.y - exMax.y;
            double b = exMax.x - exMin.x;
            double c = (exMax.y * exMin.x) - (exMin.y * exMax.x);
            double divisor = Math.sqrt(a * a + b * b);

            int left = start;
            int right = stop - 1;

            while (left <= right) {
                while (((a*p.get(left).x + b*p.get(left).y + c)/divisor) < 0) {
                    left++;
                }
                while (((a*p.get(right).x + b*p.get(right).y + c)/divisor) > 0) {
                    right--;
                }

                if (left <= right) {
                    antOver++;
                    p.swap(left, right);
                    left++;
                    right--;
                }
            }
            antUnder = segment-antOver;
        }
    }


    static IntList finnKoHyll(PunktListe p) {
        punkter = p;

        try { vent.await(); }
        catch (InterruptedException | BrokenBarrierException ex)
        { return null; }

        try { ferdig.await(); }
        catch (InterruptedException | BrokenBarrierException ex)
        { return null; }

        System.out.println("KoHyll: "+koHyll.size());
        return koHyll;
    }

    synchronized void exit() {
        done = true;
        vent.reset();
        ferdig.reset();
    }

    public static void main(String ... args) {

    }

}
