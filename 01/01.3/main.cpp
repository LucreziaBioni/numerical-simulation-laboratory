#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "random.h"

using namespace std;

double error_double( double AV, double AV2 , int n ){
    // funzione che mi restituisce la deviazione standard di n elementi
    if(n == 0){
        return 0;
    }
    return sqrt( (AV2 - AV*AV)/n );
    // oss: sarebbe diviso il numero di elementi - 1, ma io divido per n poiché l'indice dell'array parte da 0
}
 
int main (int argc, char *argv[]){

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

    double L{1.} , d{3.}; // L lunghezza dell'ago, d distanza tra righe
    int N_lanci{1000000};
    int N_blocchi{100};
    int n{ N_lanci / N_blocchi}; // numero di lanci per blocco
    double r{}, theta{}; // r distanza del baricentro dell'ago dalla riga, theta angolo dell'ago da linea parallela a riga
    double x{}, y{};

    double ave_blocco{}; // quando inserito nel ciclo for, contiene la media dei valori dei primi (i+1) blocchi
    double ave2_blocco{}; // quando inserito nel ciclo for, la media dei quadrati dei valori dei primi (i+1) blocchi
    double partial_sum{}; // accumula la somma delle medie dei primi (i+1) blocchi
    double partial_sum2{}; // accumula la somma delle medie dei quadrati 
    double pi_value{};
    ofstream fout ("data.dat");
    for( int i = 0; i< N_blocchi ; i++ ){
        int sum = 0; // numero di hit in un blocco
        int validi = 0; 
        for( int j=0 ; j<n ; j++ ){ // effettuo n lanci, e conto quanti hit ci sono
            x = rnd.Rannyu(-1, 1);
            y = rnd.Rannyu(-1, 1);
            r = rnd.Rannyu(0, d/2);
            if(  x*x + y*y < 1 ){ // se il punto non è all'interno del cerchio di raggio 1
                validi++; // incremento il numero di lanci validi
                if(y >= 0){
                    theta = acos( x / sqrt(x*x + y*y) );
                } else {
                    theta = 2*M_PI - acos( x / sqrt(x*x + y*y) );
                }
                if( r <= L/2 * fabs(sin(theta)) ){
                sum = sum + 1; // conto il numero di hit
                //pi_value += 2*L*(j+1) / (d*sum);
                }
            }
        }
        if (sum != 0) {
            //pi_value = pi_value / sum; // divido la somma di valori accumulati per il numero di hit
            pi_value = 2.*L*validi / (d*sum);
        } else {
            pi_value = 0; //caso in cui non ci siano hits
        }
        partial_sum += pi_value; // accumulo i valori di pi greco ottenuti
        partial_sum2 += pi_value * pi_value;
        ave_blocco = partial_sum / (i+1);
        ave2_blocco =partial_sum2 / (i+1);
        fout << (i+1)*L << "\t" << pi_value << "\t" << ave_blocco << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
        // prima colonna: numero di lanci (quelli contenuti negli i blocchi)
        // seconda colonna: valore medio dell'i-esimo blocco
        // terza colonna: valore medio delle medie dei primi i blocchi
        // quarta colonna: incertezza associata a valore medio dei primi i blocchi
    }
    fout.close();
        
 
 return 0;
}
