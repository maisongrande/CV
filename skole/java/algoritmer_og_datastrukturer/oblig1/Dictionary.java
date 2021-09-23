import java.util.Scanner;
import java.util.ArrayList;
import java.io.File;


/** Dictionary class
 *  @author Christopher Storheim
 * */
public class Dictionary {
	private static final String filename="dictionary.txt";
	private static final String testString = "busybody";
	private Scanner input;
	private BinarySearchTree searchTree;



	/** Dicitionary */
	public Dictionary() {
		input = new Scanner(System.in);
		searchTree = new BinarySearchTree(new File(filename));
	}



	/** Initial test as described in the assignment.
	 *  Remove the string "busybody" from the tree,
	 *  search for it.
	 *  If found: failure, so exit.
	 *  Else insert again and call user interface method.
	 * */
	protected void init() {
		searchTree.remove(testString);
		if (searchTree.search(testString)) {
			System.out.println("Initial test failed!");
			System.exit(0);
		}
		System.out.println("Ready!");
		searchTree.add(testString);
		uiLoop();
	}



	/** The user interface loop
	 *  Displays prompt and read input, then look up input in
	 *  tree or quit if 'q' is found.
	 *  Print out information about the tree when 'q' is read.
	 * */
	protected void uiLoop() {
		String in;
		int o = 1;
		System.out.print("In [1]: ");
		for (int i = 2; !(in = input.nextLine()).equals("q"); i++) {
			in = in.toLowerCase();
			if (in.length()!=0) {
				System.out.print("Out ["+(o++)+"] : ");
				if (searchTree.search(in))
					System.out.println(in);
				else { notFound(in); }
			}
			System.out.print("In ["+i+"]: ");
		}
		System.out.println("\n"+searchTree);
	}



	/** Iterates over alternative strings generated in method
	 * alternatives()  while doing a look up for match in the tree.
	 *  The process from start to end is timed and all possible
	 *  hits are printed out followed by the total running time.
	 *  @param word the word that did not have a match in the dictionary
	 *  @return void
	 *  */
	protected void notFound(String word) {
		System.out.println("Not found!");
		long start = System.currentTimeMillis();
		boolean found_flag = true;

		for (String s : alternatives(word)) {
			if (searchTree.search(s)) {
				if (found_flag) {
					System.out.println("Alternatives:");
					found_flag = false;
				}
				System.out.printf("\t -%-10s %s %d\n",
						s,
						" #lookups: ",
						searchTree.numberOfLookUps());
			}
		}
		if (!found_flag)
			System.out.println("\nGenerate alternative strings + "
					+"look-up time: "
					+(System.currentTimeMillis()-start)
					+"ms");
	}



	/** Builds an ArrayList of possible strings following the specs
	 *  givens in the assignement.
	 *  @param word		The base string which is being manipulated.
	 *  @return 		List of variants following spec.
	 */
	protected ArrayList<String> alternatives(String word) {
		ArrayList<String> result = new ArrayList<>();
		result.addAll(StringAlgorithms.swapAdjacent(word));
		result.addAll(StringAlgorithms.expandWord(word));
		result.addAll(StringAlgorithms.replaceCharByChar(word));
		result.addAll(StringAlgorithms.shortenWord(word));
		return result;
	}



	/** Main: create new Dictionary object and call its init method. */
	public static void main(String ... args) {
		System.out.print("Initializing...");
		(new Dictionary()).init();
	}
}
