public class ParallellKohyll {
    int MIN = 0, MAX = 1;
    int maxDybde;
    final int START_THREADS = 2;

    IntList paraKohyllInit(PunktListe punkter) {
        // Finn antall traader max:
        maxDybde = Runtime.getRuntime().availableProcessors();

        // Finn punktene med stoerst avstand aka extremalpunktene til linjen.
        PunktListe extremalPunkt = Punkter.finnExtremaler(punkter, 0, punkter.size());
        Punkt extremMin = extremalPunkt.get(MIN);
        Punkt extremMax = extremalPunkt.get(MAX);

        // Returnerer minpunkt-indeks og maxpunkt-indeks fra linja.
        PunktListe avstandFraLinje = Punkter.minMaxAvstandFraLinje(extremMin, extremMax, punkter);
        Punkt punktLengstTilHoyreForLinje = avstandFraLinje.get(MIN);
        Punkt punktLengstTilVenstreForLinje = avstandFraLinje.get(MAX);

        //TODO: REMOVE?
        //setVinduStoerrelse(extremalPunkt, avstandFraLinje);

        // Finn alle punkter over og under- og pa linjen.
        PunktListe punkterOverLinje = Punkter.overLinje(extremMin, extremMax, punkter);
        PunktListe punkterPaEllerUnderLinje = Punkter.paEllerUnderLinje(extremMin, extremMax, punkter);

        // Begynner paa minX->maxX saa minX legges in lista:
        IntList innhyllOverlinje = new IntList();
        IntList innhyllUnderLinje = new IntList();

        innhyllOverlinje.add(extremMin.index);
        innhyllUnderLinje.add(extremMax.index);

        // definerer startdybde i rekursjonstreet til aa vaere 1
        int startDybde = 1;

        // Finn innhyllingen for alle punkter over hovedlinja.
        Thread overLinje = new Thread(new KonvInhyll(extremMin,
                    extremMax,
                    punktLengstTilVenstreForLinje,
                    punkterOverLinje,
                    innhyllOverlinje,
                    startDybde));
        overLinje.start();

        // Finn innhyllingen for alle punkter under hovedlinja.
        Thread underLinje = new Thread(new KonvInhyll(extremMax,
                    extremMin,
                    punktLengstTilHoyreForLinje,
                    punkterPaEllerUnderLinje,
                    innhyllUnderLinje,
                    startDybde));
        underLinje.start();

        try {
            overLinje.join();
            underLinje.join();
        } catch (InterruptedException ex) { return null; }

        IntList koHyll = new IntList((innhyllOverlinje.size()
                    + innhyllUnderLinje.size()));

        //printpunkter(innhyllOverlinje);
        //printpunkter(innhyllUnderLinje);

        for (int i = 0; i < innhyllOverlinje.size(); i++) {
            koHyll.add(innhyllOverlinje.get(i));
        }

        for (int i = 0; i < innhyllUnderLinje.size(); i++) {
            koHyll.add(innhyllUnderLinje.get(i));
        }

        return koHyll;
    }


    class KonvInhyll implements Runnable {
        Punkt A, B, C;
        PunktListe m;
        IntList kohyll;
        int startDybde;

        KonvInhyll(Punkt A, Punkt B, Punkt C, PunktListe m, IntList kohyll, int startDybde) {
            this.A = A;
            this.B = B;
            this.C = C;
            this.m = m;
            this.kohyll = kohyll;
            this.startDybde = startDybde;
        }

        public void run() {
            paraRek(A, B, C, m, kohyll, startDybde);
        }

        void paraRek(Punkt A, Punkt B, Punkt C,
                PunktListe pktListe, IntList minKohyll, int dybdeNa)
        {
            //System.out.println("Dybde naa: "+dybdeNa);
            // System.out.print("("+C.x+", "+C.y+"), ");

            if (pktListe.size() == 0) {
                minKohyll.add(C.index);
                return;
            } else if (dybdeNa == maxDybde) {
                Punkter.sekvRek(A, B, C, pktListe, minKohyll);
            } else {
                // Inkrementer nivapeker
                dybdeNa = dybdeNa + 1;

                // Rekurser venstre gren:
                Punkt maxPunktFraLinjeAC =
                    (Punkter.minMaxAvstandFraLinje(A, C, pktListe)).get(MAX);
                PunktListe punkterOverLinjeAC = Punkter.overLinje(A, C, pktListe);
                IntList vKH = new IntList();
                Thread AC = new Thread (
                        new KonvInhyll(A, C,
                            maxPunktFraLinjeAC,
                            punkterOverLinjeAC,
                            vKH,
                            dybdeNa));

                // Rekurser hoyre gren:
                Punkt maxPunktFraLinjeCB =
                    (Punkter.minMaxAvstandFraLinje(C, B, pktListe)).get(MAX);
                PunktListe punkterOverLinjeCB = Punkter.overLinje(C, B, pktListe);
                IntList hKH = new IntList();
                Thread CB = new Thread (
                        new KonvInhyll(C, B,
                            maxPunktFraLinjeCB,
                            punkterOverLinjeCB,
                            hKH,
                            dybdeNa));

                AC.start();
                CB.start();

                // Samle sammen traadene:
                try { AC.join(); CB.join(); }
                catch (InterruptedException ex) { return; }

                for (int i = 0; i < vKH.size(); i++)
                    minKohyll.add(vKH.get(i));

                minKohyll.add(C.index);
                for (int i = 0; i < hKH.size(); i++)
                    minKohyll.add(hKH.get(i));
            }
        }
    }

        synchronized void printpunkter(IntList koHyll) {
            System.out.print("Punkter: ");
            for (int i = 0; i < koHyll.size(); i++) {
                System.out.print(koHyll.get(i)+", ");
            }
            System.out.println();
        }
}
