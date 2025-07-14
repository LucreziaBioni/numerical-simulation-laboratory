#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "random.h"

using namespace std;

// Funzione che restituisce l'incertezza statistica su una media, data la media dei valori (AV),
// la media dei quadrati dei valori (AV2) e il numero di dati n (n = numero di blocchi già accumulati)
double error_double(double AV, double AV2, int n);

// Versione della funzione di errore per array (non usata in questo programma)
double error(const double* AV, const double* AV2, int n);

// Calcolo del test del chi quadro tra un vettore di frequenze osservate e una frequenza attesa costante
double chi_quadro(const std::vector<int>& observed, double expected);


int main (int argc, char *argv[]) {

   // Inizializzazione del generatore di numeri casuali
   Random rnd;
   int seed[4];
   int p1, p2;

   // Lettura dei numeri primi da file per inizializzazione del generatore
   ifstream Primes("Primes");
   if (Primes.is_open()) {
      Primes >> p1 >> p2;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   // Lettura dei semi da file per inizializzazione del generatore
   ifstream input("seed.in");
   string property;
   if (input.is_open()) {
      while (!input.eof()) {
         input >> property;
         if (property == "RANDOMSEED") {
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed, p1, p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

   // Parametri per la media a blocchi
   int M {100000}; // Numero totale di lanci
   int N {100};    // Numero di blocchi
   int L = M/N;    // Numero di lanci per blocco

   // ======================
   // PUNTO 1: Legge dei grandi numeri (stima della media di Rannyu)
   // ======================

   double ave_blocco = 0;    // Media dei blocchi fino all'i-esimo
   double ave2_blocco = 0;   // Media dei quadrati delle medie dei blocchi
   double partial_sum = 0;   // Somma delle medie dei blocchi
   double partial_sum2 = 0;  // Somma dei quadrati delle medie

   ofstream fout_r("data_r.dat"); // Output dei dati

   for (int i = 0; i < N; i++) {
      double sum = 0;
      for (int j = 0; j < L; j++) {
         sum += rnd.Rannyu(); // Somma L valori uniformi in [0,1)
      }

      double media_blocco = sum / L;       // Media del blocco corrente
      partial_sum += media_blocco;         // Accumula somma delle medie
      partial_sum2 += media_blocco * media_blocco; // Accumula somma dei quadrati

      ave_blocco = partial_sum / (i+1);          // Media cumulativa
      ave2_blocco = partial_sum2 / (i+1);        // Media cumulativa dei quadrati

      fout_r << (i+1)*L << "\t"                  // Numero di lanci totali finora
             << media_blocco - 0.5 << "\t"       // Deviazione della media del blocco da 0.5
             << ave_blocco - 0.5 << "\t"         // Deviazione della media cumulativa da 0.5
             << error_double(ave_blocco, ave2_blocco, i) << endl; // Errore statistico
   }
   fout_r.close();

   // ======================
   // PUNTO 2: Stima della varianza (Rannyu - 0.5)^2
   // ======================

   // Reset delle variabili
   ave_blocco = 0;
   ave2_blocco = 0;
   partial_sum = 0;
   partial_sum2 = 0;

   ofstream fout_sigma("data_sigma.dat");

   for (int i = 0; i < N; i++) {
      double sum = 0;
      for (int j = 0; j < L; j++) {
         sum += pow(rnd.Rannyu() - 0.5, 2); // Varianza campione: (x - μ)^2
      }

      double media_blocco = sum / L;
      partial_sum += media_blocco;
      partial_sum2 += media_blocco * media_blocco;

      ave_blocco = partial_sum / (i+1);
      ave2_blocco = partial_sum2 / (i+1);

      fout_sigma << (i+1)*L << "\t"
                 << media_blocco - 1.0/12.0 << "\t" // Deviazione dalla varianza teorica di Rannyu
                 << ave_blocco - 1.0/12.0 << "\t"
                 << error_double(ave_blocco, ave2_blocco, i) << endl;
   }
   fout_sigma.close();

   // ======================
   // PUNTO 3: Test del chi-quadro per uniformità
   // ======================

   M = 100;       // Numero di sottointervalli (bins)
   int n = 10000; // Numero di lanci per test

   ofstream fout_chi("data_chi.dat");

   for (int i = 0; i < 100; i++) { // 100 test di chi-quadro

      vector<int> conteggi(M, 0); // Vettore che tiene i conteggi per ogni sottointervallo

      for (int j = 0; j < n; j++) {
         int index = floor(rnd.Rannyu() * M); // Determina in quale bin cade il numero casuale
         conteggi[index]++;
      }

      fout_chi << i+1 << "\t" << chi_quadro(conteggi, n/M) << endl;
      // Chi quadro rispetto a distribuzione uniforme attesa (n/M conteggi per bin)
   }

   return 0;
}

// ======================
// FUNZIONI DI SUPPORTO
// ======================

// Calcolo dell'errore statistico su array (non utilizzata in questo programma)
double error(const double* AV, const double* AV2, int n) {
    if (n == 0) return 0;
    return sqrt((AV2[n] - AV[n]*AV[n]) / n);
}

// Calcolo dell'errore statistico dati media e media dei quadrati
double error_double(double AV, double AV2, int n) {
    if (n == 0) return 0;
    return sqrt((AV2 - AV*AV) / n);
}

// Calcolo del test del chi quadro
double chi_quadro(const std::vector<int>& observed, double expected) {
    double chi = 0;
    for (int i = 0; i < (int)observed.size(); i++) {
        chi += pow(observed[i] - expected, 2) / expected;
    }
    return chi;
}
