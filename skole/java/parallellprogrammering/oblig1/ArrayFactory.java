import java.util.Random;

public class ArrayFactory {

    public static void insert(int[] a, int ins, int i) {
        for (; i > 0 && a[i-1] < ins; i--)
            a[i] = a[i-1];
        a[i] = ins;
    }

    public static int[] randomIntArray(int size, int uLimit, int seed) {
        Random rand = new Random(seed);
        int[] ret = new int[size];

        for (int i = 0; i < size; i++)
            ret[i] = rand.nextInt(uLimit);
        return ret;
    }

    public static int arrayIndexMaxCycle(int[] a,int from,int to,
            int stride,int max) {

        int indx = from;
        for (int i = from; i < to; i+=stride) {
            if (a[i]>max) { indx = i; }
        }
        return indx;
    }

    public static int arrayIndexMaxBlock(int[] a, int start, int seg,
            int max) {

        int indx = start;
        for (int i = 0; i < seg; i++) {
            if (a[start+i] > max) {
                indx = start+i;
                break;
            }
        }
        return indx;
    }

    public static void insertionSortRev(int[] a, int from, int to) {

        for (int i = from; i < to; i++) {
            int tmp = a[i+1];
            int k = i;
            for (; k >= 0 && tmp > a[k]; k--)
                a[k+1] = a[k];
            a[k+1] = tmp;
        }
    }

    public static void insertionSort(double[] a, int from, int to) {

        for (int i = from; i < to; i++) {
            double tmp = a[i+1];
            int k = i;
            for (; k >= 0 && a[k] > tmp; k--)
                a[k+1] = a[k];
            a[k+1] = tmp;
        }
    }

    public static void swap(int[] a, int x, int y) {
        int tmp = a[x];
        a[x] = a[y];
        a[y] = tmp;
    }

}
