#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "random.h"

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

   // random walks 

   int N_blocchi = 100;
   int N_random_walks = 10000;
   int L = N_random_walks/N_blocchi;
   int N_passi = 100;
   <vector> double ave(N, 0.0);
   <vector> double ave2(N, 0.0);
   ofstream fout ("data.dat");

   for(int i = 0 ; i < N_blocchi ; i++){ // numero di blocchi
      // all'interno di ogni blocco effettuo L random walks, e in ciascun random walk effettuo 100 passi
      // dunque ho bisogno di un vettore di 100 elementi che accumuli r^2 di ogni passo
      // poi divido per L ogni elemento di tale vettore e ne faccio la radice

      <vector> double sqrt_r2(N, 0.0); // questo vettore contiene la media della radice quadrata di r^2 per ogni passo
      for( int j = 0; j < L ; j++ )  { // effettuo L random walks
         <vector>int posizione = {0,0,0,0,0,0}; // posizione iniziale (x+,x-,y+,y-,z+,z-)
         for( int k=0 ; k<100 ; k++  ){ // in un random walk effettuo 100 passi
            double direction = rnd.Rannyu();
            double intervalli[6] = {1.0/6, 2.0/6, 3.0/6, 4.0/6, 5.0/6, 1.0};
            int indice = 0;
            for (int s = 0; s < 6; s++) {
               if (direction < intervalli[s]) {
                  indice = s;
                  posizione.[s] += 1;
                  break;
               }
            }
            sqrt_r2[k] = pow((posizione[0] - posizione[1])^2 + (posizione[2] - posizione[3])^2 + (posizione[4] - posizione[5]),2);
         
         }
      }
   }
      // dopo che ho accumulato tutti i valori di r^2 per ogni passo, ne faccio la media e ne calcolo la radice
      for( int k=0 ; k<100 ; k++  ){
         sqrt_r2[k] = sqrt_r2[k]/L;
         sqrt_r2[k] = sqrt(sqrt_r2[k]);
      }
      // 
      for( int k=0 ; k<100 ; k++  ){
         ave[k] += sqrt_r2[k];
         ave2[k] += sqrt_r2[k]*sqrt_r2[k];
      }
      
   fout.open("dati/data.dat");
  
   for (int i = 0; i < N; i++) {
      ave[i]/=(100);
      ave2[i]/=(100);
      err[i] =sqrt((ave -ave2)/99); //giusto ??
      fout << ave[i] << "," << err[i] << endl;
   }
   fout.close();




   // alla fine del for calcolo la media di ogni elemento che ho accumulato 

   <vector>int posizione = {0,0,0,0,0,0}; // posizione iniziale (x+,x-,y+,y-,z+,z-)
   // un random walk
   // genero un numero che mi dica lungo quale asse mi sposto
   double direction = rnd.Rannyu();
   double intervalli[6] = {1.0/6, 2.0/6, 3.0/6, 4.0/6, 5.0/6, 1.0};
   int indice = 0;
   for (int i = 0; i < 6; i++) {
        if (direction < intervalli[i]) {
            indice = i;
            posizione.[i] += 1;
            break;
        }
   }
   r2 = (posizione[0] - posizione[1])^2 + (posizione[2] - posizione[3])^2 + (posizione[4] - posizione[5])^2;



 return 0;
}
