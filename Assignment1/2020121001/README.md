# Using OpenMPI to parrelize algorithms
The following three algorithms have been parallelized using OpenMPI
* Prime Number
* Finding 3-cliques and 4-cliques
* Gaussian Elimination  
**Note:** All codes take two command line arguments, input file path and output file path respectively. 
## Prime Number
In this code:
* We take a number as input, and compute it's square root.
* Depeneding on the number of processes, we distribute the numbers from 0 to square root across processes equally.
* Each process goes ahead to check whether the inputted number is prime or not, with the range of numbers assigned to it, and if it is not prime, we set a boolean is_prime to 1. 
* In the end we use MPI_Reduce and sum all values of is_prime.
* If is_prime is greater than 1 we set the number as prime.

## Cliques
For calculating cliques we use the recursive method.
* By default we get input of all 2 cliques. 
* We distribute the 2 cliques across processes.
* Then we find a symmetric difference (XOR) between each two cliques and see if they contain 2 nodes.
* If they do, and if there exists a edge between them, we add the three nodes to 3 cliques.
* We create a 1d vector and share the data to rank 0, where we insert all the generated 3-cliques into a set, ( to eliminate duplicates).
* The code is written such that each set of 3 nodes will have the three nodes in increasing order.
* The same process is followed for 4-cliques.
> This has a much better time complexity than brute force.

## Gaussian Elimination
For gaussian elimination we first take the input of the adjoint matrix
* We then create the gaussian (i.e. upper triangular matrix)
* We try to paraalize this step by splitting all rows across processes for each parent, this ensures all rows are calculated at the same time.
* Then we re create the array and repeat the step till parent is the last row.