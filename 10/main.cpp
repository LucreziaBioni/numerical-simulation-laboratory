#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <mpi.h>             // Libreria per calcolo parallelo MPI
#include "population.h"      // Classe che gestisce una popolazione di percorsi
#include "random.h"          // Classe per generazione numeri casuali
#include "route.h"           // Classe per rappresentare un percorso

using namespace std;

// Funzione che legge le coordinate delle città e costruisce la matrice delle distanze
void read_cities( int n_cities, arma::mat & dist_matrix);

int main(int argc, char** argv){

    // Inizializza l'ambiente MPI
    MPI_Init(&argc, &argv);

    // Ottiene il numero totale di processi e il rank (ID) del processo corrente
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Numero di città da visitare
    int n_cities = 110;

    // Matrice delle distanze tra città
    arma::mat dist_matrix(n_cities, n_cities);
    read_cities(n_cities, dist_matrix); // Riempie la matrice

    // Inizializzazione della popolazione
    Population pop;
    int npop = 6000;     // Numero di individui nella popolazione
    int ngen = 1000;     // Numero di generazioni dell'evoluzione
    pop.initialize_pop(npop, ngen, &dist_matrix);

    // Messaggio iniziale da parte del processo principale
    if(world_rank == 0) {
        cout << "Avvio evoluzione parallela con " << world_size << " processi" << endl;
    }

    // Evoluzione della popolazione (algoritmo genetico)
    pop.evolve(dist_matrix, world_rank, world_size);

    // Ogni processo estrae il proprio miglior percorso
    Route local_best = pop.get_percorso(0);
    double local_best_length = local_best.calculate_length(&dist_matrix);
    cout << "Rank " << world_rank << " best route length: " << local_best_length << endl;
    double global_best_length;
    int best_rank;

    // === FASE DI RIDUZIONE DEI RISULTATI ===

    // Array per raccogliere le lunghezze migliori da ogni processo (solo rank 0 lo usa)
    double* all_lengths = nullptr;
    if (world_rank == 0) {
        all_lengths = new double[world_size];
    }

    // Raccolta delle lunghezze migliori da ogni processo su rank 0
    MPI_Gather(&local_best_length, 1, MPI_DOUBLE, all_lengths, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Il processo 0 determina la lunghezza minima tra tutti i processi
    if (world_rank == 0) {
        global_best_length = all_lengths[0];
        best_rank = 0;
        for (int i = 1; i < world_size; i++) {
            if (all_lengths[i] < global_best_length) {
                global_best_length = all_lengths[i];
                best_rank = i;
            }
        }
    }

    // I processi ricevono dal rank 0 il valore della miglior lunghezza e il rank associato
    MPI_Bcast(&global_best_length, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&best_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // === RACCOLTA E SALVATAGGIO DEL MIGLIOR PERCORSO ===

    if (world_rank == 0) {
        vector<int> global_best_route(n_cities);
        
        if (best_rank == 0) {
            // Se il miglior percorso è già nel processo 0, lo copia
            for (int i = 0; i < n_cities; i++) {
                global_best_route[i] = local_best.getstop(i);
            }
        } else {
            // Altrimenti riceve il miglior percorso dal processo che lo possiede
            MPI_Recv(global_best_route.data(), n_cities, MPI_INT,
                     best_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Scrive su file il miglior percorso globale
        ofstream fout_global("global_best_route.dat");
        fout_global << "# Miglior percorso globale (lunghezza: " << global_best_length << ")" << endl;
        for (int city : global_best_route) {
            fout_global << city << endl;
        }
        fout_global.close();

        cout << "Evoluzione completata. Miglior lunghezza trovata: " 
             << global_best_length << " dal rank " << best_rank << endl;
    } 
    else if (world_rank == best_rank) {
        // Il processo che ha il miglior percorso lo invia al processo 0
        vector<int> best_route(n_cities);
        for (int i = 0; i < n_cities; i++) {
            best_route[i] = local_best.getstop(i);
        }
        MPI_Send(best_route.data(), n_cities, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Deallocazione memoria sul processo 0
    if (world_rank == 0) {
        delete[] all_lengths;
    }

    // Terminazione dell’ambiente MPI
    MPI_Finalize();
    return 0;
}

// Legge le coordinate delle città da un file e costruisce la matrice delle distanze euclidee
void read_cities( int n_cities, arma::mat & dist_matrix) {
    
    vec x(n_cities);  // Vettore coordinate x
    vec y(n_cities);  // Vettore coordinate y

    ifstream province("cap_prov_ita.dat"); // Apre il file con le coordinate delle città
    if (!province) {
        cerr << "Error: could not open cap_prov_ita.dat" << endl;
        exit(1); 
    }

    // Legge le coordinate da file
    for (int i = 0; i < n_cities; i++) {
        province >> x[i] >> y[i];
    }
    province.close();

    // Calcola la matrice delle distanze euclidee tra tutte le coppie di città
    for(int i = 0; i < n_cities; i++) {
        for(int j = 0; j <  n_cities; j++) {
            dist_matrix(i,j) = sqrt(pow(x(i)-x(j),2) + pow(y(i)-y(j),2));
            // Nota: non è necessario rendere esplicitamente simmetrica la matrice
        }
    }
}
