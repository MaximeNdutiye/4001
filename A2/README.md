### SYSC 4001 A2
Maxime Ndutiye 101040367

### Instructions to run
You can use make to compile the program
`make`

then `./SORT` to run the program

### Design Explanation
The program starts off by creating the sharedd memory required and the semaphore.

The program program then proceeds to `fork()` 5 times, with each child calling `sort(...)`.
The child processes get passed the shared memory as well as which part of the array they are
responsible for sorting.

Here's some pseudo code explaining how the children sort the array

```
while (array not sorted) {
	acquire lock

	if (low < hi){
		swap(low, hi)
	}

	release lock
}

exit child process
```

Each child proceeds to try and acquire a lock while the array is not yet sorted. The child
process then performs a swap if needed, then it releases the lock so other children can swap.
The process is continued untill the array is sorted.

** important design goals **

- The program is desgined to protect critical sections of code using semaphores
	- `semaphore_p` and `semaphore_v` are used to protect modifying the array
- The program is designed sort the array fully and correctly
	- each process will keep running while the array is not yet sorted
- The program is designed to wait until the array is sorted before exiting
	- the parent process waits for the children to finish with `wait(...)`
