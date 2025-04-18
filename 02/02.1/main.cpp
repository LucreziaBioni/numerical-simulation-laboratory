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

   int M = 100000; // numero totale di stime
   int N = 100; // numero di blocchi
   int L = M/N; // numero di stime per blocco


   double ave_blocco{}; // quando inserito nel ciclo for, contiene la media delle medie dei primi (i+1) blocchi
   double ave2_blocco{}; // quando inserito nel ciclo for, la media dei quadrati delle medie dei primi (i+1) blocchi
   double partial_sum{}; // accumula la somma delle medie dei primi (i+1) blocchi
   double partial_sum2{}; // accumula la somma delle medie dei quadrati 
   ofstream fout_media ("data_media.dat");

   // integrale con il metodo della media
   for( int i = 0; i< N ; i++ ){ // effettuo un ciclo sul numero di blocchi
      double stima = 0;
      for( int j=0 ; j<L ; j++ ){ // effettuo L stime dell'integrale
         stima = stima + ( M_PI/2 * cos(M_PI * rnd.Rannyu() / 2) );
      }
      partial_sum= partial_sum + stima/L;
      partial_sum2 =partial_sum2 + stima/L * stima/L;
      ave_blocco = (partial_sum) / (i+1);
      ave2_blocco = ( partial_sum2 ) / (i+1);
      fout_media << (i+1) << "\t" << stima/L - 1 << "\t" << (ave_blocco - 1) << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
      // prima colonna: numero di blocchi
      // seconda colonna: valore medio dell'i-esimo blocco
      // terza colonna: valore medio delle medie dei primi i blocchi
      // quarta colonna: incertezza associata a valore medio dei primi i blocchi
   }
   fout_media.close();


   ave_blocco=0, ave2_blocco =0 , partial_sum = 0 , partial_sum2 = 0;
   // integrale con il metodo dell'importance sampling
   ofstream fout_importance ("data_importance.dat");
   for( int i = 0; i< N ; i++ ){ // effettuo un ciclo sul numero di blocchi
      double stima = 0;
      for( int j=0 ; j<L ; j++ ){ // effettuo L stime dell'integrale
         double y = rnd.Rannyu();
         double x = 1 - sqrt(1-y);
         stima = stima + ( M_PI/2 * cos(M_PI * x / 2) / (2 * (1 - x)) );
      }
      partial_sum= partial_sum + stima/L;
      partial_sum2 =partial_sum2 + stima/L * stima/L;
      ave_blocco = (partial_sum) / (i+1);
      ave2_blocco = ( partial_sum2 ) / (i+1);
      fout_importance << (i+1) << "\t" << stima/L -1 << "\t" << (ave_blocco -1 ) << "\t" << error_double(ave_blocco, ave2_blocco, i) << endl;
   }
   fout_importance.close();


}
