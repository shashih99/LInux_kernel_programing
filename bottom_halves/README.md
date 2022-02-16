When Interrupt triggers, Interrupt Handler should be executed very quickly and it should not run for more time (it should not perform time-consuming tasks). If we have the interrupt handler which is doing more tasks then we need to divide it into two halves.

1. Top Half
2. Bottom Half
The top Half is nothing but our interrupt handler. If we want to do less work, then the top half is more than enough. No need for the bottom half in that situation. But if we have more work when interrupt hits, then we need the bottom half. The bottom half runs in the future, at a more convenient time, with all interrupts enabled. So, The job of bottom halves is to perform any interrupt-related work not performed by the interrupt handler.

There are 3 bottom half mechanisms are available in Linux:
1. Workqueue in Linux Kernel
2. Softirq – Executed in an atomic context.
3. Tasklet – Executed in an atomic context.