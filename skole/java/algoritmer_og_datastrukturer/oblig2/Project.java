import java.util.ArrayList;
import java.util.Queue;
import java.io.File;
import java.util.Scanner;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.LinkedList;
import java.io.FileNotFoundException;
import java.util.NoSuchElementException;

/**Class Project
 * @author Christopher Storheim */
class Project {
	private int manpower;
	private Task[] taskList;
	private int numOfTask;
	private int projectSize;
	private int minProjectTime;
	private Task start;
	private Task end;


	/* CTOR: ------------------------------------------------------------*/


	/**Instanciate a new project and creating all of its tasks from
	 * supplied file.
	 * @param file the project file name.
	 * @see Task */
	public Project(String file, String manpower) {
		Scanner sc = null;
		numOfTask = 0;
		try {
			this.manpower = Integer.parseInt(manpower);
			sc = new Scanner(new File(file));
			if (sc.hasNext()) { projectSize = sc.nextInt(); }
			initTaskList();
			addTasksToProject(sc, 0);
		} catch (FileNotFoundException | NumberFormatException e) {
			e.printStackTrace();
			System.exit(0);
		} finally { sc.close(); }
		start = taskList[0];
		end = taskList[taskList.length-1];
		minProjectTime = 0;
	}


	/**PrintTasks
	 * For debugging purposes, prints all tasks in project/taskList */
	public void printTasks() {
		for(Task task : taskList) { System.out.println(task); }
	}


	/**optimalTimeSchedule
	 * Prints the path from project start to project end based on the
	 * shortest optimal time path assigned to every task during
	 * realizeable check.
	 * @see isRealizable */
	public void optimalTimeSchedule() {
		printShortestPathTo(end);
		System.out.println("\n*************************************** "
				+"Shortest possible project execution is "
				+minProjectTime+
				" ***************************************");
	}


	/**printPath
	 * Prints shortest path to the task given as argument.
	 * @param task*/
	public void printShortestPathTo(Task task) {
		minProjectTime += task.getTime();
		if (task.getPath() != null) {
			printShortestPathTo(task.getPath());
			System.out.print(" ==> ");
		}
		System.out.print(task.getName());
	}

	/**simulate
	 * Runs a time simulation of the project by populating two chaining
	 * hash tables with start and end times respectively.
	 * Iterates over all time steps from 0 through project duration and
	 * does a lookup in the table if there exists an starting or finished
	 * time. If so it is presented and current staff is updated.*/
	public void simulate() {
		Map<Integer,List<Task>> starting = new HashMap<>(projectSize);
		Map<Integer,List<Task>> ending = new HashMap<>(projectSize);

		for (Task t : taskList) {
			if (starting.containsKey(t.getEarliestStart())) {
				starting.get(t.getEarliestStart()).add(t);
			} else {
				ArrayList<Task> tmp = new ArrayList<Task>();
				tmp.add(t);
				starting.put(t.getEarliestStart(),tmp);
			}

			int endTime = t.getEarliestStart()+t.getTime();
			if (ending.containsKey(endTime)) {
				ending.get(endTime).add(t);
			} else {
				ArrayList<Task> tmp = new ArrayList<Task>();
				tmp.add(t);
				ending.put(endTime,tmp);
			}
		}
		System.out.println("------------Starting Project-----------");
		int staff = 0;
		for (int i = 0; i <= end.getLatestStart()+end.getTime();i++) {
			List<Task> st = starting.get(i);
			List<Task> et = ending.get(i);
			if (st==null && et==null) { continue; }
			System.out.println("Time: "+i);
			if (et!=null) {
				for (Task t:et) {
					System.out.println("\tFinished: "
							+t.getName());
					staff -= t.getStaff();
				}
			}
			if (st!=null) {
				for (Task t:st) {
					System.out.println("\tStarting: "
							+t.getName());
					staff += t.getStaff();
				}
			}
			System.out.println("\tCurrent staff: "+staff);
		}
	}


	/**firstTask
	 * Returns first found occurance of a task with zero indegree in
	 * the list of tasks */
	public Task firstTask() {
		for (Task t: taskList) {
			if (t.inDeg()==0) { return t; }
		}
		throw new NoSuchElementException();
	}



	/**initTask
	 * Helper method for the CTOR as object is created.
	 * Populates the taskList array with tasks. */
	private void initTaskList() {
		taskList = new Task[projectSize];
		for (int i=0; i < projectSize; i++) { taskList[i]=new Task(); }
	}



	/**addTasksToProject
	 * Creates tasks and adds them to project as long as there are tasks
	 * to be read from the supplied file.
	 * For every line in a project file a new Task object is created
	 * recursively. This is done recursively so that it is possible to
	 * create  dependencies between tasks while they are being created.
	 * @param sc a scanner object containing the project-file
	 * @param count a counter to keep track of index into task-array
	 * @see createTask Helper function for task creation
	 * @throws NumberFormatException */
	private void addTasksToProject(Scanner sc, int count) {
		String nextTask = null;
		if (!sc.hasNext()) { return; }
		else if ((nextTask = sc.nextLine().trim()).length() != 0) {
			createTask(sc, nextTask, count);
		} else { addTasksToProject(sc, count); }
	}



	/**createTask
	 * Helper method for addTaskToProject().
	 * Pulls out a task from the taskList and populates its fields.
	 * Then does so for the next task until there are no more tasks.
	 * Then it adds dependencies to all tasks if there are any
	 * @see addTaskToProject
	 * @throws NumberFormatException */
	private void createTask(Scanner sc, String nextTask, int count) {
		int i = 0;
		int depEdge = 0;
		List<Task> tmp = new LinkedList<Task>();
		Task t = taskList[numOfTask++];

		for (String s : nextTask.split("\\s+")) {
			if (i==0) { t.setID(Integer.parseInt(s)); }
			else if (i==1) { t.setName(s); }
			else if (i==2) { t.setTime(Integer.parseInt(s)); }
			else if (i==3) { t.setStaff(Integer.parseInt(s)); }
			else {
				addTasksToProject(sc, count+1);
				if ((depEdge = Integer.parseInt(s)) == 0) {
					break;
				}
				tmp.add(taskList[depEdge-1]);
			}
			i++;
		}
		taskList[count].populateEdges(tmp);
	}



	/**isRealizable
	 * Is a topological sort algorithm implemented to test if the project
	 * is Realizable. It is said to be realizable iff it has no cycles.
	 * Based on and follows the algorithm presented in the book.
	 * An extra linked list is added to the algorithm to pick up all
	 * visited nodes. which is an argument to exception if cycle is found.
	 * @throws CycleFoundException */
	public void isRealizable() {
		LinkedList<Task> queue = new LinkedList<Task>();
		LinkedList<Task> tmp = new LinkedList<Task>();
		int cnt = 0;

		for (Task t : taskList) {
			if (t.inDeg() == 0) {
				queue.addFirst(t);
				tmp.addFirst(t);
			}
		}

		while (!queue.isEmpty()) {
			Task t = queue.removeLast();
			t.setTopNum(++cnt);
			for (Task w : t.getOutEdges()) {
				// Earliest Start
				int es = t.getEarliestStart()+t.getTime();
				if (es>w.getEarliestStart()) {
					w.setEarliestStart(es);
					w.setLatestStart(es);//LCn=ECn
					w.setPath(t);
				}

				if (w.decrementInDeg()==0) {
					tmp.addFirst(w);
					queue.addFirst(w);
				}
			}
		}

		if (cnt != projectSize) { loopFound(tmp.getFirst()); }

		for (Task t : tmp) {
			t.reinitInDegCount();
			taskList[t.getTopNum()-1] = t; // Topo-order taskList.
		}
		end.assignLatestStartTimes();
	}


	/**loopFound
	 * When number of toplogicaly assigned tasks differ from the total
	 * number of tasks this method prints out the cycle and exits the
	 * program.*/
	public void loopFound(Task task) {
		System.out.print("Loop found: ");
		dfs(task);
		System.out.print(task.getName());
		for (Task t:taskList) {
			if (t.getState()==0)
				System.out.print(" ==> "+t.getName());
		}
		System.out.println();
		System.exit(0);
	}

	/**dfs
	 * An implementation of dfs based on the algorithm explained in one
	 * of the lecture notes as the method lokkelet(), but not used as
	 * intended. */
	public boolean dfs(Task task) {
		if (task.getState() == 0) {
			return false;
		} else if (task.getState() == -1) {
			task.setState(0);
			for (Task t : task.getOutEdges()) {
				if (!dfs(t)) { return false; }
			}
			task.setState(1);
		}
		return true;
	}
}
