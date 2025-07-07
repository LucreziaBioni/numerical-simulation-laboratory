#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <mpi.h>
#include "population.h"
#include "random.h"
#include "route.h"

using namespace std;

void read_cities( int n_cities, arma::mat & dist_matrix);

int main(int argc, char** argv){

    // Inizializza MPI
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int n_cities = 110;
    arma::mat dist_matrix(n_cities, n_cities);
    read_cities(n_cities, dist_matrix);

    Population pop;
    int npop = 2000;
    int ngen = 3000;
    pop.initialize_pop(npop, ngen, &dist_matrix);

    if(world_rank == 0) {
        cout << "Avvio evoluzione parallela con " << world_size << " processi" << endl;
    }

    pop.evolve(dist_matrix, world_rank, world_size);

    // Salvataggio miglior percorso locale
    Route local_best = pop.get_percorso(0);
    double local_best_length = local_best.calculate_length(&dist_matrix);
    double global_best_length;
    int best_rank;

    // 1. Raccogli tutte le lunghezze su rank 0
    double* all_lengths = nullptr;
    if (world_rank == 0) {
        all_lengths = new double[world_size];
    }

    MPI_Gather(&local_best_length, 1, MPI_DOUBLE, all_lengths, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 2. Processo 0 trova il minimo e il rank corrispondente
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

    // 3. Broadcast dei risultati a tutti i processi
    MPI_Bcast(&global_best_length, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&best_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 4. Comunicazione del miglior percorso
    if (world_rank == 0) {
        vector<int> global_best_route(n_cities);
        
        if (best_rank == 0) {
            // Se il miglior percorso è già sul rank 0
            for (int i = 0; i < n_cities; i++) {
                global_best_route[i] = local_best.getstop(i);
            }
        } else {
            // Ricevo il miglior percorso dal rank che lo possiede
            MPI_Recv(global_best_route.data(), n_cities, MPI_INT,
                    best_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Salvo il miglior percorso globale
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
        // Se questo processo ha il miglior percorso, lo invio al rank 0
        vector<int> best_route(n_cities);
        for (int i = 0; i < n_cities; i++) {
            best_route[i] = local_best.getstop(i);
        }
        MPI_Send(best_route.data(), n_cities, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // 5. Pulizia memoria
    if (world_rank == 0) {
        delete[] all_lengths;
    }

    MPI_Finalize();
    return 0;
}


// Legge coordinate delle città e calcola la matrice delle distanze
void read_cities( int n_cities, arma::mat & dist_matrix) {
    
    vec x(n_cities);
    vec y(n_cities);

    ifstream province("cap_prov_ita.dat"); // Apre il file con le coordinate delle città
    if (!province) {
        cerr << "Error: could not open cap_prov_ita.dat" << endl;
        exit(1); 
    }

    // Legge coordinate x e y delle città dal file
    for (int i = 0; i < n_cities; i++) {
        province >> x[i] >> y[i];
    }
    province.close();

    // Calculate the distance matrix
    for(int i = 0; i < n_cities; i++) {
        for(int j = 0; j <  n_cities; j++) {
            dist_matrix(i,j) = sqrt(pow(x(i)-x(j),2) + pow(y(i)-y(j),2)); // L1 norm
            //dist_matrix(j,i) = dist_matrix(i,j); // Symmetric matrix
        }
    }
}




