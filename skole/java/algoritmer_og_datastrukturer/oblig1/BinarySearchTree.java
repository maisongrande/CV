import java.util.Scanner;
import java.util.ArrayList;
import java.io.File;
import java.io.IOException;
import java.util.NoSuchElementException;
import java.util.concurrent.atomic.AtomicInteger;


/** Binary Search Tree
 *  @author Christopher Storheim
 *  Works mostly as a driver class for the (internal) node class. */
public class BinarySearchTree {
	private Scanner sc;
	protected Node<String> root;
	protected int numberOfNodes = 0;
	protected AtomicInteger lookUps = new AtomicInteger(0);
	protected static final int ROOT_HGT = 0; // Defining as 0
	protected ArrayList<Integer> nodeCntAtDepth = new ArrayList<Integer>();


	/** Constructors:
	 * Three different ways to grow a tree:
	 * empty, through an ArrayList of Strings or
	 * directly from a file discriptor.*/
	public BinarySearchTree() { root = null; }

	public BinarySearchTree(ArrayList<String> words) {
		root = null;
		for (String s : words) { add(s); }
	}

	public BinarySearchTree(File infile) {
		root = null;
		try {
			sc = new Scanner(infile);
			while(sc.hasNextLine()) { add(sc.nextLine().trim()); }
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(0);
		} finally { sc.close(); }
	}



	/* Public APIs ------------------------------------------------------*/



	/** Add word to the tree.
	 *  @param word		String to be inserted into tree
	 *  @return void
	 *  */
	public void add(String word) {
		if (root==null) { root=new Node<String>(word); }
		else { root.add(word); }
		numberOfNodes++;
	}



	/** Prints the tree in an inordered representation. */
	public void print() { root.printNode(); }



	/** Removes given string from tree.
	 * If the given string is the root node, a new node is created
	 * and its left child is assigned the previous root and the root itself
	 * to this new node. Then, leaning on the remove implementation of
	 * nodes without two children, left and right children should be
	 * correctly assigned.
	 *  @param word 	The string that is to be deleted.
	 *  @return void */
	public void remove(String word) {
		if (root==null)
			throw new NoSuchElementException("Empty Tree");
		if (word==root.content()) {
			Node<String> tmp = new Node<String>(word);
			tmp.descendants = root.descendants;
			tmp.left = root;
			root = tmp;
		}
		root.remove(word, root);
		numberOfNodes--;
	}



	/** Search the tree for specified string.
	 *  @param word		The string that is to be searched for
	 *  @return boolean	True if found, else false. */
	public boolean search(String word) {
		lookUps.set(0);
		return root.search(word);
	}



	/** Returns number of recursive lookups of a word in the tree. */
	public int numberOfLookUps() { return lookUps.get(); }



	/** Returns the height of tree from the root node. */
	public int height() { return root.height(); }



	/** Returns the sum of the depth of all nodes in the tree
	 *  Based upon chapter 4.3.5 in the book used by this course p.140.
	 * */
	public int InternalPathLength() { return root.sumOfDepth(); }



	/** Build a string containing information about this tree.
	 *  @return String */
	public String toString() {
		int height = root.height();
		float avgCalc = (float)Math.log(numberOfNodes)
			/((float)Math.log(2));
		String statistics =
			"=================================================\n"+
			"TREE INFORMATION:\n"+
			"=================================================\n"+
			"Depth:                  "+height+"\n\n"+
			"Nodes pr depth:         "+getNodesPrDepth()+"\n\n"+
			"Average depth:          "+avgInternalPathLen()+"\t\t"+
			"log2("+numberOfNodes+") = "+avgCalc+"\n"+
			"First (Alphabetically): "+root.minVal()+"\n"+
			"Last (Alphabetically):  "+root.maxVal()+"\n"+
			"Root:                   "+root+"\n";
		return statistics;
	}



	/** Constructs a string containing depth-level and the amount
	 * of nodes at that level. */
	public String getNodesPrDepth() {
		int  treeHeight = root.height();
		String result = "(Level : Number)\n"
			+"-------------------------------------------------\n";
		int sum = 0;
		for (int lvl=ROOT_HGT; lvl < treeHeight; lvl++) {
			int numNodes = root.nodesAtDepth(lvl);//, root);
			nodeCntAtDepth.add(numNodes);
			result += "("+lvl+" : "+numNodes+") \t";
			result += (lvl+(1-ROOT_HGT))%5==0?"\n":" ";
			sum += numNodes;
		}
		result+="\n------------------------------------------------\n"+
			"SUM (all nodes at all levels): "+sum+
			"\n------------------------------------------------";
		return result;
	}




	/** Average depth calculated by "assigning" a depth-number to each node
	 * in the tree, and multiplying that number with all nodes at
	 * that depth. Summing up all of these numbers will give a weight of
	 * depths. Dividing this number by all the nodes in the tree will yield
	 * the average depth. */
	public double avgInternalPathLen() {
		int depth = 0;
		double avgDepth = 0;
		for (int numNodes:nodeCntAtDepth) {
			avgDepth+=numNodes*depth++;
		}
	       return avgDepth/numberOfNodes;
	}



	/* Private class ----------------------------------------------------*/



	/** Tree node
	 *  @author Christopher Storheim. */
	private class Node<T extends Comparable<T>> {
		private static final int LEAF_CHD = 0; // def. for Readablilty.
		private static final int ROOT = 1; // My def.& for readability.
		private T data;
		private Node<T> left;
		private Node<T> right;
		private int count;
		private int descendants;



		/** Instanciate new node with initial content count of one
		 * element and descendants=0 (This is because all new nodes
		 *  are leaf)*/
		public Node(T data) { this(data, null, null); }
		public Node(T data, Node<T> left, Node<T> right) {
			this.data = data;
			this.left = left;
			this.right = right;
			descendants = 0;
			count = 1;
		}



		/** Prints information about nodes content and how many
		 *  @return String	:info */
		public String toString() {
			return data + "\t#descendants: " + descendants
				+ " #duplicates: "+(count-1);
		}



		/** Returns a pointer to *this* nodes contet.
		 * @return T 	the string node points to. */
		public T content() { return data; }



		/** Get number of times the same word has been inserted.
		 * @return int count of duplicate keys */
		public int getCount() { return count; }



		/** Get number of descendants this node has. */
		public int getDescendants() { return descendants; }



		/** Add a key to the tree by recursively walking in the
		 * direction of the correct insertion point dictated by the
		 * rules of trichotomy. When stackframe collapses aka callee
		 * returns, all nodes in the path has its descendants
		 * incremented by one, unless it is a duplicate key.*/
		public void add(T newData) {
			if (newData.compareTo(data)<0)
				if (left!=null) { left.add(newData); }
				else { left=new Node<>(newData); }
			else if (newData.compareTo(data)>0)
				if (right!=null) { right.add(newData); }
				else { right = new Node<>(newData); }
			else { count++; }
			descendants += count==1?1:0;
		}



		/** Remove specified content from tree while keeping reference
		 * to the parent node. When pointers are done being shifted
		 * and cut. Number of descendants are decremented all the way
		 * to parent node.
		 * @param T		content to be removed
		 * @param Node<T> 	parent node to be able to re-link */
		public void remove(T content, Node<T> parent) {
			int cmp = content.compareTo(data);
			if (cmp<0) {
				if (left!=null) { left.remove(content, this); }
			} else if (cmp>0) {
				if (right!=null) {right.remove(content, this);}
			} else if (left != null && right != null) {
				data = right.minVal().data;
				right.remove(data, parent);
			} else if (parent.left == this) {
				parent.left=left==null?right:left;
			} else { parent.right=right==null?left:right; }
			descendants--;
		}



		/** Return the sum of nodes at the specific depth n.
		 * @param n	The depth to be counted.
		 * @return 	number of nodes  at that depth. */
		public int nodesAtDepth(int n) {
			if (n==0) return 1;
			return ((left!=null)?left.nodesAtDepth(n-1):0) +
				((right!=null)?right.nodesAtDepth(n-1):0);
		}



		/** Prints all nodes in inorder fashion from this node
		 * Used for testing and not included in assignement.
		 * */
		public void printNode() {
			if (left!=null) { left.printNode(); }
			System.out.println(this);
			if (right!=null) { right.printNode(); }
		}



		/** Searches all nodes from *this* node for match on
		 *  supplied string.
		 *  @param T 	The word to be searched for.
		 *  @return true/false	depending on result, true=found. */
		public boolean search(T content) {
			int cmp = content.compareTo(this.data);
			lookUps.getAndIncrement();
			if (cmp<0) {
				return left!=null?left.search(content):false;
			} else if (cmp>0) {
				return right!=null?right.search(content):false;
			} else { return true; }
		}



		/** Calculate height of tree from root node. Done by
		 * recursively *asking* each node which branch is the one with
		 * the most descendants, and passing the maximum value all the
		 * way up to root. Adding one to each max() so root is also
		 * included. LEAF_CHD indicates that leaf has zero children */
		public int height() {
			return ROOT+Math.max(left!=null?left.height():LEAF_CHD,
					right!=null?right.height():LEAF_CHD);
		}



		/** Based upon chapter 4.3.5 in the book used by this course
		 *  p.140. */
		public int sumOfDepth() {
			return (left!=null)?descendants+left.sumOfDepth()
				:(right!=null)?descendants+right.sumOfDepth()
				:0;
		}



		/** Returns the node at the leftmost distance from the root by
		 * recursing in that direction. */
		public Node<T> minVal() {
			return left!=null?left.minVal():this;
		}



		/** Returns the node at the rightmost distance from the root
		 * by recursing in that direction.*/
		public Node<T> maxVal() {
			return right!=null?right.maxVal():this;
		}
	}
}
