#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <tuple>
#include <fstream>
#include <mpi.h>

using namespace std;

int hasEdge(int **arr, int num_edges, int x, int y)
{
    for (int i = 0; i < num_edges; i++)
    {
        if ((arr[i][0] == x && arr[i][1] == y) || (arr[i][0] == y && arr[i][1] == x))
            return i;
    }
    return -1;
}

tuple<int, int, int> get_3cliques(int edge1[], int edge2[])
{
    int x, y, z;
    x = edge1[0];
    y = edge1[1];
    for (int i = 0; i < 2; i++)
    {
        z = edge2[i];
        if (x != z && y != z)
            break;
    }
    if (x > y && x > z)
        if (y > z)
            return make_tuple(z, y, x);
        else
            return make_tuple(y, z, x);
    else if (y > x && y > z)
        if (x > z)
            return make_tuple(z, x, y);
        else
            return make_tuple(x, z, y);
    else if (x > y)
        return make_tuple(y, x, z);
    else
        return make_tuple(x, y, z);
}

tuple<int, int, int, int> get_4cliques(int edge1[], int edge2[])
{
    int x, y, z, k;
    x = edge1[0];
    y = edge1[1];
    z = edge1[2];
    for (int i = 0; i < 3; i++)
    {
        k = edge2[i];
        if (x != k && y != k && z != k)
            break;
    }
    if (k < x)
        return make_tuple(k, x, y, z);
    else if (k < y)
        return make_tuple(x, k, y, z);
    else if (k < z) return make_tuple(x, y, k, z);
    else return make_tuple(x, y, z, k);
}

int main( int argc, char **argv ) {
    int rank, numprocs, chunk_size;
 
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
    int num_verices, num_edges;
    in_file >> num_verices >> num_edges;

    int **edges = (int **) malloc(num_edges * sizeof(int *));
    for(int i = 0; i < num_edges; i++)
        edges[i] = (int *) malloc(3 * sizeof(int));
    
    vector <int> edges3;
    set<tuple<int, int, int> > cliques3;

    // new int *[num_edges];
    // for (int i = 0; i < num_edges; ++i)
    //     edges[i] = new int[2];
     
    // Edges 3 _ P
    // int **edges3 =  (int **) malloc(num_edges * sizeof(int *));
    // for (int i = 0; i < num_edges; ++i)
    //     edges3[i] = (int *) malloc(3 * sizeof(int));
    // new int *[100];
    // for(int i=0; i< num_edges; i++){
    //     edges3[i] = new int [3];
    // }

    // int **edges3_p = new int *[100];
    // for(int i=0; i< num_edges; i++){
    //     edges3_p[i] = new int [3];
    // }

    int *weights = new int[num_edges];

    int three_cliques[4] = {0, 0, 0, 0};
    int four_cliques[7] = {0, 0, 0, 0, 0, 0, 0};

    // For reduction
    int total_num_3  = 0;
    for (int i = 0; i < num_edges; i++)
    {
        in_file >> edges[i][0] >> edges[i][1] >> weights[i];
    }
    in_file.close();

    // initiate MPI
    MPI_Init( &argc, &argv );

    // get size of the current communicator
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    // get current process rank
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    vector <int> edges3_p;
    if(rank==0){
        chunk_size = num_edges/numprocs > 4 ? num_edges/numprocs : 4;
    }
 
  /*synchronize all processes*/
  MPI_Barrier( MPI_COMM_WORLD );
  double start_time = MPI_Wtime();
  int num_3 = 0;

  MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if((rank*chunk_size <= num_edges)){
        cout << "rank: " << rank << " chunk: " << chunk_size << endl;
        set<tuple<int, int, int> > cliques3_edges;
        vector<int> v(4);
        vector<int>::iterator it, st;
        int weights3[num_edges];
        int start = rank * chunk_size - 1;
        start = start < 0 ? 0 : start;
        chunk_size = (rank + 1) * chunk_size > num_edges ? ((rank + 1) * chunk_size) - num_edges - 2: chunk_size;
        int end = start + chunk_size;
        cout << "rank: " << rank << " start: " << start << " end: " << end << endl;
        for (int i = start; i < end; i++)
        {
            for (int j = 0; j < num_edges; j++)
            {
                it = set_symmetric_difference(edges[i], edges[i] + 2, edges[j], edges[j] + 2, v.begin());
                if ((it - v.begin()) == 2)
                {
                    st = v.begin();
                    int edge_weight_index = hasEdge(edges, num_edges, *st, *(st + 1));
                    if (edge_weight_index != -1)
                    {
                        tuple<int, int, int> clique_3 = get_3cliques(edges[i], edges[j]);
                        if (cliques3_edges.count(clique_3) == 0)
                        {
                            cliques3_edges.insert(clique_3);
                            three_cliques[weights[edge_weight_index] + weights[i] + weights[j]]++;
                            weights3[num_3] = weights[edge_weight_index] + weights[i] + weights[j];
                            num_3++;
                        }
                    }
                }
            }
        }
        cout << "Process: " << rank << " has " << num_3 << " 3-cliques" << endl;
        for (auto x : cliques3_edges)
        {
            tuple<int, int, int> tp = x;
            edges3_p.push_back(get<0>(tp));
            edges3_p.push_back(get<1>(tp));
            edges3_p.push_back(get<2>(tp));
            // cout << get<0>(tp) << ' ' << get<1>(tp) << ' ' << get<2>(tp) << '\n';
        }
        // num_3 = 0;

        // Add cliques3_edges to edges3
        // for (auto it = cliques3_edges.begin(); it != cliques3_edges.end(); ++it)
        // {
        //     edges3_p.push_back(make_tuple(get<0>(*it), get<1>(*it), get<2>(*it)));
        //     // edges3_p[num_3][0] = get<0>(*it);
        //     // edges3_p[num_3][1] = get<1>(*it);
        //     // edges3_p[num_3][2] = get<2>(*it);
        //     num_3++;
        // }
        // Print from edges3_p
        // cout << "Vectors: " << endl;
        // for (int i = 0; i < num_3*3; i+=3)
        // {
        //     cout << edges3_p[i] << ' ' << edges3_p[i+1] << ' ' << edges3_p[i+2] << '\n';
        // }
    }else{
        num_3 = 0;
        cout << "Not enough edges for process: " << rank << endl;
    }
    // TODO: Get num by reducing num_3
    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Reduce( &num_3, &total_num_3, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
    // if(rank==0){
    //     // edges3.resize(total_num_3);
    //     // cout << "Total number of 3-cliques: " << total_num_3 << endl;
    //     // cout << "Gathering chunk size: " << chunk_size << endl;
    //     // edges3.resize(total_num_3 * 3);
    //     // MPI Gather edges3_p vector to edges3
    // }
    int *counts = new int[numprocs];
    MPI_Gather(&num_3, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank==0){
        for(int i=0;i<numprocs;i++){
            counts[i] = counts[i] * 3;
            cout << "Process: " << i << " has " << counts[i] << " counts" << endl;
        }
    }
    int *disps = new int[numprocs];
    // Displacement for the first chunk of data - 0
    if(rank==0){
        for (int i = 0; i < numprocs; i++)
            disps[i] = (i > 0) ? (disps[i-1] + counts[i-1]) : 0;
        for(int i=0;i<numprocs;i++){
            cout << "Process: " << i << " has " << disps[i] << " start point" << endl;
        }
        edges3 = vector<int>(disps[numprocs - 1] + counts[numprocs - 1]);
    }
    MPI_Gatherv(&edges3_p.front(), edges3_p.size(), MPI_INT, &edges3.front(), counts, disps, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank==0){
        cout << "Here: " << edges3.size() << endl;
        for (int i = 0; i < total_num_3*3; i+=3)
        {
            // cout << edges3[i] << ' ' << edges3[i+1] << ' ' << edges3[i+2] << '\n';
            cliques3.insert(make_tuple(edges3[i], edges3[i+1], edges3[i+2]));
        }
        cout << "Total number of 3-cliques: " << cliques3.size() << endl;
        for (auto x : cliques3)
        {
            cout << get<0>(x) << ' ' << get<1>(x) << ' ' << get<2>(x) << '\n';
        }
    }
    // MPI_Gather(edges3_p.data(), chunk_size, MPI_INT, edges3.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Barrier( MPI_COMM_WORLD );
    double end_time = MPI_Wtime() - start_time;
    double maxTime;
    // get max program run time for all processes
    MPI_Reduce( &end_time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        cout<<"Total time (s): "<<maxTime<<"\n";
    }

    // shut down MPI and close
    MPI_Finalize();
    return 0;
}