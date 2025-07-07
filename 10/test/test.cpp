//#include "/usr/include/x86_64-linux-gnu/mpich/mpi.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inizializza MPI
    MPI_Init(&argc, &argv);

    // Ottieni il numero di processi
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ottieni il rank del processo
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Ottieni il nome del processore
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Stampa un messaggio
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalizza MPI
    MPI_Finalize();
    return 0;
}