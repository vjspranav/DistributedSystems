#include <iostream>
#include <mpi.h>
#include <math.h>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
    int rank, numprocs;
    int root = 0;
    int is_prime;
    long long int data;
    long long int num_per_proc;
    long long int sqrt_num = sqrt(data);

    if (argv[1] == NULL)
    {
        cout << "Please enter input and output file name" << endl;
        return 0;
    }

    ifstream in_file;
    ofstream out_file;
    in_file.open(argv[1]);
    if (!in_file)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    out_file.open(argv[2]);
    if (!out_file)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    in_file >> data;
    in_file.close();
    // initiate MPI
    MPI_Init(&argc, &argv);

    // get size of the current communicator
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // get current process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /*synchronize all processes*/
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Error if num_proc < 1
    if (rank == 0)
    {
        cout << "Number of Processes - " << numprocs << endl;
        cout << "Input Number - " << data << endl;
        num_per_proc = sqrt_num / numprocs;
    }
    MPI_Bcast(&num_per_proc, 1, MPI_LONG_LONG_INT, root, MPI_COMM_WORLD);
    long long int start_num = rank * num_per_proc;
    start_num = start_num == 0 ? 1 : start_num;
    long long int end_num = start_num + num_per_proc > sqrt_num ? sqrt_num : start_num + num_per_proc;
    int my_prime = 0;
    // Check if number is prime
    for (long long int i = start_num; i <= end_num; i++)
    {
        if (data % i == 0 && i != 1 && i != data)
        {
            cout << "Divisible by " << i << endl;
            my_prime = 1;
            break;
        }
    }
    MPI_Reduce(&my_prime, &is_prime, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

    if (rank == 0)
    {
        if (is_prime == 0)
        {
            cout << "Number is prime" << endl;
            out_file << "YES";
        }
        else
        {
            cout << "Number is not prime" << endl;
            out_file << "NO";
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime() - start_time;
    double maxTime;
    // get max program run time for all processes
    MPI_Reduce(&end_time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        cout << "Total time (s): " << maxTime << "\n";
    }

    // shut down MPI and close
    MPI_Finalize();
    return 0;
}