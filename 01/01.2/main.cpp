#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "random.h"

using namespace std;

int main (int argc, char *argv[]) {

   // ======================================================
   // Inizializzazione del generatore di numeri casuali
   // ======================================================

   Random rnd;
   int seed[4];
   int p1, p2;

   // Leggo due numeri primi dal file "Primes" (usati per inizializzare il generatore)
   ifstream Primes("Primes");
   if (Primes.is_open()) {
      Primes >> p1 >> p2;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   // Leggo i semi da "seed.in"
   ifstream input("seed.in");
   string property;
   if (input.is_open()) {
      while (!input.eof()) {
         input >> property;
         if (property == "RANDOMSEED") {
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed, p1, p2); // Inizializzazione del generatore
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

   // Salva lo stato corrente del seme per eventuale ripresa della simulazione
   rnd.SaveSeed();

   // ======================================================
   // Parametri della simulazione
   // ======================================================

   // Numero di variabili sommate per ciascuna distribuzione
   vector<int> N = {1, 2, 10, 100};

   // Parametri per distribuzioni esponenziale e lorentziana
   double lambda {1}; // parametro della distribuzione esponenziale
   double mu {0};     // media della lorentziana
   double gamma {1};  // larghezza della lorentziana

   // ======================================================
   // Generazione dei dati
   // ======================================================

   for (int i = 0; i < (int) N.size(); i++) {

      // Ogni file contiene i dati relativi alla somma di N[i] variabili indipendenti
      string filename = "data_N=" + std::to_string(N[i]) + ".dat";
      ofstream fout(filename);

      // Ogni riga del file conterrà 3 colonne:
      // media di N[i] variabili uniformi, esponenziali e lorentziane
      if (fout.is_open()) {

         for (int j = 0; j < 10000; j++) { // 10^4 campioni per ogni N
            double sum_std = 0; // somma di N[i] variabili uniformi in [0,1)
            double sum_exp = 0; // somma di N[i] variabili esponenziali con parametro lambda
            double sum_lor = 0; // somma di N[i] variabili lorentziane con media mu e gamma

            for (int k = 0; k < N[i]; k++) {
               sum_std += rnd.Rannyu();
               sum_exp += rnd.Exponential(lambda);
               sum_lor += rnd.Lorentz(mu, gamma);
            }

            // Scrivo nel file le medie (somma / N[i]) per ciascuna distribuzione
            fout << sum_std / N[i] << "\t"
                 << sum_exp / N[i] << "\t"
                 << sum_lor / N[i] << endl;
         }

         fout.close();

      } else {
         // Se il file non si apre correttamente, stampo un messaggio di errore
         cerr << "Errore nell'apertura del file: " << filename << endl;
      }
   }

   return 0;
}
