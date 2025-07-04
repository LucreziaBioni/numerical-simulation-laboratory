#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "random.h"

using namespace std;

double error_double( double AV, double AV2 , int n );
 
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

   // random walks 

   int N_blocchi = 100;
   int N_random_walks = 10000;
   int L = N_random_walks/N_blocchi;
   int N_passi = 100;
   vector <double> ave(N_passi, 0.0);
   vector <double> ave2(N_passi, 0.0);
   vector <double> partial_sum(N_passi, 0.0);
   vector <double> partial_sum2(N_passi, 0.0);
   ofstream fout ("RW_discreto.dat");

   // reticolo cubico discreto

   for(int i = 0 ; i < N_blocchi ; i++){ // numero di blocchi
      // all'interno di ogni blocco effettuo L random walks, e in ciascun random walk effettuo 100 passi
      // dunque ho bisogno di un vettore di 100 elementi che accumuli r^2 di ogni passo
      // poi divido per L ogni elemento di tale vettore e ne faccio la radice

      vector <double> sum_r2(N_passi, 0.0); // questo vettore contiene la media della radice quadrata di r^2 per ogni passo
      for( int j = 0; j < L ; j++ )  { // effettuo L random walks
         vector <int> posizione = {0,0,0,0,0,0}; // posizione iniziale (x+,x-,y+,y-,z+,z-)
         for( int k=0 ; k<100 ; k++  ){ // in un random walk effettuo 100 passi
            double direction = rnd.Rannyu();
            double intervalli[6] = {1.0/6, 2.0/6, 3.0/6, 4.0/6, 5.0/6, 1.0};
            int indice = 0;
            for (int s = 0; s < 6; s++) {
               if (direction < intervalli[s]) {
                  indice = s;
                  posizione[s] += 1;
                  break;
               }
            }
            sum_r2[k] += pow(posizione[0] - posizione[1],2) + pow(posizione[2] - posizione[3], 2) + pow(posizione[4] - posizione[5],2);
         }
      }
      // dopo che ho effettuato L random walks, ho un vettore di 100 elementi che contiene le somme degli L r^2 per ogni passo
      // dunque divido per L e faccio la radice quadrata di ogni elemento del vettore sum_r2, al fine di ottenere la media
      for( int k=0 ; k<N_passi ; k++  ){
         sum_r2[k] = sum_r2[k]/L;
         sum_r2[k] = sqrt(sum_r2[k]);
         // questo è l'output di ogni L-esimo blocco

         // calcolo la media e la varianza di ogni passo
         partial_sum[k] += sum_r2[k];
         partial_sum2[k] += sum_r2[k]*sum_r2[k];
         ave[k] = partial_sum[k] / (i+1);
         ave2[k] = partial_sum2[k] / (i+1);
         
      }
   }
   // stampo i valori finali del valore medio ottenuto nell'N-esimo blocco per ogni passo
   for (int i = 0; i < N_passi; i++) {
      fout << i+1 << "\t" << ave[i] << "\t" << error_double(ave[i], ave2[i], N_blocchi) << endl;
   }
   fout.close();


   // continuo
   ave.assign(N_passi, 0.0);  // Azzero tutti gli elementi impostandoli a 0.0
   ave2.assign(N_passi, 0.0);
   partial_sum.assign(N_passi, 0.0);
   partial_sum2.assign(N_passi, 0.0);
   ofstream fout_c ("RW_continuo.dat");

   for(int i = 0 ; i < N_blocchi ; i++){ // numero di blocchi
      // all'interno di ogni blocco effettuo L random walks, e in ciascun random walk effettuo 100 passi
      // dunque ho bisogno di un vettore di 100 elementi che accumuli r^2 di ogni passo
      // poi divido per L ogni elemento di tale vettore e ne faccio la radice

      vector <double> sum_r2(N_passi, 0.0); // questo vettore contiene la media della radice quadrata di r^2 per ogni passo
      for( int j = 0; j < L ; j++ )  { // effettuo L random walks
         vector <double> posizione = {0,0,0}; // posizione iniziale (x,y,z)
         for( int k=0 ; k<N_passi ; k++  ){ // in un random walk effettuo 100 passi
            double theta = rnd.Rannyu(0,M_PI);
            double phi = rnd.Rannyu(0,2*M_PI);
            posizione[0] += sin(theta) * cos(phi); // x
            posizione[1] += sin(theta) * sin(phi); // y
            posizione[2] += cos(theta); // z
            sum_r2[k] += pow(posizione[0],2) + pow(posizione[1], 2) + pow(posizione[2],2);
         }
      }
      // dopo che ho effettuato L random walks, ho un vettore di 100 elementi che contiene le somme degli L r^2 per ogni passo
      // dunque divido per L e faccio la radice quadrata di ogni elemento del vettore sum_r2, al fine di ottenere la media
      for( int k=0 ; k<N_passi ; k++  ){
         sum_r2[k] = sum_r2[k]/L;
         sum_r2[k] = sqrt(sum_r2[k]);
         // questo è l'output di ogni L-esimo blocco

         // calcolo la media e la varianza di ogni passo
         partial_sum[k] += sum_r2[k];
         partial_sum2[k] += sum_r2[k]*sum_r2[k];
         ave[k] = partial_sum[k] / (i+1);
         ave2[k] = partial_sum2[k] / (i+1);
         
      }
   }
   // stampo i valori finali del valore medio ottenuto nell'N-esimo blocco per ogni passo
   for (int i = 0; i < N_passi; i++) {
      fout_c << i+1 << "\t" << ave[i] << "\t" << error_double(ave[i], ave2[i], N_blocchi) << endl;
   }
   fout_c.close();





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