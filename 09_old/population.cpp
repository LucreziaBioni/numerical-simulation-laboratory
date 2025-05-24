#include <armadillo>
#include "random.h"
#include "route.h"
#include "population.h"
#include <iostream>
#include <cmath>
#include <algorithm> 


void Population :: initialize_pop(int npop, int ngen, arma::mat * dist_matrix){
    _npop=npop, _ngen=ngen;
    int p1, p2; // Read from ../INPUT/Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("Primes");
    Primes >> p1 >> p2 ;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    _rnd.SetRandom(seed,p1,p2);

    _dist_matrix = *dist_matrix; // Initialize the distance matrix

    _pop.set_size(_npop);
    for (int i = 0; i < _npop; ++i) {
        _pop[i].initialize(&_dist_matrix, _rnd);
    }

}

Route Population :: get_percorso(int i){
    return _pop(i);
}


int Population::select_index() {
    double r = _rnd.Rannyu(); 
    int j = int(_npop * std::pow(r, _selexp));
    return j;
}

void Population::sort_by_length() {
    // Copia i dati in un vector
    std::vector<Route> temp_vec(_npop);
    for (int i = 0; i < _npop; ++i){
        temp_vec[i] = _pop[i];
    }

    // Ordina il vector
    std::sort(temp_vec.begin(), temp_vec.end(), [&](const Route& a, const Route& b) {
        return a.calculate_length(&_dist_matrix) < b.calculate_length(&_dist_matrix);
    });
    
    // Copia indietro
    for (int i = 0; i < _npop; ++i){
        _pop[i] = temp_vec[i];
    }
}


Route Population::select() { 
    int j = select_index();
    return _pop(j);
}

int Population::size() const {
    return _pop.size(); // Return the size of the population
}

int Population :: pbc(int city) const{
    int ndim = 34;
    if(city >= ndim){
        return city - ndim + 1;
    }
    return city;
}


arma::field<Route> Population :: crossover(Route a, Route b){
    if (_rnd.Rannyu()<_pcross){
        arma::field<Route> figli(2);
        figli[0].initialize(&_dist_matrix, _rnd);
        figli[1].initialize(&_dist_matrix, _rnd);
        arma::Col<int> a_route = a.getroute(); // Get the route of the first parent
        arma::Col<int> b_route = b.getroute(); // Get the route of the second parent
        int pos = int(_rnd.Rannyu(0,a.getdim())); // Randomly select a crossover point
        arma::Col<int> figlio1_temp;
        arma::Col<int> figlio2_temp;
        arma::Col<int> sub1 = a_route.subvec(0,pos);
        arma::Col<int> sub2 = b_route.subvec(0,pos);
        figlio1_temp = arma::join_vert(figlio1_temp,sub1);
        figlio2_temp = arma::join_vert(figlio2_temp,sub2);
        arma::Col<int> sub3 = a_route.subvec(pbc(pos+1),a.getdim()-1); // gli indici vanno da 0 a 33
        arma::Col<int> sub4 = b_route.subvec(pbc(pos+1),b.getdim()-1);
        arma::Col<int> sub3_ord = sort_by_reference(sub3,b_route);
        arma::Col<int> sub4_ord = sort_by_reference(sub4,a_route);
        figlio1_temp = arma::join_vert(figlio1_temp,sub3_ord);
        figlio2_temp = arma::join_vert(figlio2_temp,sub4_ord);
        figli[0].setroute(figlio1_temp);
        figli[1].setroute(figlio2_temp);
        
        return figli;
    }
    else{
        return {a,b};
    }
}


arma::Col<int> Population :: sort_by_reference(arma::Col<int> a, arma::Col<int> ref){
    arma::Col<int> ref_pos(ref.size()); 
    for(int i = 0; i < ref.size(); i++){
        ref_pos[ref[i]-1] = i;
    }

    vector<int> sorted(a.begin(),a.end());
    sort(sorted.begin(), sorted.end(), [&ref_pos](int j, int k){
        return ref_pos[j] < ref_pos[k];
    });

    return arma::Col<int>(sorted); // riconverto vector in vettore di armadillo
}

void Population::evolve( const arma::mat dist_matrix) {
    ofstream coutf("results.dat");
    coutf << "#" << "\t\t L2 \t\t <L2>" << endl;
    sort_by_length();
    for (int i = 0; i < _ngen; i++){
        arma::field<Route> figli(_npop);
        for(int j = 0; j < _npop/2; j++){
            Route a = select();
            Route b = select();
            arma::field<Route> temp = crossover(a,b);
            figli[j].initialize(&_dist_matrix, _rnd);
            figli[j+_npop/2].initialize(&_dist_matrix, _rnd);
            figli[j].setroute( temp[0].getroute());
            figli[j+_npop/2].setroute( temp[1].getroute());
        }
        for(int j = 0; j < _npop; j++){
            figli[j].mutate();
        }
        _pop = figli;
        sort_by_length();
        double sum = 0;
        for (int j = 0; j < _npop/2; j++){
            sum += _pop[j].calculate_length(&dist_matrix);
        }
        coutf << i << "\t\t" << _pop[0].calculate_length(&dist_matrix) << "\t\t" << sum/(_npop/2) << endl;
    }
    coutf.close();
    ofstream out("best_route.dat");
    out << "#Best route \t \t x \t \t y" << endl;
    for (int i = 0; i < _pop[0].getdim(); i++){
        out << _pop[0].getroute()[i] << endl;
    }
    out.close();
}
