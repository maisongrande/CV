import java.util.ArrayList;

/** Static methods conforming to rules described in the assignment.
 *  @author Christopher Stoheim */
class StringAlgorithms {

	private static char[] alphabet =
		"abcdefghijklmnopqrstuvwxyz".toCharArray();



	/** A word identical to the input, except that two letters
	 *  next to each other have been switched. */
	public static ArrayList<String> swapAdjacent(String word) {
		ArrayList<String> result = new ArrayList<String>();
		for (int i = 0; i < word.length()-1; i++) {
			result.add(swap(word, i, i+1));
		}
		return result;
	}


	/** A word identical to the input, except one letter has
	 *  been replaced with another. */
	public static ArrayList<String> replaceCharByChar(String word) {
		ArrayList<String> result = new ArrayList<String>();
		for (int i = 0; i < word.length(); i++)
			for (char c : alphabet){result.add(replace(word,i,c));}
		return result;
	}


	/** A word identical to the input, except one letter has been added
	 * in front, at the end, or somewhere in the middle of the word */
	public static ArrayList<String> expandWord(String word) {
		ArrayList<String> result = new ArrayList<String>();
		for (char c : alphabet) { result.add(new String(c+word)); }
		for (int i = 0; i < word.length(); i++) {
			String part1 = word.substring(0,i+1);
			for (char c : alphabet) {
				String part3 = word.substring(i+1);
				result.add(new String(part1+c+part3));
			}
		}
		return result;
	}

	/** word identical to the input, except one letter has been removed.*/
	public static ArrayList<String> shortenWord(String word) {
		ArrayList<String> result = new ArrayList<String>();
		for (int i = 0; i < word.length(); i++)
			result.add( word.substring(0,i)+ word.substring(i+1));
		return result;
	}


	/* Helper methods */

	/** Replaces character at index in string.
	 * @param word		the string to be operated on.
	 * @param index		index of the char that is to be replaced.
	 * @param c		character that is replacing former.
	 * @return String	new string.*/
	protected static String replace(String word, int index, char c) {
		char[] split = word.toCharArray();
		split[index] = c;
		return new String(split);
	}

	/** Swaps to characters in string
	 * @param word		the string that is to be operated on.
	 * @param current	index of first char
	 * @param next		index of next character.
	 * @return String	the newly allocated string.*/
	protected static String swap(String word, int current, int next) {
		char[] split = word.toCharArray();
		char tmp = split[current];
		split[current] = split[next];
		split[next] = tmp;
		return new String(split);
	}
}
