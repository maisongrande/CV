class PunktListe {
    static int DEFAULT_SIZE = 16;
    Punkt [] data;
    int len;

    PunktListe(int len) {
        data = new Punkt[Math.max(1,len)];
    }

    PunktListe() {
        data = new Punkt[DEFAULT_SIZE];
    }

    void add(Punkt p) {
        if (len == data.length-1) {
            Punkt [] b = new Punkt[data.length*2];
            for (int i = 0; i < len; i++) {
                b[i] = data[i];
            }
            data = b;
        }
        data[len++] = p;
    }

    void addAt (Punkt elem, int pos) {
        while (pos > data.length) {
            Punkt [] b = new Punkt[data.length*2];
            for (int i = 0; i < data.length; i++) b[i] = data[i];
            data = b;
        }
        len++;
        data[pos] = elem;
    } // end addAt

    void clear() {
        len =0;
    } // end clear;

    Punkt get (int pos) {
        if (pos > len-1 )
            return null;
        else
            return data[pos];
    }//end get

    void swap(int indexP0, int indexP1) {
        Punkt tmp = data[indexP0];
        data[indexP0] = data[indexP1];
        data[indexP1] = tmp;
    }

    int size() {
        return len;
    }//end size
}
