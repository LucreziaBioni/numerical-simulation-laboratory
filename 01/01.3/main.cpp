#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "random.h"

using namespace std;

// Funzione per calcolare l'incertezza statistica (errore standard)
double error_double(double AV, double AV2, int n){
    if(n == 0){
        return 0;
    }
    return sqrt((AV2 - AV*AV)/n);
    // Nota: il divisore è n, e non n-1, perché si accumulano n blocchi numerati da 0
}

int main (int argc, char *argv[]){

    // Inizializzazione del generatore di numeri casuali
    Random rnd;
    int seed[4];
    int p1, p2;

    // Lettura dei numeri primi da file
    ifstream Primes("Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    // Lettura del seme da file
    ifstream input("seed.in");
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
    double L{1.};      // Lunghezza dell'ago
    double d{3.};      // Distanza tra le righe parallele
    int N_lanci{1000000}; // Numero totale di lanci
    int N_blocchi{100};   // Numero di blocchi
    int n{ N_lanci / N_blocchi }; // Lanci per ogni blocco

    // Variabili temporanee per ogni lancio
    double r{}, theta{}, x{}, y{};

    // Variabili per la stima e l'analisi statistica di π
    double ave_blocco{}, ave2_blocco{};
    double partial_sum{}, partial_sum2{};
    double pi_value{};

    ofstream fout("data.dat");

    // Inizio del blocco principale di simulazione
    for(int i = 0; i < N_blocchi; i++){
        int sum = 0;       // Conta gli hit (intersezioni) nel blocco
        int validi = 0;    // Conta i lanci validi nel blocco

        for(int j = 0; j < n; j++){ // Lanci nel blocco
            x = rnd.Rannyu(-1, 1); // Generazione punto casuale nel quadrato [-1,1]^2
            y = rnd.Rannyu(-1, 1);
            r = rnd.Rannyu(0, d/2); // Distanza del baricentro dell'ago dalla linea più vicina

            if(x*x + y*y < 1){ // Punto interno al cerchio di raggio 1 => valido
                validi++; // Conteggio dei lanci validi

                // Calcolo dell'angolo theta rispetto all'asse orizzontale
                if(y >= 0){
                    theta = acos(x / sqrt(x*x + y*y));
                } else {
                    theta = 2*M_PI - acos(x / sqrt(x*x + y*y));
                }

                // Condizione di intersezione con una riga: r ≤ (L/2)·|sinθ|
                if(r <= (L/2) * fabs(sin(theta))){
                    sum++;
                }
            }
        }

        // Stima di π per questo blocco (solo se ci sono hit)
        if(sum != 0){
            pi_value = 2. * L * validi / (d * sum);
        } else {
            pi_value = 0;
        }

        // Statistiche progressive sui blocchi
        partial_sum += pi_value;
        partial_sum2 += pi_value * pi_value;
        ave_blocco = partial_sum / (i+1);
        ave2_blocco = partial_sum2 / (i+1);

        // Scrivo i dati nel file: numero di lanci finora, stima corrente, media progressiva, errore statistico
        fout << (i+1)*n << "\t"              // Numero di lanci totali finora
             << pi_value << "\t"             // Stima di π nel blocco i
             << ave_blocco << "\t"           // Media cumulata fino al blocco i
             << error_double(ave_blocco, ave2_blocco, i) // Errore sulla media
             << endl;
    }

    fout.close();
    return 0;
}
