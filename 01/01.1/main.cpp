#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "random.h"

using namespace std;

// funzione inutile se non lavoro con array
double error( const double* AV, const double* AV2 , int n ){
    // funzione che mi restituisce la deviazione standard n-esima
    // AV2 è array che contiene valore quadratico medio e AV è array che contiene valore medio
    // n è indice dell'array che indica di che termine calcolo la deviazione standard
    if(n == 0){
        return 0;
    }
    return sqrt( (AV2[n] - AV[n]*AV[n])/n );
    // oss: sarebbe diviso il numero di elementi - 1, ma io divido per n poiché l'indice dell'array parte da 0
}

double error_double( double AV, double AV2 , int n ){
   // funzione che mi restituisce la deviazione standard di n elementi
   if(n == 0){
       return 0;
   }
   return sqrt( (AV2 - AV*AV)/n );
   // oss: sarebbe diviso il numero di elementi - 1, ma io divido per n poiché l'indice dell'array parte da 0
}

double chi_quadro( const std::vector<int>& observed , double expected ){
   // funzione che restituisce il chi quadro di M valori osservati
   double chi{};
   for( int i = 0; i < (int) observed.size() ; i++){
      chi = chi +(  ( observed[i] - expected ) * ( observed[i] - expected ) / (expected ) );
   }
   return chi;
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

   
   int M {100000}; // numero di lanci
   int N{100}; // numero di blocchi
   int L = M/N; // numero di lanci per blocco


   //double ave[N]; // array che contiene i valori medi di ciascun blocco
   //double ave2[N]; // array che contiene il quadrato del valore medio di ciascun blocco
   //ofstream fout("data.dat");
   //for( int i=0 ; i<N ; i++ ){ // ciclo su numero di blocchi
   //     double sum = 0;
   //     ave[i] = 0;
   //     ave2[i] = 0;
   //     for( int j=0 ; j<L ; j++ ){ // ciclo su tutti gli elementi del blocco
   //         sum += rnd.Rannyu();
   //     }
   //     ave[i] = sum/L; // riempio vettore con i valori medi di ciascun blocco
   //     ave2[i] = sum/L * sum/L; // riempio vettore con i quadrati dei valori medi di ciascun blocco
   //}
   //double AV[N]; // l'n-esima componente di questo vettore contiene i valori medi delle medie dei primi n blocchi
   //double AV2[N]; // l'n-esima componente di questo vettore contiene valore quadratico medio delle medie dei primi n blocchi
   //double errors[N]; 
   //for( int i=0 ; i<N ; i++ ){ // ciclo su numero di blocchi
   //     AV[i] = 0;
   //     AV2[i] = 0;
   //     for( int j=0 ; j<(i+1) ; j++ ){
   //         // riempio la componente j-esima dell'array
   //         AV[i] = AV[i] + ave[j]; // qui sto facendo la somma delle medie
   //         AV2[i] = AV2[i] + ave2[j]; // qui sto facendo la somma dei quadrati delle medie
   //     }
   //     AV[i] = AV[i] / (i+1); // divido per il numero di elementi, e ottengo effettivamente la media
   //     AV2[i] = AV2[i] / (i+1);
   //     errors[i] = error( AV , AV2 , i);
   //     fout << (i+1)* L << "\t" << (AV[i] - 0.5) << "\t" << errors[i] << endl;
   //}
   //fout.close();


   // versione più compatta punto 1
   double ave_blocco{}; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
   double ave2_blocco{}; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
   double partial_sum{}; // accumula la somma delle medie dei primi (i+1) blocchi
   double partial_sum2{}; // accumula la somma delle medie dei quadrati 
   ofstream fout_r ("data_r.dat");
   for( int i = 0; i< N ; i++ ){
      double sum = 0;
      for( int j=0 ; j<L ; j++ ){ // sommo L elementi generati casualmente tra 0 e 1 
         sum = sum + rnd.Rannyu();
      }
      partial_sum= partial_sum + sum/L;
      partial_sum2 =partial_sum2 + sum/L * sum/L;
      ave_blocco = (partial_sum) / (i+1);
      ave2_blocco = ( partial_sum2 ) / (i+1);
      fout_r << (i+1)*L << "\t" << sum/L -0.5 << "\t" << (ave_blocco - 0.5) << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
      // prima colonna: numero di lanci (quelli contenuti negli i blocchi)
      // seconda colonna: valore medio dell'i-esimo blocco
      // terza colonna: valore medio delle medie dei primi i blocchi
      // quarta colonna: incertezza associata a valore medio dei primi i blocchi
   }
   fout_r.close();

   // punto 2
   ave_blocco = 0;
   ave2_blocco = 0;
   partial_sum = 0;
   partial_sum2 = 0;
   ofstream fout_sigma ("data_sigma.dat");
   for( int i = 0; i< N ; i++ ){
      double sum = 0;
      for( int j=0 ; j<L ; j++ ){ // sommo L elementi generati casualmente tra 0 e 1 
         sum = sum + pow((rnd.Rannyu()-0.5),2);
      }
      partial_sum= partial_sum + sum/L;
      partial_sum2 =partial_sum2 + sum/L * sum/L;
      ave_blocco = (partial_sum) / (i+1);
      ave2_blocco = ( partial_sum2 ) / (i+1);
      fout_sigma << (i+1)*L << "\t" << sum/L - (float)1/12 << "\t" << (ave_blocco - (float)1/12) << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
   }
   fout_sigma.close();

   // punto 3

   M = 100;
   int n = 10000;
   ofstream fout_chi("data_chi.dat");
   for(int i=0; i < 100 ; i ++){
      // creo il vettore dei conteggi con M elementi (numero di sotto-intervalli)
      // faccio 10^4 lanci
      vector<int> conteggi(M,0);
      for(int j=0 ; j<n; j++){ 
         int index = floor(rnd.Rannyu()*M);
         // per capire il numero che ho estratto in quale sotto-intervallo rientra, e segnare il conteggio
         conteggi[index]++;
       }
     fout_chi << i+1 << "\t" << chi_quadro(conteggi, n/M ) << endl;
   }

   return 0;
}