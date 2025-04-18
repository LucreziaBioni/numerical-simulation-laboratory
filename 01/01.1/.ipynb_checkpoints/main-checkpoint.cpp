#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

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

using namespace std;
 
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
   double ave[N]; // array che contiene i valori medi di ciascun blocco
   double ave2[N]; // array che contiene il quadrato del valore medio di ciascun blocco
   ofstream fout("data.dat");

   for( int i=0 ; i<N ; i++ ){ // ciclo su numero di blocchi
        double sum = 0;
        ave[i] = 0;
        ave2[i] = 0;
        for( int j=0 ; j<L ; j++ ){ // ciclo su tutti gli elementi del blocco
            sum += rnd.Rannyu();
        }
        ave[i] = sum/L; // riempio vettore con i valori medi di ciascun blocco
        ave2[i] = sum/L * sum/L; // riempio vettore con i quadrati dei valori medi di ciascun blocco
   }

   double AV[N]; // l'n-esima componente di questo vettore contiene i valori medi delle medie dei primi n blocchi
   double AV2[N]; // l'n-esima componente di questo vettore contiene valore quadratico medio delle medie dei primi n blocchi
   double errors[N]; 
   for( int i=0 ; i<N ; i++ ){ // ciclo su numero di blocchi
        AV[i] = 0;
        AV2[i] = 0;
        for( int j=0 ; j<(i+1) ; j++ ){
            // riempio la componente j-esima dell'array
            AV[i] = AV[i] + ave[j]; // qui sto facendo la somma delle medie
            AV2[i] = AV2[i] + ave2[j]; // qui sto facendo la somma dei quadrati delle medie
        }
        AV[i] = AV[i] / (i+1); // divido per il numero di elementi, e ottengo effettivamente la media
        AV2[i] = AV2[i] / (i+1);
        errors[i] = error( AV , AV2 , i);
        fout << i* L << "\t" << (AV[i] - 0.5) << "\t" << errors[i] << endl;
   }
   fout.close();
   return 0;
}