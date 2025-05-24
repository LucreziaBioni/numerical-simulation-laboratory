#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include "population.h"
#include "random.h"
#include "route.h"

using namespace std;

void generate_cities(int choice, int n_cities, arma::mat & dist_matrix);

int main(){

    int choice;

    cout << "Insert 0 for cities on a circumference, 1 for cities inside a square: ";
    cin >> choice;
    if (choice != 0 && choice != 1) {
        cout << "Invalid choice. Please enter 0 or 1." << endl;
        return 1; // Exit the program with an error code
    }
    
    int n_cities = 34; // Number of cities
    arma::mat dist_matrix(n_cities, n_cities); // Distance matrix
    generate_cities(choice, n_cities, dist_matrix); // Generate cities and distance matrix

    Population pop; // Create a population object
    int npop = 200; // Number of individuals in the population
    int ngen = 300; // Number of generations
    pop.initialize_pop(npop, ngen, &dist_matrix); // Initialize the population
    pop.evolve(dist_matrix); // Evolve the population


    return 0;
}

void generate_cities(int choice, int n_cities, arma::mat & dist_matrix) {
    ofstream outc("cities.dat");
    outc << "# \t \t x \t \t y" << endl;

    Random rnd_city;
    int p1, p2; 
    ifstream Primes("Primes");
    Primes >> p1 >> p2 ;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    rnd_city.SetRandom(seed,p1,p2);

    vec x(n_cities);
    vec y(n_cities);

    if (choice == 0) {
        // Generate cities on a circumference
        for (int i = 0; i < n_cities; i++) {
            double theta = rnd_city.Rannyu(0, 2 * M_PI);
            x[i] =  cos(theta);
            y[i] =  sin(theta);
            outc << i+1 << "\t\t" << x[i] << "\t\t" << y[i] << endl;
        }
    } else if (choice == 1) {
        // Generate cities inside a square
        for (int i = 0; i < n_cities; i++) {
            x[i] = rnd_city.Rannyu(-1.0, 1.0); // Random x coordinate in [-1, 1]
            y[i] = rnd_city.Rannyu(-1.0, 1.0); // Random y coordinate in [-1, 1]
            outc << i+1 << "\t\t" << x[i] << "\t\t" << y[i] << endl;
        }
    }

    // Calculate the distance matrix
    for(int i = 0; i < n_cities; i++) {
        for(int j = 0; j <  n_cities; j++) {
            dist_matrix(i,j) = pow(x(i)-x(j),2) + pow(y(i)-y(j),2); // L2 norm
            //dist_matrix(j,i) = dist_matrix(i,j); // Symmetric matrix
        }
    }
}




