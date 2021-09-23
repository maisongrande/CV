public class StopWatch {
	protected long tic, toc;
	protected Object obj;

	/** Times the excution of the object obj execute function
	 * inherite from the Timeable interface.
	 * @param obj Object which implements the Timable interface.
	 * */
	public StopWatch(Timeable obj) {
		this.obj = obj;
		tic = System.nanoTime();
		obj.execute();
		toc = System.nanoTime();
	}

	/**toString
	 * Returns string representation of result on the form:
	 * @return class@xxxxx: XXXXX ms.
	 */
	@Override
	public String toString() { return obj+": "+(toc-tic)/1000000.0+" ms."; }
}
