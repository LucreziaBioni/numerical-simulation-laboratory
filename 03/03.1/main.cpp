#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
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

   int M = 100000; // numero di repliche del processo stocastico
   int N = 100; // numero di blocchi
   int L = M/N; // numero di repliche in ogni blocco

   // parametri forniti
   double S0 = 100; // prezzo iniziale
   double T = 1; // tempo di consegna
   double K = 100; // prezzo di consegna
   double r = 0.1; // tasso di interesse  (risk-free)
   double sigma = 0.25; // volatilità


   // punto 1: campiono direttamente il prezzo finale dell'opzione

   double ave_blocco_call{}, ave_blocco_put{}; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
   double ave2_blocco_call{}, ave2_blocco_put{}; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
   double partial_sum_call{}, partial_sum_put{}; // accumula la somma delle medie dei primi (i+1) blocchi
   double partial_sum2_call{}, partial_sum2_put{}; // accumula la somma delle medie dei quadrati 
   ofstream fout_call_dir ("data_call_dir.dat");
   ofstream fout_put_dir ("data_put_dir.dat");
   for( int i = 0; i< N ; i++ ){ // effettuo ciclo su numero di blocchi dei processi stocastici
      double sum_call = 0, sum_put =0;
      for( int j=0 ; j<L ; j++ ){ 
         double S_T = S0 * exp( (r - 0.5 * sigma * sigma) * T + sigma * rnd.Gauss(0,1) * sqrt(T) );
         sum_call += exp(-r*T) * max(0., S_T - K); // profitto scontato per call
         sum_put += exp(-r*T) * max(0., K - S_T); // profitto scontato per put
      }
      partial_sum_call= partial_sum_call + sum_call/L;
      partial_sum_put= partial_sum_put + sum_put/L;
      partial_sum2_call =partial_sum2_call + sum_call/L * sum_call/L;
      partial_sum2_put =partial_sum2_put + sum_put/L * sum_put/L;
      ave_blocco_call = (partial_sum_call) / (i+1);
      ave_blocco_put = (partial_sum_put) / (i+1);
      ave2_blocco_call = ( partial_sum2_call ) / (i+1);
      ave2_blocco_put = ( partial_sum2_put ) / (i+1);
      fout_call_dir << (i+1) << "\t" << sum_call/L << "\t" << (ave_blocco_call) << "\t" << error_double(ave_blocco_call, ave2_blocco_call, i) << endl;
      fout_put_dir << (i+1) << "\t" << sum_put/L << "\t" << (ave_blocco_put) << "\t" << error_double(ave_blocco_put, ave2_blocco_put, i) << endl;
      // prima colonna: numero di blocchi
      // seconda colonna: valore medio dell'i-esimo blocco
      // terza colonna: valore medio delle medie dei primi i blocchi
      // quarta colonna: incertezza associata a valore medio dei primi i blocchi
   }
   fout_call_dir.close();
   fout_put_dir.close();


   // punto 2: campiono in 100 passi il prezzo finale dell'opzione

   ave_blocco_call=0, ave_blocco_put=0; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
   ave2_blocco_call=0, ave2_blocco_put=0; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
   partial_sum_call=0, partial_sum_put=0; // accumula la somma delle medie dei primi (i+1) blocchi
   partial_sum2_call=0, partial_sum2_put=0; // accumula la somma delle medie dei quadrati 
   ofstream fout_call_discr ("data_call_discr.dat");
   ofstream fout_put_discr ("data_put_discr.dat");
   for( int i = 0; i< N ; i++ ){ // effettuo ciclo su numero di blocchi dei processi stocastici
      double sum_call = 0, sum_put =0;
      for( int j=0 ; j<L ; j++ ){ // sommo L elementi generati casualmente tra 0 e 1 
         double S_T= S0;
         for( int k=0 ; k<100 ; k++ ){ // campiono con 100 passi il moto browniano geometrico
            S_T = S_T * exp( (r - 0.5 * sigma * sigma) * T/100 + sigma * rnd.Gauss(0,1) * sqrt(T/100) ); // divido T per 100 perché mi dà l'intervallino di tempo tra t_(i+1) e t_i
         }
         sum_call += exp(-r*T) * max(0., S_T - K); // profitto scontato per call
         sum_put += exp(-r*T) * max(0., K - S_T); // profitto scontato per put
      }
      partial_sum_call= partial_sum_call + sum_call/L;
      partial_sum_put= partial_sum_put + sum_put/L;
      partial_sum2_call =partial_sum2_call + sum_call/L * sum_call/L;
      partial_sum2_put =partial_sum2_put + sum_put/L * sum_put/L;
      ave_blocco_call = (partial_sum_call) / (i+1);
      ave_blocco_put = (partial_sum_put) / (i+1);
      ave2_blocco_call = ( partial_sum2_call ) / (i+1);
      ave2_blocco_put = ( partial_sum2_put ) / (i+1);
      fout_call_discr << (i+1) << "\t" << sum_call/L << "\t" << (ave_blocco_call) << "\t" << error_double(ave_blocco_call, ave2_blocco_call, i) << endl;
      fout_put_discr << (i+1) << "\t" << sum_put/L << "\t" << (ave_blocco_put) << "\t" << error_double(ave_blocco_put, ave2_blocco_put, i) << endl;
      // prima colonna: numero di blocchi
      // seconda colonna: valore medio dell'i-esimo blocco
      // terza colonna: valore medio delle medie dei primi i blocchi
      // quarta colonna: incertezza associata a valore medio dei primi i blocchi
   }
   fout_call_discr.close();
   fout_put_discr.close();

   return 0;
}

