import java.util.ArrayList;
import java.io.FileReader;
import java.io.File;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;

/**TextAlgorithm */
public class TextAlgorithm {
	private static final int POSSIBLE_BYTES = 256;
	private final int NEEDLE_LEN;
	private boolean wildcard;
	private int WC_STEP = 1;
	private ArrayList<Integer> newlines;
	private char[] needle;
	private char[] haystack;


	/**TextAlgorithm
	 * Instanciates a new pattern matching object.
	 * Reads inn both *needle* and *haystack* into two character arrays.
	 * The needle array is striped for new line characters where as the
	 * haystack is not.
	 * If a _-character is found a global wild card flag is raised and the
	 * pattern matching is done according to the rules for wildcards.
	 * All newline characters in the haystack file are counted and later
	 * used as relative reference for mathces. */
	public TextAlgorithm(String n, String hs) {
		BufferedReader br1 = null;
		BufferedReader br2 = null;
		int wc_index = 0;
		try {
			File f1 = new File(n);
			File f2 = new File(hs);
			br1 = new BufferedReader(new FileReader(f1));
			br2 = new BufferedReader(new FileReader(f2));
			try {
				int c;
				needle = new char[(int)f1.length()-1];
				haystack = new char[(int)f2.length()];
				newlines = new ArrayList<Integer>();
				for (int i = 0; (c = br1.read()) != -1; i++) {
					if (c!='\n') { needle[i] = (char)c; }
					if (c=='_') { wildcard = true; }
				}
				for (int i = 0; (c = br2.read()) != -1; i++) {
					if (c=='\n') { newlines.add(i+1); }
					haystack[i] = (char)c;
				}
				if (haystack.length==0) {
					throw new NegativeArraySizeException();
				}
			} catch (NegativeArraySizeException e) {
				System.err.println("Empty Needle"
						+ " or Haystack supplied!");
				System.exit(0);
			}finally { br1.close(); br2.close(); }
		} catch (IOException e) { e.printStackTrace(); }
		if (needle.length > haystack.length)
			throw new IndexOutOfBoundsException();
		NEEDLE_LEN = needle.length;
	}



	/**preprocess
	 * First populate array with the most usual step length, namely the
	 * length of the needle
	 * Then fill in the skip length according to character position. */
	private int[] preprocess() {
		int[] bcs = new int[POSSIBLE_BYTES];
		int last = NEEDLE_LEN-1;
		int wc_idx = NEEDLE_LEN;
		for (int i = 0; i < last; i++) {
			bcs[(int)needle[i]] = last-i;
			if (wildcard && needle[i]=='_') {
				wc_idx = WC_STEP = last-i;
			}
		}
		int step = wildcard ? wc_idx : NEEDLE_LEN;
		for (int i = 0; i < POSSIBLE_BYTES; i++) {
			if (bcs[i]==0 || bcs[i] > step) { bcs[i] = step; }
		}
		if (needle[NEEDLE_LEN-1]=='_') { bcs[(int)'_']= step; }
		return bcs;
	}



	/**boyer_more_horspool
	 * Based on the algorithm in the lecture notes on pattern matching,
	 * plus wiki article on the horspool simplification.
	 * Differences implemented to accomodate the assignment are:
	 * - ArrayList containing all succsessful matches.
	 * - Additional predicate testing for wild card character.
	 * @return result List of matches. */
	public ArrayList<String> boyer_moore_horspool() {
		ArrayList<String> result = new ArrayList<>(haystack.length);
		int[] badCharShift = preprocess();
		int skip = 0;
		int i = 0;
		while ((haystack.length-skip) >= NEEDLE_LEN) {
			i = NEEDLE_LEN - 1;
			while (haystack[skip+i]==needle[i] || needle[i]=='_') {
				if (i==0) {
					result.add(getResult(skip));
					break;
				}
				i--;
			}
			skip += badCharShift[haystack[skip+NEEDLE_LEN-1]];
		}
		return result;
	}



	/**getResult
	 * Creates a string containg line and line index of the first char
	 * where the match was found.
	 * @param index the index where match occured in the haystack array.
	 * @return string containg info of where. */
	private String getResult(int index) {
		char[] res = new char[NEEDLE_LEN];
		int line = 1;
		int linePos = index;
		for (int i = index, j=0; j < NEEDLE_LEN; i++,j++) {
			res[j] = haystack[i];
		}
		for (int i : newlines) {
			if (index >= i) { line++; }
			else { break; }
			linePos = index-i;
		}
		String retStr = "Line: "+line+" Index: " +linePos+" : ";
		return retStr+new String(res);
	}



	@Override
	/**toString
	 * Little bit of info */
	public String toString() {
		int stepsize = wildcard? WC_STEP : NEEDLE_LEN-1;
		return "General info: Wild Card: "+wildcard
			+". step size: "+stepsize;
	}



	/**Print needle and haystack array for debugging purpose*/
	public void print() {
		for (char c : needle) { System.out.print(c);}
		System.out.println();
		for (char c : haystack) { System.out.print(c);}
	}
}
