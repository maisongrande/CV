import java.util.ArrayList;

public class AssignmentThree {
	private TextAlgorithm ta;
	private ArrayList<String> results;

	/**Constructs a new pattern matching object with filenames from cmdl */
	public AssignmentThree(String needle, String haystack) {
		ta = new TextAlgorithm(needle, haystack);
	}

	/**Print out results from calling the pattern matcher object */
	private void run() {
		for (String s : ta.boyer_moore_horspool()) {
			System.out.println(s);
		}
		System.out.println(ta);
	}

	/** Prints out a usage hint then exits. */
	private static void usage() {
		System.out.println("Java AssignmentThree <Needle> <Haystack>");
		System.exit(0);
	}

	/**main method*/
	public static void main(String ... args) {
		if (args.length != 2) { usage(); }
		(new AssignmentThree(args[0],args[1])).run();
	}
}
