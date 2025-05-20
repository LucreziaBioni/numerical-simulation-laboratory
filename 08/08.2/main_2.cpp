#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <armadillo>
#include <algorithm>
#include <vector>
#include "random.h"

using namespace std;

double error_double( double AV, double AV2 , int n );
double psi(double x, double mu, double sigma);
bool metro_H(double x , double y, double mu, double sigma, Random&  rnd);
double Hpsi(double x , double mu, double sigma);
double set_delta(Random& rnd, double& delta, double mu, double sigma, double x0, int steps);
vector<double> compute_H ( Random& rnd , double mu, double sigma );
bool metro_par(Random& rnd, double sigma_old , double sigma_new , double mu_old , double mu_new , double beta );


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

    double mu = 1.0, sigma = 1.0; // parametri iniziali

    ofstream coutp("parameters.dat");
    coutp << "T  \t \t mu \t \t sigma" << endl;
    for ( double T = 2. ; T>= 0.01  ; T*=0.99){ // 
        for ( int n = 0 ; n < 100 ; n++){

            double delta = 0.5 * T; // ampiezza del passo di Metropolis (lo modifico in base alla temperatura)
            double beta = 1./T; // beta
            double mu_new = mu + rnd.Rannyu(-1, 1)*delta; // nuova proposta per mu
            double sigma_new = fabs(sigma + rnd.Rannyu(-1, 1)*delta); // nuova proposta per sigma
            if (metro_par(rnd, sigma , sigma_new , mu , mu_new , beta)){
                mu = mu_new; // accetto la nuova proposta
                sigma = sigma_new; // accetto la nuova proposta
            }

        }
        coutp << T << "\t \t" << mu << "\t \t" << sigma << endl;
    }




}

vector<double> compute_H ( Random& rnd , double mu, double sigma ){
    // Simulation parameters
    double x = 0.0; // configurazione (posizione) iniziale
    double delta = 0.5; // ampiezza del passo di Metropolis

    int n_steps = 1000; // numero di passi per blocco
    int n_blocks = 10; // numero di blocchi
    int tune_steps = 1000; // numero di passi per il tuning del delta
    double tuned_acceptance = set_delta(rnd, delta, mu, sigma, x, tune_steps);
    
    double ave_blocco{}; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
    double ave2_blocco{}; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
    double partial_sum{}; // accumula la somma delle medie dei primi (i+1) blocchi
    double partial_sum2{}; // accumula la somma delle medie dei quadrati 

    vector<double> H;

    //double n_attemps{};
    //double n_accepted{};

  for(int i=0; i <n_blocks; i++){ //loop over blocks
    double stima = 0;
    //n_attemps = 0;
    //n_accepted = 0;
    for(int j=0; j < n_steps; j++){ //loop over steps in a block
        // step Metropolis 
        //n_attemps++;
        double y = x +  rnd.Rannyu(-1.0,1.0) * delta;
        if(metro_H(x,y,mu, sigma,rnd)){
            x = y;
            //n_accepted++;
        }
        stima +=  Hpsi(x, mu, sigma);
    }
    partial_sum= partial_sum + stima/n_steps;
    partial_sum2 =partial_sum2 + stima/n_steps * stima/n_steps;
    ave_blocco = (partial_sum) / (i+1);
    ave2_blocco = ( partial_sum2 ) / (i+1);
  }
    
  H.push_back(ave_blocco);
  H.push_back(error_double(ave_blocco, ave2_blocco, n_blocks-1));

  return H;
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
    return -0.5 *(  -1/(sigma*sigma) *  exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) -1/(sigma*sigma) *  exp(-(x+mu)*(x+mu)/(2*sigma*sigma)) + 1/(pow(sigma,4)) * (x-mu)*(x-mu) * exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) + 1/(pow(sigma,4)) * (x+mu)*(x+mu) * exp(-(x+mu)*(x+mu)/(2*sigma*sigma)) )/psi(x, mu, sigma) + pow(x,4) - 5./2. * pow(x,2);
}

bool metro_H(double x , double y, double mu, double sigma, Random&  rnd){ // Metropolis algorithm
    bool decision = false;
    double acceptance = min( 1. , psi(y , mu , sigma)*psi(y , mu , sigma) / (psi(x, mu , sigma)*psi(x, mu,sigma)) ); 
    if(rnd.Rannyu() < acceptance ) decision = true; //Metropolis acceptance step
    return decision;
}

bool metro_par(Random& rnd, double sigma_old , double sigma_new , double mu_old , double mu_new , double beta ){
    bool decision = false;
    double acceptance = exp(-beta * ( compute_H(rnd , mu_new , sigma_new)[0] - compute_H(rnd , mu_old , sigma_old)[0] ) );
    if(rnd.Rannyu() < acceptance ) decision = true; //Metropolis acceptance step
    return decision;
}

double set_delta(Random& rnd, double& delta, double mu, double sigma, double x0, int steps) {
    double target_acceptance=0.5; // valore target per l'accettanza
    double acceptance = 0;
    double x = x0; // punto di partenza
    double tol = 0.01; // tolleranza per la vicinanza dell'accettanza al valore target
    int max_iter = 1000;
    int iter = 0;
    while (iter < max_iter) {
        int accepted = 0;
        for (int i = 0; i < steps; ++i) {
            double y = x + rnd.Rannyu(-1., 1.) * delta;
            if (metro_H(x, y, mu, sigma, rnd)) {
                x = y;
                accepted++;
            }
        }
        acceptance = static_cast<double>(accepted) / steps;
        if (fabs(acceptance - target_acceptance) < tol) {
            break;
        }
        if (acceptance < target_acceptance)
            delta *= 0.9; // riduci se accettanza troppo bassa
        else
            delta *= 1.1; // aumenta se troppo alta

        iter++;
    }
    return acceptance;
}

