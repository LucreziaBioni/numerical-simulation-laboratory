#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <armadillo>
#include <algorithm>
#include <filesystem>
#include "random.h"

using namespace std;

// Prototipi delle funzioni
double error_double(double AV, double AV2 , int n); // Calcolo errore statistico
double psi(double x, double mu, double sigma);      // Funzione d'onda trial
bool metro(double x, double y, double mu, double sigma, Random& rnd); // Algoritmo di Metropolis
double Hpsi(double x, double mu, double sigma);     // Valutazione dell'hamiltoniana su psi
double set_delta(Random& rnd, double& delta, double mu, double sigma, double x0, int steps); // Tuning di delta per Metropolis

int main(int argc, char *argv[]){

    // Inizializzazione generatore di numeri casuali
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes"); // File con numeri primi per inizializzare il generatore
    if (Primes.is_open()){
        Primes >> p1 >> p2;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
 
    ifstream input("seed.in"); // File con il seme del generatore
    string property;
    if (input.is_open()){
        while (!input.eof()){
            input >> property;
            if(property == "RANDOMSEED"){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;

    // Parametri della simulazione
    double x = 0.0; // posizione iniziale
    vector<double> mu_values = {0.767369, 0.0, 1.0, 1.0}; // valori di mu da testare
    vector<double> sigma_values = {0.6295870, 0.5, 1.0, 0.5}; // valori di sigma da testare
    double delta = 0.5; // ampiezza iniziale del passo di Metropolis

    int n_steps = 10000;     // numero di passi per blocco
    int n_blocks = 100;      // numero di blocchi
    int tune_steps = 1000;   // passi per il tuning del delta

    // Ciclo su diverse configurazioni (mu, sigma)
    for (int k = 0; k < mu_values.size(); k++) {
        double mu = mu_values[k];
        double sigma = sigma_values[k];

        // Tuning di delta per ottenere accettanza ~0.5
        double tuned_acceptance = set_delta(rnd, delta, mu, sigma, x, tune_steps);

        // Apertura file di output
        ofstream coute("energy_" + to_string(k) + ".dat");
        ofstream couta("acceptance_" + to_string(k) + ".dat");
        ofstream amp("amplitude_" + to_string(k) + ".dat");
        ofstream out("output_" + to_string(k) + ".dat");

        // Header dei file
        coute << "#     BLOCK:  ACTUAL_E:     E_AVE:      ERROR:" << endl;
        couta << "# BLOCK\tACCEPTANCE\n";
        amp << "# x\t|psi(x)|^2\n";
        out << "mu = " << mu << endl;
        out << "sigma = " << sigma << endl;
        out << "delta = " << delta << endl;
        out << "tuned_acceptance = " << tuned_acceptance << endl;

        // Variabili per media e deviazione standard a blocchi
        double ave_blocco{};     // media cumulativa
        double ave2_blocco{};    // media dei quadrati cumulativa
        double partial_sum{};    // somma delle stime di energia medie
        double partial_sum2{};   // somma dei quadrati delle stime

        double n_attemps{};
        double n_accepted{};

        // Loop sui blocchi
        for(int i = 0; i < n_blocks; i++){
            double stima = 0; // stima dell’energia per il blocco
            n_attemps = 0;
            n_accepted = 0;

            // Loop sui passi all'interno del blocco
            for(int j = 0; j < n_steps; j++){
                n_attemps++;
                double y = x + rnd.Rannyu(-1.0, 1.0) * delta; // nuova posizione proposta

                if(metro(x, y, mu, sigma, rnd)){ // accettazione secondo Metropolis
                    x = y;
                    n_accepted++;
                }

                stima += Hpsi(x, mu, sigma); // accumulo Hψ/ψ
                amp << x << "\t" << pow(psi(x, mu, sigma), 2) << endl; // |ψ(x)|²
            }

            // Salvo accettanza per blocco
            couta << (i+1) << "\t" << double(n_accepted)/double(n_attemps) << endl;

            // Aggiorno medie
            partial_sum += stima / n_steps;
            partial_sum2 += pow(stima / n_steps, 2);
            ave_blocco = partial_sum / (i+1);
            ave2_blocco = partial_sum2 / (i+1);

            // Scrivo energia media e errore a blocchi
            coute << (i+1) << "\t" << stima/n_steps << "\t" << ave_blocco << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
        }

        // Chiudo i file
        coute.close();
        couta.close();      
    }

    return 0;
}

// Calcolo errore statistico sulla media di n blocchi
double error_double(double AV, double AV2 , int n){
    if(n == 0){
        return 0;
    }
    return sqrt((AV2 - AV*AV)/n); // deviazione standard della media
}

// Funzione d'onda trial ψ(x; μ, σ)
double psi(double x, double mu, double sigma){
    return exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) + exp(-(x+mu)*(x+mu)/(2*sigma*sigma));
}

// Energia Hψ/ψ per la trial wavefunction
double Hpsi(double x, double mu, double sigma){
    return -0.5 * (
           -1/(sigma*sigma) * exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) 
           -1/(sigma*sigma) * exp(-(x+mu)*(x+mu)/(2*sigma*sigma)) 
           + 1/pow(sigma,4) * (x-mu)*(x-mu) * exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) 
           + 1/pow(sigma,4) * (x+mu)*(x+mu) * exp(-(x+mu)*(x+mu)/(2*sigma*sigma))
           ) / psi(x, mu, sigma)
           + pow(x, 4) - (5.0/2.0) * pow(x, 2); // potenziale armonico anarmonica
}

// Algoritmo di Metropolis per l’accettazione del nuovo punto y
bool metro(double x, double y, double mu, double sigma, Random& rnd){
    bool decision = false;
    double acceptance = min(1.0, pow(psi(y, mu, sigma), 2) / pow(psi(x, mu, sigma), 2));
    if(rnd.Rannyu() < acceptance)
        decision = true;
    return decision;
}

// Routine per il tuning di delta: cerca un'accettanza ~0.5
double set_delta(Random& rnd, double& delta, double mu, double sigma, double x0, int steps){
    double target_acceptance = 0.5; // accettanza desiderata
    double acceptance = 0;
    double x = x0; // punto di partenza
    double tol = 0.1; // tolleranza sull'accettanza
    int max_iter = 1000; // numero massimo di iterazioni
    int iter = 0;

    while (iter < max_iter){
        int accepted = 0;
        for (int i = 0; i < steps; ++i){
            double y = x + rnd.Rannyu(-1., 1.) * delta;
            if (metro(x, y, mu, sigma, rnd)){
                x = y;
                accepted++;
            }
        }
        acceptance = static_cast<double>(accepted) / steps;
        if (fabs(acceptance - target_acceptance) < tol){
            break;
        }
        if (acceptance < target_acceptance)
            delta *= 0.9; // diminuisci passo
        else
            delta *= 1.1; // aumenta passo

        iter++;
    }
    return acceptance;
}
