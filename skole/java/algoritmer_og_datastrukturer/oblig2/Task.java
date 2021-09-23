import java.util.List;
import java.util.LinkedList;


/**Class Task
 * @author Christopher Storheim */
class Task {
	private String name;
	private int id, staff;
	private int time, earliestStart, latestStart;
	private boolean visited;
	private List<Task> outEdges;
	private List<Task> inEdges;
	private int predecessorCount, topNum, state;
	private Task path;

	/**Instantiate a new Task */
	public Task() {
		outEdges = new LinkedList<Task>();
		inEdges = new LinkedList<Task>();
		predecessorCount = 0;
		visited = false;
		topNum = state = -1;
	}

	/**setID
	 * @param id an integer describing the ID of the task.*/
	public void setID(int id) { this.id = id; }

	/**getId
	 * @return id of the task. */
	public int getID() { return id; }

	/**setName
	 * @param name short description of the task. */
	public void setName(String name) { this.name = name; }

	/**setTime
	 * @param time expected duration of the task. */
	public void setTime(int time) { this.time = time; }

	/**setStaff
	 * @param staff expected number of workers assigned to task. */
	public void setStaff(int staff) { this.staff = staff; }


	/**setOutEdge
	 * joins a new task to the adjecency list for this task.
	 * This is the edges leading OUT of this task.
	 * @param task the task the follows from this task.
	 * */
	public void setOutEdge(Task task) { outEdges.add(task); }

	/**getChildren
	 * Returns a list of all the tasks following this task. */
	public List<Task> getOutEdges() { return outEdges; }

	/**SetVisited
	 * Sets a mark on this task */
	public void setVisited(boolean pred) { visited = pred; }

	/**getVisited
	 * returns whether or not task has been visited */
	public boolean isVisited() { return visited; }

	/**getTime
	 * @return expected duration of the task. */
	public int getTime() { return time; }

	/**Getname
	 * @return name of the task. */
	public String getName() { return name; }

	/**getStaff
	 * @return number of staff assigned to task */
	public int getStaff() { return staff; }

	/**getSlack
	 * @return the calcuated slack for this task */
	public int getSlack() { return latestStart-earliestStart; }

	/**SetEarliestStart
	 * @param earliest starting time possible for this task */
	public void setEarliestStart(int eStart) { earliestStart = eStart; }

	/**getEarliestStart
	 * @return earliest possible starting time for this task */
	public int getEarliestStart() { return earliestStart; }

	/**getLatestStart
	 * @return latest start time possible for this task */
	public int getLatestStart() { return latestStart; }

	/** SetLatestStart
	 * @param lstart the Latest start time for this task */
	public void setLatestStart(int lstart) { latestStart = lstart; }

	/**getPath
	 * @return next Task in the critical path. */
	public Task getPath() { return path; }

	/**setPath
	 * @param Task the next task in the critical path */
	public void setPath(Task p) { path = p; }

	/**getTopNum
	 * @return the topological number of this task in a project */
	public int getTopNum() { return topNum; }

	/**setTopNum
	 * @param topological number of this task */
	public void setTopNum(int num) { topNum = num; }

	/**getState
	 * @return the current state, visited, ongoing or done in dfs */
	public int getState() { return state; }

	/**setState
	 * @param state the current state of the task in dfs */
	public void setState(int state) { this.state = state; }

	/**reinitInDegCount
	 * sets the indegree count to number of preceeding nodes. */
	public void reinitInDegCount() { predecessorCount = inEdges.size(); }


	/**populateEdges
	 * Based upon the tasks going in to this task, all of those tasks
	 * are set to point to this task as their successor task.
	 * At the same time number of predecessors are added to this task
	 * @param list of predecessing task. */
	public void populateEdges(List<Task> incomingTasks) {
		inEdges = incomingTasks;
		for (Task t : incomingTasks) {
			++predecessorCount;
			t.setOutEdge(this);
		}
	}

	/**Indeg
	 * Return the number of incoming edges to this task, aka tasks which
	 * this task is dependent upon.
	 * @return the indegree og the task. */
	public int inDeg() { return predecessorCount; }



	/**decrementInDeg
	 * A reference count of number of edges leading in to this task
	 * Intended used in topological search.
	 * @return predecessorCount*/
	public int decrementInDeg() { return --predecessorCount; }


	@Override
	/**toString
	 * @return a string representation of the task.
	 * @see nameAndID method */
	public String toString() {
		String str = String.format("Task id: %-3d | Name: %-35s |"
				+" Time: %-4d | Staff: %-3d | ES: %-3d |"
			       +" LS: %-3d | Slack: %-3d | Toponum %-3d"
			       +"| Successors: %s",
				id, name,time, staff, earliestStart,
				latestStart, getSlack(), topNum, successors());
		return str;
	}

	/**succesors
	 * returns a format string containing list of successors.*/
	private String successors() {
		String retval = "[ ";
		for (Task t : outEdges) {
			retval += t.getID();
			retval += " ";
		}
		retval += "]";
		return retval;
	}
	/**assignLatestStartTimes */
	public void assignLatestStartTimes() {
		for (Task t : inEdges) {
			int tmp_ls = this.earliestStart - t.time;
			if (t.latestStart < tmp_ls) { t.latestStart = tmp_ls; }
			else { t.latestStart = t.earliestStart; }
			t.assignLatestStartTimes();
		}
	}
}
