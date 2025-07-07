
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
  int _ndim = 110; // Dimensionality of the system
  arma::Col<int> _route; // Route vector
  double _length; // Length of the route
  Random* _point_rnd;
  double _pmut = 0.1; // Mutation probability

public: // Function declarations
  Route& operator=(const Route& other); 
  void initialize( arma::mat * distance_matrix , Random &rnd );                      // Initialize route properties
  void setstop(int stop , int city);
  int getstop(int stop);
  bool check();
  int pbc(int city) const;
  double calculate_length( const arma::mat * distance_matrix ) const ;
  void setlength(const arma::mat * distance_matrix );
  int getdim(){ return _ndim; } // Get the dimensionality of the system
  double getlength(){ return _length; } // Get the length of the route
  arma::Col<int> getroute(){ return _route; } // Get the route vector
  void setroute(arma::Col<int> route){ _route = route; } // Set the route vector

  // mutation functions
  void swap(int i , int j); // Swap two cities in the route
  void swap();
  void shift(); // Shift a segment of the route
  void swap_block();
  void invert_block();
  void mutate();
};

#endif // __Route__