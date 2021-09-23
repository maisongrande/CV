class Oblig4 {
    static int MIN = 0, MAX = 1;
    static final boolean TEGNUT = true;
    double[] seqTime, parTime;
    int lowerN, upperN, divisor, nIter;
    int[] x, y;
    PunktListe punkter;
    NPunkter points;
    int n, MIN_X, MAX_X, MIN_Y, MAX_Y;
    TegnUt tu;

    public Oblig4(String[] args) {
        this.lowerN = Integer.parseInt(args[0]);
        this.divisor = Integer.parseInt(args[1]);
        this.upperN = Integer.parseInt(args[2]);
        this.nIter = Integer.parseInt(args[3]);
        System.out.println("Test av Konveks Innhylling"
                +(lowerN!=upperN ? (" fra "+upperN+" til "+lowerN)
                    : (" for "+upperN))+" punkter, "+nIter
                +(nIter>1?" ganger.":" gang."));
        System.out.printf("%8s%20s%20s%10s\n", "Punkter:", "Sekvensiell Tid:",
                "Parallell tid:", "Speedup:");
        execute();
    }

    void execute() {

        seqTime = new double[nIter];
        parTime = new double[nIter];
        int antTraader = Runtime.getRuntime().availableProcessors();
        //TODO: REMOVE
        //ParaKoHyll parallell = new ParaKoHyll(antTraader);
        ParallellKohyll pk = new ParallellKohyll();

        for (int N = upperN;
                N >= lowerN;
                N/=divisor)
        {
            x = new int[N];
            y = new int[N];
            n = N;
            points = new NPunkter(N);
            points.fyllArrayer(x, y);
            punkter = new PunktListe(n);

            for (int i = 0; i < n; i++) { punkter.add(new Punkt(x[i], y[i], i)); }

            for (int run = 0;
                    run < nIter;
                    run++)
            {
                long tic = System.nanoTime();
                IntList sekvKH = runSequential();
                if (TEGNUT) { tu = new TegnUt(this, sekvKH, "SEKVENSIELL"); }
                seqTime[run] = ((System.nanoTime() - tic)/1000000.0f);
                tic = System.nanoTime();
                IntList paraKH = pk.paraKohyllInit(punkter);
                parTime[run] = ((System.nanoTime() - tic)/1000000.0f);
                if (TEGNUT) { tu = new TegnUt(this, paraKH, "PARALLELL"); }
            }
            showResult(N);
        }
            //TODO: REMOVE:
            //parallell.exit();
    }


    void setVinduStoerrelse(PunktListe xMinMax, PunktListe yMinMax) {
        MIN_X = xMinMax.get(MIN).x;
        MAX_X = xMinMax.get(MAX).x;
        MIN_Y = yMinMax.get(MIN).y;
        MAX_Y = yMinMax.get(MAX).y;
    }


    IntList runSequential() {
        // Finn punktene med stoerst avstand aka extremalpunktene til linjen.
        PunktListe extremalPunkt = Punkter.finnExtremaler(punkter, 0, punkter.size());
        Punkt extremMin = extremalPunkt.get(MIN);
        Punkt extremMax = extremalPunkt.get(MAX);

        // Returnerer minpunkt-indeks og maxpunkt-indeks fra linja.
        PunktListe avstandFraLinje = Punkter.minMaxAvstandFraLinje(extremMin, extremMax, punkter);
        Punkt punktLengstTilHoyreForLinje = avstandFraLinje.get(MIN);
        Punkt punktLengstTilVenstreForLinje = avstandFraLinje.get(MAX);

        setVinduStoerrelse(extremalPunkt, avstandFraLinje);

        // Finn alle punkter over og under- og pa linjen.
        PunktListe punkterOverLinje = Punkter.overLinje(extremMin, extremMax, punkter);
        PunktListe punkterPaEllerUnderLinje = Punkter.paEllerUnderLinje(extremMin, extremMax, punkter);

        // TODO: Fjernes foer innlevering
        //DebugPrint(extremMin, extremMax,
        //        punktLengstTilHoyreForLinje, punktLengstTilVenstreForLinje,
        //        punkterPaEllerUnderLinje,
        //        punkterOverLinje);

        // Begynner paa minX->maxX saa minX legges in lista:
        IntList koHyll = new IntList();
        koHyll.add(extremMin.index);

        // Finn innhyllingen for alle punkter over hovedlinja.
        Punkter.sekvRek(extremMin, extremMax,
                punktLengstTilVenstreForLinje, // Her er perspektivet snudd
                punkterOverLinje,
                koHyll);

        // Legg til neste extremalpunkt
        koHyll.add(extremMax.index);

        // Finn så innhyllingen for alle punkter under hovedlinja.
        Punkter.sekvRek(extremMax, extremMin,
                punktLengstTilHoyreForLinje,
                punkterPaEllerUnderLinje,
                koHyll);

        return koHyll;
    }

    double arrayMedian(double[] a) {
        int len = a.length;
        for (int i = 0; i < len-1; i++) {
            int k = i;
            double tmp = a[i+1];
            for (; k >= 0 && a[k] > tmp; k--)
                a[k+1] = a[k];
            a[k+1] = tmp;
        }
        return len%2==0 ? ((a[((len/2)-1)]+a[len/2])/2) : a[len/2];
    }





    // TODO: Fjernes foer innlevering
    void printKoHyll(IntList koHyll) {
        System.out.print("KoHyll: ");
        for (int i = 0; i < koHyll.size(); i++)
            System.out.print("("+x[koHyll.get(i)]+", "+y[koHyll.get(i)]+")");
        System.out.println();
    }

    // TODO: Fjernes foer innlevering
    void showResult(int run) {
        int genSpace = Integer.toString(upperN).length();

        System.out.printf("%-12d %-20.5f %-15.5f %.3f\n",
                run,
                arrayMedian(seqTime),
                arrayMedian(parTime),
                (arrayMedian(seqTime) / arrayMedian(parTime)));
    }



    // TODO: Fjernes foer innlevering
    void DebugPrint(Punkt p0, Punkt p1, Punkt max, Punkt min, PunktListe punkterPaEllerUnderLinje, PunktListe punkterOverLinje)
    {
        double a = p0.y - p1.y;
        double b = p1.x - p0.x;
        double c = (p1.y * p0.x) - (p0.y * p1.x);

        /** DEBUG UTSKRIFT */
        for (int i = 0; i < n; i++) {
            System.out.println(i+": ("+x[i]+", "+y[i]+") == ("+punkter.get(i).x+", "+punkter.get(i).y+")");
        }
        System.out.println();
        System.out.println("Extremal: min: ("+p0.x+", "+p0.y+") max: ("+p1.x+", "+p1.y+") ");
        System.out.println("Max: fra linjen: "+a+"x + "+b+"y + "+c);
        System.out.println("Punkt lengst til hoyre: Punktet: ("+min.x+", "+min.y+") ");
        System.out.println("Punkt lengst til venstre: Punktet: ("+max.x+", "+max.y+") ");

        System.out.println("Punkter under linja:");
        for (int i = 0; i < punkterPaEllerUnderLinje.size(); i++) {
            Punkter.printPunkt(punkterPaEllerUnderLinje.get(i));
        }

        System.out.println("Punkter over linja:");
        for (int i = 0; i < punkterOverLinje.size(); i++) {
            Punkter.printPunkt(punkterOverLinje.get(i));
        }

    }

    public static void main(String...args) {
        if (args.length != 4) {
            System.out.println("java Oblig4 [nedre N] [dividend] [ovre N]"
                    + " [antall iter] [filnavn]");
        } else {
            Oblig4 ob4 = new Oblig4(args);
        }
    }
}

class Punkter {
    static final int MAX = 1;
    static final int MIN = 0;

    /***/
    static void
        sekvRek(Punkt A, Punkt B, Punkt C, PunktListe pktListe, IntList koHyll) {
        if (pktListe.size() != 0) {
            // Finn maxpunktene fra de to linjene som fremkommer mellom
            // extremalpunktene og punktet med stoerst avstand fra linjen
            // mellom disse
            Punkt maxPunktFraLinjeAC = (Punkter.minMaxAvstandFraLinje(A, C, pktListe)).get(MAX);
            Punkt maxPunktFraLinjeCB = (Punkter.minMaxAvstandFraLinje(C, B, pktListe)).get(MAX);

            // Finn alle punkter som befinner seg utenfor trekanten som
            // innhylles av overnevnte linjer.
            PunktListe punkterOverLinjeAC = Punkter.overLinje(A, C, pktListe);
            PunktListe punkterOverLinjeCB = Punkter.overLinje(C, B, pktListe);

            // Gjenta for de nye linjene og legg foerste split-punkt i listen
            // av punkter for a oppnaa riktig rekkefoelge.
            sekvRek(A, C, maxPunktFraLinjeAC, punkterOverLinjeAC, koHyll);
            koHyll.add(C.index);
            sekvRek(C, B, maxPunktFraLinjeCB, punkterOverLinjeCB, koHyll);
        }

    }

    static void
    printPunkt(Punkt punkt)
    {
        System.out.println("("+punkt.x+", "+punkt.y+")");
    }

    static void
    printPunkt(String info, Punkt punkt)
    {
        System.out.println(info+"("+punkt.x+", "+punkt.y+")");
    }

    /**Funksjonen returnerer et par av de punkter som befiner seg lengst unna,
     * pa oversiden og undersiden av linjen som fremkommer mellom punktene
     * extremMin og extremMax.
     * */
    static PunktListe
    minMaxAvstandFraLinje(Punkt extremMin, Punkt extremMax, PunktListe punkter, int start, int stop)
    {
        int a = extremMin.y - extremMax.y;
        int b = extremMax.x - extremMin.x;
        int c = (extremMax.y * extremMin.x) - (extremMin.y * extremMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        Punkt p0 = punkter.get(0);

        Punkt maxAvstand = new Punkt(p0.x, p0.y, p0.index);
        Punkt minAvstand = new Punkt(p0.x, p0.y, p0.index);

        double max = 0;
        double min = 0;

        for (int i = start; i < stop; i++) {

            double avstandFraLinje =
                ((a * punkter.get(i).x + b * punkter.get(i).y + c) / divisor);

            if (avstandFraLinje <= min) {
                min = avstandFraLinje;
                minAvstand = punkter.get(i);
            }

            if (avstandFraLinje >= max) {
                max = avstandFraLinje;
                maxAvstand = punkter.get(i);
            }
        }

        PunktListe result = new PunktListe(2);
        result.add(minAvstand);
        result.add(maxAvstand);

        return result;
    }

    /**Funksjonen returnerer et par av de punkter som befiner seg lengst unna,
     * pa oversiden og undersiden av linjen som fremkommer mellom punktene
     * extremMin og extremMax.
     * */
    static PunktListe
    minMaxAvstandFraLinje(Punkt extremMin, Punkt extremMax, PunktListe punkter)
    {
        int a = extremMin.y - extremMax.y;
        int b = extremMax.x - extremMin.x;
        int c = (extremMax.y * extremMin.x) - (extremMin.y * extremMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        Punkt p0 = punkter.get(0);

        Punkt maxAvstand = new Punkt(p0.x, p0.y, p0.index);
        Punkt minAvstand = new Punkt(p0.x, p0.y, p0.index);

        double max = 0;
        double min = 0;

        for (int i = 0; i < punkter.size(); i++) {

            double avstandFraLinje =
                ((a * punkter.get(i).x + b * punkter.get(i).y + c) / divisor);

            if (avstandFraLinje <= min) {
                min = avstandFraLinje;
                minAvstand = punkter.get(i);
            }

            if (avstandFraLinje >= max) {
                max = avstandFraLinje;
                maxAvstand = punkter.get(i);
            }
        }

        PunktListe result = new PunktListe(2);
        result.add(minAvstand);
        result.add(maxAvstand);

        return result;
    }


    /**Gitt en sampling punkter returnerer funksjonen et par av de to punktene
     * som ligger lengst fra hverandre pa x-aksen
     * */
    static PunktListe
    finnExtremaler(PunktListe punkter, int fra, int til)
    {

        Punkt p0 = punkter.get(0);
        Punkt minPunkt = new Punkt(p0.x, p0.y, p0.index);
        Punkt maxPunkt = new Punkt(p0.x, p0.y, p0.index);

        for (int i = fra; i < til; i++) {
           if (punkter.get(i).x > maxPunkt.x) { maxPunkt = punkter.get(i); }
           if (punkter.get(i).x < minPunkt.x) { minPunkt = punkter.get(i); }
        }

        PunktListe result = new PunktListe(2);
        result.add(minPunkt);
        result.add(maxPunkt);
        return result;
    }

    /** Funksjonen tar inn to endepunkter: extremalMin og extremalMax, samt en
     * liste over alle punkter i planet og returnerer en liste med  punkter som
     * befinner seg over linjen som fremkommer mellom endepunktene.
     * */
    static PunktListe
    overLinje(Punkt extremalMin, Punkt extremalMax, PunktListe punkter, int start, int stop)
    {
        PunktListe result = new PunktListe();

        int a = extremalMin.y - extremalMax.y;
        int b = extremalMax.x - extremalMin.x;
        int c = (extremalMax.y*extremalMin.x) - (extremalMin.y*extremalMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        for (int i = start; i < stop; i++) {
            Punkt p = punkter.get(i);
            double avstandFraLinje = (a * p.x + b * p.y + c) / divisor;

            if (avstandFraLinje > 0) {
                if ((p.x != extremalMax.x || p.y != extremalMax.y)
                        && (p.x != extremalMin.x || p.y != extremalMin.y))
                {
                    result.add(p);
                }
            }
        }
        return result;
    }

    /** Funksjonen tar inn to endepunkter: extremalMin og extremalMax, samt en
     * liste over alle punkter i planet og returnerer en liste med punkter som
     * befinner seg pa eller under linjen som fremkommer mellom endepunktene.
     * */
    static PunktListe
    paEllerUnderLinje(Punkt exMin, Punkt exMax, PunktListe punkter, int start, int stop)
    {
        PunktListe result = new PunktListe();

        double a = exMin.y - exMax.y;
        double b = exMax.x - exMin.x;
        double c = (exMax.y * exMin.x) - (exMin.y * exMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        for (int i = start; i < stop; i++) {
            Punkt p = punkter.get(i);
            double avstandFraLinje = (a * p.x + b * p.y + c) / divisor;

            if (avstandFraLinje <= 0) {
                if ((p.x != exMax.x || p.y != exMax.y)
                        && (p.x != exMin.x || p.y != exMin.y))
                {
                    result.add(p);
                }
            }
        }
        return result;
    }

    /** Funksjonen tar inn to endepunkter: extremalMin og extremalMax, samt en
     * liste over alle punkter i planet og returnerer en liste med  punkter som
     * befinner seg over linjen som fremkommer mellom endepunktene.
     * */
    static PunktListe
    overLinje(Punkt extremalMin, Punkt extremalMax, PunktListe punkter)
    {
        PunktListe result = new PunktListe();

        int a = extremalMin.y - extremalMax.y;
        int b = extremalMax.x - extremalMin.x;
        int c = (extremalMax.y*extremalMin.x) - (extremalMin.y*extremalMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        for (int i = 0; i < punkter.size(); i++) {
            Punkt p = punkter.get(i);
            double avstandFraLinje = (a * p.x + b * p.y + c) / divisor;

            // TODO: Endret fra avstand fra linje > 0 -> >= 0 5.5.16
            if (avstandFraLinje > 0) {
                if ((p.x != extremalMax.x || p.y != extremalMax.y)
                        && (p.x != extremalMin.x || p.y != extremalMin.y))
                {
                    result.add(p);
                }
            }
        }
        return result;
    }

    /** Funksjonen tar inn to endepunkter: extremalMin og extremalMax, samt en
     * liste over alle punkter i planet og returnerer en liste med punkter som
     * befinner seg pa eller under linjen som fremkommer mellom endepunktene.
     * */
    static PunktListe
    paEllerUnderLinje(Punkt exMin, Punkt exMax, PunktListe punkter)
    {
        PunktListe result = new PunktListe();

        double a = exMin.y - exMax.y;
        double b = exMax.x - exMin.x;
        double c = (exMax.y * exMin.x) - (exMin.y * exMax.x);
        double divisor = Math.sqrt(a * a + b * b);

        for (int i = 0; i < punkter.size(); i++) {
            Punkt p = punkter.get(i);
            double avstandFraLinje = (a * p.x + b * p.y + c) / divisor;

            if (avstandFraLinje <= 0) {
                if ((p.x != exMax.x || p.y != exMax.y)
                        && (p.x != exMin.x || p.y != exMin.y))
                {
                    result.add(p);
                }
            }
        }
        return result;
    }
}
