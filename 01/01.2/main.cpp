#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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


   rnd.SaveSeed();

   vector<int> N = {1, 2, 10, 100};
   double lambda {1}, mu{0}, gamma{1};

   for (int i = 0; i < (int) N.size(); i++) { 
      string filename = "data_N=" + std::to_string( N[i] ) + ".dat"; 
      ofstream fout(filename); 
      // ogni file contiene medie di N elementi. La prima colonna da dado standard, seconda esponenziale, terza lorentziano
      if (fout.is_open()) {
         for(int j=0 ; j < 10000 ; j++){
            double sum_std = 0;
            double sum_exp = 0;
            double sum_lor = 0;
            for(int k=0 ; k < N[i] ; k++){
               sum_std = sum_std + rnd.Rannyu();
               sum_exp = sum_exp + rnd.Exponential(lambda);
               sum_lor = sum_lor + rnd.Lorentz( mu, gamma);
            }
            fout << (double) sum_std/N[i] << "\t" << (double) sum_exp/N[i] << "\t" <<  (double)sum_lor/N[i] << endl;
         }  
         fout.close();
      } else {
          std::cerr << "Errore nell'apertura del file: " << filename << endl;
      }
  }



   return 0;
}

