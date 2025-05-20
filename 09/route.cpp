
#include <iostream>
#include <math.h>
#include "route.h"

using namespace std;

void Route :: setstop(int stop , int city){
    _route(stop) = city;
    return;
}

int Route :: getstop(int stop){
    return _route(stop);
}


bool Route ::  check(){
    for(int i = 0; i < _ndim ; i++){
        for( int j = i+1; j < _ndim ; j++){
            if(_route(i) == _route(j)){
                return false;
            }
        }
    }
}

int Route :: pbc(int city){
    if(city >= _ndim){
        return city - _ndim + 1;
    }
    return city;
}

void Route :: swap(int i, int j){
    int temp = _route(i);
    _route(i) = _route(j);
    _route(j) = temp;
    return;
}

double Route :: calculate_length( arma::mat * distance_matrix ){
    double length = 0;
    for(int i=0 ; i < _ndim - 1 ; i++){
        length += distance_matrix->at(_route(i)-1, _route(pbc(i+1))-1);
    }
    length += distance_matrix->at(_route(_ndim-1)-1, _route(0)-1);
    return length;
}

void Route :: initialize(  arma::mat * distance_matrix , Random &rnd ){

    _point_rnd = &rnd;
    _route.resize(_ndim);

    for(int i=0 ; i< _ndim ; i ++){
      this->setstop(i,i+1);
    }

    int n_swaps = _ndim;
    for(int i=0 ; i < n_swaps ; i++){
        int j = (int) rnd.Rannyu(1,_ndim); // generates a random number in [1,_ndim)
        int k = (int) rnd.Rannyu(1,_ndim);
        this->swap(j,k);
    }

    _length = this->calculate_length(distance_matrix);


   return;
}


void Route :: shift( ){
    int n = _point_rnd->Rannyu(1,_ndim-1);
    arma::Col<int> temp(_ndim);
    for(int i=0 ; i< _ndim ; i++){
        temp(i) = _route(i);
    }
    for(int j = 1; j < _ndim ; j++){
        _route(pbc(j+n)) = temp(j);
    }
    if(check() == false){
        cout << "Error: Route not valid after shift" << endl;
    }
    return;
}

void Route :: swap_block(){
    int m = _point_rnd->Rannyu(1,_ndim/2 - 1 );
    int pos = _point_rnd->Rannyu(1,_ndim - 1);

    for(int i=0 ; i < m ; i++){
        int j = pbc(i+pos);
        int k = pbc(i+m+pos);
        this->swap(j,k);
    }
    if(check() == false){
        cout << "Error: Route not valid after swap_block" << endl;
    }
}

void Route :: invert_block(){
    int m = _point_rnd->Rannyu(1, _ndim - 1 );
    int pos = _point_rnd->Rannyu(1,_ndim - 1);

    arma::Col<int> temp(_ndim);
    for(int i=0 ; i< _ndim ; i++){
        temp(i) = _route(i);
    }

    for(int j = m ; j > 0 ; j--){
        int k = pbc(j+pos);
        int l = pbc(j+m+pos);
        _route(k) = temp(l);
    }
    if(check() == false){
        cout << "Error: Route not valid after invert_block" << endl;
    }
    return;

}