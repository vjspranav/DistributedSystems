#include <iostream>
#include <mpi.h>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    int rank, numprocs, idx;
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

    int n;

    in_file >> n;

    vector<double> av;
    double **a = new double *[n];
    for (int i = 0; i < n; i++)
    {
        a[i] = new double[n + 1];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            in_file >> a[i][j];
    in_file.close();
    // for (int m = 1; m < n-1; m++)
    // {
    //     if (a[0][0] == 0)
    //     {
    //         cout << "Replacing row " << 0 << " with row " << m << endl;
    //         for (int k = 0; k < n + 1; k++)
    //         {
    //             double temp = a[0][k];
    //             a[0][k] = a[m][k];
    //             a[m][k] = temp;
    //         }
    //         break;
    //     }
    // }
    double *x = new double[n];
    // initiate MPI
    MPI_Init(&argc, &argv);
    // double **a_local = new double *[n];
    // for (int i = 0; i < n; i++)
    // {
    //     a_local[i] = new double[n + 1];
    // }
    vector<double> a_local;
    // get size of the current communicator
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // get current process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /*synchronize all processes*/
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    int tnum = 0;
    int *counts = new int[numprocs];
    int *disps = new int[numprocs];
    if (rank == 0)
    {
        cout << "Total number of processes: " << numprocs << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n + 1; j++)
            {
                cout << a[i][j] << " ";
            }
            cout << endl;
        }
    }
    for (int i = 0; i < n - 1; i++)
    {
        // for (int l = i; l < n; l++)
        // {
        if (a[i][i] == 0)
        {
            for (int m = i + 1; m < n; m++)
            {
                if(a[m][i] != 0){
                    cout << "Replacing row " << i << " with row " << m << endl;
                    for (int k = 0; k < n + 1; k++)
                    {
                        double temp = a[i][k];
                        a[i][k] = a[m][k];
                        a[m][k] = temp;
                    }
                }
                break;
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        // }
        if (a[i][i] == 0)
        {
            // cout << "Divide by 0 error.\n";
            // MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            continue;
        }
        for (int j = i + 1; j < n; j++)
        {
            if ((j-1) % numprocs == rank)
            {
                double t = a[j][i] / a[i][i];
                for (int k = 0; k < n + 1; k++)
                    a_local.push_back(a[j][k] - (t * a[i][k]));
                tnum++;
            }
        }
        // Cout ector data 4 at a time
        cout << "Vector: " << tnum << endl;
        for (int j = 0; j < tnum * 4; j++)
        {
            cout << a_local[j] << " ";
        }
        cout << endl;
        // Wait for iteration to be over
        MPI_Barrier(MPI_COMM_WORLD);
        // Get counts and displacements
        MPI_Gather(&tnum, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int total_tnum = 0;
        MPI_Reduce(&tnum, &total_tnum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        // Cout the counts
        if (rank == 0)
        {
            // All counts
            for (int i = 0; i < numprocs; i++)
            {
                cout << "Counts: " << i << ":" << counts[i];
            }
            cout << endl;
            for (int j = 0; j < numprocs; j++)
            {
                counts[j] = counts[j] * (n + 1);
            }
            disps[0] = 0;
            for (int j = i; j < numprocs; j++)
            {
                disps[j - i + 1] = disps[j - i] + counts[j - i];
            }
            // for(int j=n;j<numprocs;j++){
            //     disps[j] = disps[j-1];
            // }
            cout << "Total number of elements: " << total_tnum << endl;
            for (int j = 0; j < numprocs; j++)
            {
                cout << "Process " << j << " has " << counts[j] << " elements, "
                     << "Displacement is " << disps[j] << endl;
            }
            av = vector<double>(disps[numprocs - 1] + (counts[numprocs - 1]));
        }
        MPI_Gatherv(a_local.data(), a_local.size(), MPI_DOUBLE, av.data(), counts, disps, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (rank == 0)
        {
            total_tnum = av.size();
        }
        MPI_Bcast(&total_tnum, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // cout << "rank " << rank << " total_tnum " << total_tnum << endl;
        av.resize(total_tnum);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(av.data(), total_tnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // cout << "My Rank: " << rank << " " << av.size() << endl;
        // for (int j = 0; j < av.size(); j++)
        // {
        //     cout << av[j] << " ";
        // }
        // cout << endl;
        MPI_Barrier(MPI_COMM_WORLD);

        // Cout a
        // for (int j = i + 1; j < n; j++)
        // {
        //     for (int k = 0; k < n + 1; k++)
        //     {
        //         cout << a[j][k] << " ";
        //     }
        //     cout << endl;
        // }
        // Cout av
        // for (int j = 0; j < av.size(); j++)
        // {
        //     cout << av[j] << " "
        //          << "Rank " << rank << endl;
        // }
        MPI_Barrier(MPI_COMM_WORLD);
        tnum = 0;
        for (int j = i + 1; j < n; j++)
        {
            for (int k = 0; k < n + 1; k++)
            {
                a[j][k] = av[tnum++];
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n + 1; k++)
                cout << a[j][k] << " ";
            cout << endl;
        }
        // Reset a_local and av
        a_local.clear();
        av.clear();
        // Make all displacements 0
        
        tnum = 0;
        // Cout i and n each process
        // MPI_Barrier(MPI_COMM_WORLD);
            // if (rank == 0)
        // {
        //     cout << "=========" << endl;
        // }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        cout << "Caluculating solution" << endl;
        x[n - 1] = a[n - 1][n] / a[n - 1][n - 1];
        for (int i = n - 2; i >= 0; i--)
        {
            x[i] = a[i][n];
            for (int j = i + 1; j < n; j++)
            {
                x[i] -= a[i][j] * x[j];
            }
            x[i] /= a[i][i];
        }
        for (int i = 0; i < n; i++)
        {
            cout << x[i] << " ";
            // If -0 print 0
            if (x[i] == -0)
                out_file << 0 << " ";
            else
                out_file << x[i] << " ";
        }
        cout << endl;
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
    out_file.close();
    return 0;
}