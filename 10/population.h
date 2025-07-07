#ifndef __Population__
#define __Population__

#include <armadillo>
#include "random.h"
#include "route.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;
using namespace arma;

class Population {

    private:
    Random _rnd;
    int _npop;
    const double _selexp = 2.0;
    arma::field <Route> _pop;
    arma::mat _dist_matrix;
    const double _pcross = 0.90;
    int _ngen;
    int _n_migranti = 30; // Number of migrants
    int N_migr = 50; // Number of generations after which migration occurs

    public:
    void initialize_pop(int npop, int ngen, arma::mat * dist_matrix);
    Route get_percorso(int i);
    int size() const;
    void sort_by_length();
    int select_index();
    Route select();
    arma::field<Route> crossover(Route a, Route b);
    arma::Col<int> sort_by_reference(arma::Col<int> a, arma::Col<int> ref);
    void evolve( const arma::mat dist_matrix, int rank , int size); 
    int pbc(int city) const;
};

#endif