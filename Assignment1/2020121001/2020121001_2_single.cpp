#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <tuple>
#include <fstream>

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

int main(int argc, char **argv)
{
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
    // vector of sets
    // int edges[num_edges][2];
    int **edges = new int *[num_edges];
    for (int i = 0; i < num_edges; ++i)
        edges[i] = new int[2];
    int *weights = new int[num_edges];
    set<tuple<int, int, int> > cliques3_edges;
    set<tuple<int, int, int, int> > cliques4_edges;
    int three_cliques[4] = {0, 0, 0, 0};
    int four_cliques[7] = {0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < num_edges; i++)
    {
        in_file >> edges[i][0] >> edges[i][1] >> weights[i];
    }
    in_file.close();
    // Cout all edges
    vector<int> v(4);
    vector<int>::iterator it, st;
    // Cout all edges
    int num_3 = 0;
    int num_4 = 0;
    int weights3[num_edges];
    // Should happen parallel
    for (int i = 0; i < num_edges; i++)
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
    num_3 = 0;

    // Should not happen parallel
    int edges3[num_3][3];
    // Add cliques3_edges to edges3
    for (auto it = cliques3_edges.begin(); it != cliques3_edges.end(); ++it)
    {
        edges3[num_3][0] = get<0>(*it);
        edges3[num_3][1] = get<1>(*it);
        edges3[num_3][2] = get<2>(*it);
        num_3++;
    }
    // Parallely calculate all 4 cliques
    // Should happen parallel
    for (int i = 0; i < num_3; i++)
    {
        for (int j = 0; j < num_3; j++)
        {
            it = set_symmetric_difference(edges3[i], edges3[i] + 3, edges3[j], edges3[j] + 3, v.begin());
            if ((it - v.begin()) == 2)
            {
                st = v.begin();
                int edge_weight_index = hasEdge(edges, num_edges, *st, *(st + 1));
                it = set_intersection(edges3[i], edges3[i] + 3, edges3[j], edges3[j] + 3, v.begin());
                st = v.begin();
                int remove_edge_weight_index = hasEdge(edges, num_edges, *st, *(st + 1));
                if (edge_weight_index != -1)
                {
                    tuple<int, int, int, int> clique_4 = get_4cliques(edges3[i], edges3[j]);
                    if (cliques4_edges.count(clique_4) == 0)
                    {
                        // cout << "removing weight of " << *st << " " << *(st + 1) << ": " << remove_edge_weight_index << ":" << weights[remove_edge_weight_index] << endl;
                        // cout << "adding weight: " << edge_weight_index << ":" << weights[edge_weight_index] << endl;
                        // cout << "Adding clique weight: " << i << ":" << weights3[i] << endl;
                        // cout << "Adding clique weight: " << j << ":" << weights3[j] << endl;
                        cliques4_edges.insert(clique_4);
                        four_cliques[weights[edge_weight_index] + weights3[i] + weights3[j] - weights[remove_edge_weight_index]]++;
                        // cout << weights[edge_weight_index] + weights3[i] + weights3[j] - weights[remove_edge_weight_index] << endl;
                        num_4++;
                    }
                }
            }
        }
    }

    cout << "=====3-cl=====" << endl;
    for (auto x : cliques3_edges)
    {
        tuple<int, int, int> tp = x;
        cout << get<0>(tp) << ' ' << get<1>(tp) << ' ' << get<2>(tp) << '\n';
    }
    cout << "=====4-cl=====" << endl;
    for (auto x : cliques4_edges)
    {
        tuple<int, int, int, int> tp = x;
        cout << get<0>(tp) << ' ' << get<1>(tp) << ' ' << get<2>(tp) << ' ' << get<3>(tp) << '\n';
    }
    cout << endl;
    for (int i = 0; i < 4; i++)
    {
        cout << "3 " << i << " " << three_cliques[i] << endl;
        out_file << "3 " << i << " " << three_cliques[i] << endl;
    }
    for (int i = 0; i < 7; i++)
    {
        cout << "4 " << i << " " << four_cliques[i] << endl;
        out_file << "4 " << i << " " << four_cliques[i] << endl;
    }
    out_file.close();
}