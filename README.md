# Parallel-Bubblesort-using-multiple-processes and threads
Implemented a parallel version of the even-odd bubble sort using multiple processes.

Besides N, the parallel bubble sort should take an additional argument P, representing the number of concurrent worker processes. For example, for N=100 and P=5,
$ multi_process_bubble 100 5 

The initial (parent) process creates a shared memory segment in which it stores an array of N random integers.

Next, the parent process forks P worker processes. After fork, each worker process automatically inherits the attached shared memory segment that was created by the parent. Hence there is no need for child processes to call the shmat() system call.

Each worker process should execute the parallel bubble sort operation on (about equal sized) overlapping segments of the array, as explained in the slides.\

Each worker should use a barrier (busy looping for now) to coordinate with ''neighboring'' workers at the end of each pass.

You can also store any additional data in the shared memory that's needed for inter-worker synchronization.

The parent prints out the fully sorted array once all worker processes have finished.

