
#ifndef __Population__
#define __Population__

#include <armadillo>
#include "random.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace arma;

class Population {

private:
  Random _rnd;          // Random number generator
  
public: // Function declarations
    void initialize_pop(); // Initialize population properties
};

#endif // __Route__