/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <armadillo>
#include <algorithm>
#include "random.h"

using namespace std;

double error_double( double AV, double AV2 , int n );
bool metro();
double psi(double x, double mu, double sigma);
bool metro(double x , double y, double mu, double sigma, Random&  rnd);
double Hpsi(double x , double mu, double sigma);


int main (int argc, char *argv[]){

    // random number generator
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
       Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
 
    ifstream input("seed.in");
    string property;
    if (input.is_open()){
       while ( !input.eof() ){
          input >> property;
          if( property == "RANDOMSEED" ){
             input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
             rnd.SetRandom(seed,p1,p2);
          }
       }
       input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;


    ofstream couta("acceptance.dat"); // Set the heading line in file ../OUTPUT/acceptance.dat
    couta << "#   N_BLOCK:  ACCEPTANCE:" << endl;

    // Simulation parameters
    int n_steps = 10000; // numero di passi per blocco
    int n_blocks = 20; // numero di blocchi
    double delta = 0.01; 
    
    double x = 0.0; // configurazione (posizione) iniziale
    double mu = 0.0; // media della gaussiana
    double sigma = 1.0; // deviazione standard della gaussiana

    // creo file per stampare la proprietà che mi interessa
    ofstream coute("energy.dat");
    coute << "#     BLOCK:  ACTUAL_E:     E_AVE:      ERROR:" << endl;


    // creo double per memorizzare i risultati
    double ave_blocco{}; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
    double ave2_blocco{}; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
    double partial_sum{}; // accumula la somma delle medie dei primi (i+1) blocchi
    double partial_sum2{}; // accumula la somma delle medie dei quadrati 

    double n_attemps{};
    double n_accepted{};

  for(int i=0; i <n_blocks; i++){ //loop over blocks
    double stima = 0;
    n_attemps = 0;
    n_accepted = 0;
    for(int j=0; j < n_steps; j++){ //loop over steps in a block
        // step Metropolis 
        n_attemps++;
        double y = x +  rnd.Rannyu(-1.0,1.0) * delta;
        if(metro(x,y,mu, sigma,rnd)){
            x = y;
            n_accepted++;
        }
        stima +=  Hpsi(x, mu, sigma);
    }
    couta << (i+1) << "\t" << double(n_accepted)/double(n_attemps) << endl;
    cout << n_accepted << "\t" << n_attemps << endl; 
    partial_sum= partial_sum + stima/n_steps;
    partial_sum2 =partial_sum2 + stima/n_steps * stima/n_steps;
    ave_blocco = (partial_sum) / (i+1);
    ave2_blocco = ( partial_sum2 ) / (i+1);
    coute << (i+1) << "\t" << stima/n_steps  << "\t" << (ave_blocco ) << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
  }
    coute.close();
    couta.close();

    // Debug: esplora psi e Hpsi in un range di x
ofstream testout("test_psi_Hpsi.dat");
testout << "# x\tpsi(x)\tHpsi(x)\n";
double mu_test = 1.0;      // prova con mu diverso da 0
double sigma_test = 0.75;  // valore realistico

for (double x_test = -5.0; x_test <= 5.0; x_test += 0.1) {
    double psi_val = psi(x_test, mu_test, sigma_test);
    double H_val = Hpsi(x_test, mu_test, sigma_test);
    testout << x_test << "\t" << psi_val << "\t" << H_val << "\n";
}
testout.close();


  return 0;
}



double error_double( double AV, double AV2 , int n ){
    // funzione che mi restituisce la deviazione standard di n elementi
    if(n == 0){
        return 0;
    }
    return sqrt( (AV2 - AV*AV)/n );
    // oss: sarebbe diviso il numero di elementi - 1, ma io divido per n poiché l'indice dell'array parte da 0
}

double psi(double x, double mu, double sigma){
    return exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) + exp(-(x+mu)*(x+mu)/(2*sigma*sigma));
}

double Hpsi(double x , double mu, double sigma){
    if (psi(x,mu,sigma) < 1e-10) return 1e6; // evita NaN
    return -0.5 *(  -1/(sigma*sigma) *  exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) -1/(sigma*sigma) *  exp(-(x+mu)*(x+mu)/(2*sigma*sigma)) + 1/(pow(sigma,4)) * (x-mu)*(x-mu) * exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) + 1/(pow(sigma,4)) * (x+mu)*(x+mu) * exp(-(x+mu)*(x+mu)/(2*sigma*sigma)) )/psi(x, mu, sigma) + pow(x,4) - 5./2. * pow(x,2);
}

bool metro(double x , double y, double mu, double sigma, Random&  rnd){ // Metropolis algorithm
    bool decision = false;
    double acceptance = min( 1. , psi(x , mu , sigma)*psi(x , mu , sigma) / (psi(y, mu , sigma)*psi(y, mu,sigma)) );
    if(rnd.Rannyu() < acceptance ) decision = true; //Metropolis acceptance step
    return decision;
}


/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
