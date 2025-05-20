
#ifndef __Route__
#define __Route__

#include <armadillo>
#include "random.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace arma;

class Route {

private:
  const int _ndim = 34; // Dimensionality of the system
  arma::Col<int> _route; // Route vector
  double _length; // Length of the route
  Random* _point_rnd;
  
public: // Function declarations
  void initialize( arma::mat * distance_matrix , Random &rnd );                      // Initialize route properties
  void setstop(int stop , int city);
  int getstop(int stop);
  void swap(int i, int j); // Swap two cities in the route
  bool check();
  int pbc(int city);
  double Route :: calculate_length( arma::mat * distance_matrix );
  void shift(); // Shift a segment of the route
  void swap_block();
  void invert_block();
};

#endif // __Route__