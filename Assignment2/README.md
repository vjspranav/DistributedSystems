# Using Map Reduce to Implement Algorithms
In this assignment we use map reduce to implement simple matrix multiplication and for finding components given edges in an undirected graph. Below we'll briefly discuss the approach in each problem. 

## 1. Matrix Multiplication
We divide this into three parts.
* Mapper
* Reducer
* Runner Script

### Runner Script
The runner script does the main job.  
It takes the input of matrix in given format and converts it into key value pairs.  
Ex:
```
2 2
1 2
3 4
2 2
3 4
5 6
```
This will be converted into
```
AR 2 2
A 0 0 1 
A 0 1 2
A 1 0 3 
A 1 1 4
BR 2 2
B 0 0 3
B 0 1 4
B 1 0 5
B 1 1 6
```
We Label each matrix as A and B respectively.
The AR and BR are the lines that have the dimensions of the matrix.
All other lines are of the form  
```pre i j val```  
pre - A/B | i,j - row and column | val - Value at that row and column.  

### Mapper
The mapper file takes the modified input produced by runner script and prints it as key value pairs.  

### Reducer
With the help of each line, we can figure out which matrix and which value we have, and then we reconstruct the matrix. and then proceed with the multiplication.

## 2. Finding Connected Components in a graph
We divide this into two parts.
* Mapper
* Reducer
> In this case the runner script is used just for running the program. 

### Mapper 
Mapper takes input as a file of lines where each line has two space separated integeres, which show us the nodes between which an edge exists.  
In mapper we make sure that out of the two nodes the smaller one is the first one and the bigger one second, ensuring all the edges starting with a particular node are printed in order (nature of hadoop)

### Reducer
We use an algorithm that uses properties of set, union and intersection.  
First we have an empty list of sets.  
For each set of nodes we recieve we iterate through the list of sets:
1. We take an intersection of the node with the current set, if intersection is not empty:
    * We replace the current set with union of current set and the nodes.
2. If throughout loop we don't find any set, we create a new set if two nodes and append it to list.
3. In the end we iterate through all sets and see if there is any merge possible between any combination of two. 

By the end of this we will have a list of connected components. 