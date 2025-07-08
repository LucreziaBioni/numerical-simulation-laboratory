#include <armadillo>
#include "random.h"
#include "route.h"
#include "population.h"
#include <iostream>
#include <cmath>
#include <algorithm> 


// Inizializza la popolazione, il generatore di numeri casuali e assegna la matrice delle distanze
void Population::initialize_pop(int npop, int ngen, arma::mat *dist_matrix) {
    _npop = npop;     // Numero di individui nella popolazione
    _ngen = ngen;     // Numero di generazioni (step evolutivi)

    int p1, p2; // Variabili per i due numeri primi da usare per inizializzare il generatore di numeri casuali (RNG)

    // Legge i due numeri primi dal file "Primes"
    ifstream Primes("Primes");
    Primes >> p1 >> p2;
    Primes.close();

    int seed[4]; // Array per i 4 interi che rappresentano il seed del RNG

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current MPI process rank
    
    // Legge il seed dal file "seed.in"
    ifstream Seed("seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    Seed.close();

    //  Inizializza il generatore di numeri casuali con il seed e i numeri primi, rendendolo unico per ogni processo MPI
    for (int i = 0; i < 4; i++) {
        seed[i] += rank * 3; // scelta arbitraria per rendere il seed unico per ogni processo
    }

    // Inizializza il generatore di numeri casuali con il seed e i numeri primi
    _rnd.SetRandom(seed, p1 + rank, p2 + rank);

    // Copia il contenuto della matrice delle distanze passata come argomento nel membro interno della classe
    _dist_matrix = *dist_matrix;

    // Alloca lo spazio per la popolazione di individui
    _pop.set_size(_npop);

    // Inizializza ogni individuo della popolazione
    for (int i = 0; i < _npop; ++i) {
        // Ogni individuo riceve la matrice delle distanze e una copia del generatore casuale
        _pop[i].initialize(&_dist_matrix, _rnd);
    }
}


// Restituisce il percorso i-esimo della popolazione
Route Population :: get_percorso(int i){
    return _pop(i);
}

// Seleziona un indice della popolazione secondo uno specifico algoritmo
int Population::select_index() {
    double r = _rnd.Rannyu(); 
    int j = int(_npop * std::pow(r, _selexp)); // predilige indici più bassi (individui migliori)
    return j;
}

// Ordina la popolazione per lunghezza, mettendo i percorsi migliori (più corti) all'inizio del vettore
void Population::sort_by_length() {

    std::vector<Route> temp_vec(_npop);
    for (int i = 0; i < _npop; ++i){
        temp_vec[i] = _pop[i];
    }

    // Ordina il vector
    std::sort(temp_vec.begin(), temp_vec.end(), [&](const Route& a, const Route& b) {
        return a.calculate_length(&_dist_matrix) < b.calculate_length(&_dist_matrix);
    });
    
    // Copia indietro
    for (int i = 0; i < _npop; ++i){
        _pop[i] = temp_vec[i];
    }
}

// Seleziona e restituisce un individuo basato sul meccanismo di selezione select_index
Route Population::select() { 
    int j = select_index();
    return _pop(j);
}

// Restituisce la dimensione della popolazione
int Population::size() const {
    return _pop.size(); // Return the size of the population
}

// Funzione per il pbc (periodic boundary condition) che restituisce l'indice corretto
int Population :: pbc(int city) const{
    int ndim = 110; // Dimensione del percorso (numero di città)
    if(city >= ndim){
        return city - ndim + 1;
    }
    return city;
}

// Operatore di crossover tra due percorsi a e b
arma::field<Route> Population :: crossover(Route a, Route b){
    if (_rnd.Rannyu()<_pcross){
        arma::field<Route> figli(2);
        int n_cities = a.getdim();

        figli[0].initialize(&_dist_matrix, _rnd);
        figli[1].initialize(&_dist_matrix, _rnd);

        arma::Col<int> a_route = a.getroute(); // percorso del primo genitore
        arma::Col<int> b_route = b.getroute(); // percorso del secondo genitore

        int pos1 = int(_rnd.Rannyu(1, n_cities - 2)); // primo taglio (interno)
        int pos2 = int(_rnd.Rannyu(pos1 + 1, n_cities)); // secondo taglio (dopo il primo)

        arma::Col<int> figlio1_temp(n_cities, arma::fill::zeros);
        arma::Col<int> figlio2_temp(n_cities, arma::fill::zeros);

        // Copio - nei figli - il segmento tra i due tagli
        for (int i = pos1; i <= pos2; i++) {
            figlio1_temp[i] = a_route[i];
            figlio2_temp[i] = b_route[i];
        }

         // Riempie il resto del figlio1 con le città di b_route (escludendo duplicati)
        int idx1 = (pos2 + 1) % n_cities;
        int idx2 = idx1;
        for (int i = 0; i < n_cities; ++i) {
            int city = b_route[(pos2 + 1 + i) % n_cities]; // città successiva in ordine circolare
            if (arma::any(figlio1_temp == city)) continue; // salta se già presente
            figlio1_temp[idx1] = city;
            idx1 = (idx1 + 1) % n_cities; // passa alla posizione successiva
        }

        // Riempie il resto del figlio2 con le città di a_route (escludendo duplicati)
        for (int i = 0; i < n_cities; ++i) {
            int city = a_route[(pos2 + 1 + i) % n_cities];
            if (arma::any(figlio2_temp == city)) continue;
            figlio2_temp[idx2] = city;
            idx2 = (idx2 + 1) % n_cities;
        }

        figli[0].setroute(figlio1_temp);
        figli[1].setroute(figlio2_temp);

        if (!figli[0].check()) {
            cerr << "Errore: figlio1 non è un percorso valido." << endl;
        }
        if (!figli[1].check()) {
            cerr << "Errore: figlio2 non è un percorso valido." << endl;
        }
        
        return figli;
    }
    else{
        // Se il crossover non avviene (in base a probabilità), ritorna semplicemente i genitori
        return {a,b};
    }
}

// Ordina il vettore a in base alla posizione degli elementi in ref
arma::Col<int> Population :: sort_by_reference(arma::Col<int> a, arma::Col<int> ref){
    arma::Col<int> ref_pos(ref.size()); 
    for(int i = 0; i < ref.size(); i++){
        ref_pos[ref[i]-1] = i;
    }

    vector<int> sorted(a.begin(),a.end());
    sort(sorted.begin(), sorted.end(), [&ref_pos](int j, int k){
        return ref_pos[j] < ref_pos[k];
    });

    return arma::Col<int>(sorted); // riconverto vector in vettore di armadillo
}

// Funzione di evoluzione della popolazione
void Population::evolve( const arma::mat dist_matrix, int rank , int size) {

   
    ofstream coutf("./simulazioni-eseguite/results/results_" + to_string(rank) + ".dat");
    coutf << "#" << "\t\t L1 \t\t <L1>" << endl;

    sort_by_length(); // Ordina la popolazione iniziale in base alla lunghezza del percorso

    for (int i = 0; i < _ngen; i++){
        arma::field<Route> figli(_npop);
        for(int j = 0; j < _npop/2; j++){

            Route a = select();
            Route b = select(); // Seleziona due genitori dalla popolazione corrente

            arma::field<Route> temp = crossover(a,b);  // Applica l'operatore di crossover per ottenere due figli

             // Inizializza i due figli con matrice delle distanze e RNG
            figli[j].initialize(&_dist_matrix, _rnd);
            figli[j+_npop/2].initialize(&_dist_matrix, _rnd);

            // Imposta il percorso dei figli ottenuti dal crossover
            figli[j].setroute( temp[0].getroute());
            figli[j+_npop/2].setroute( temp[1].getroute());
        }

        // Applica mutazione a tutti i figli
        for(int j = 0; j < _npop; j++){
            figli[j].mutate();
        }

        // Aggiorna la popolazione con i figli (nuova generazione)
        _pop = figli;
        sort_by_length();

        // Migrazione ogni N_migr generazioni
        if (i % N_migr == 0 && i != 0 && size > 1) {
            int dim = 110; // Dimensione del percorso (numero di città)
            // Preparazione buffer per la migrazione
            arma::field<arma::Col<int>> migranti_inviare(_n_migranti);
            arma::field<arma::Col<int>> migranti_ricevuti(_n_migranti);
        
            // Seleziona individui da migrare (i migliori)
            for (int j = 0; j < _n_migranti; j++) {
                migranti_inviare[j] = _pop[j].getroute();  // Assumendo che getroute() restituisca arma::Col<int>
            }
        
            // Determinazione rank vicini
            int rank_dest = (rank + 1) % size;
            int rank_src = (rank - 1 + size) % size;
        
            // Creazione di richieste non bloccanti
            MPI_Request send_req, recv_req;
            arma::Col<int> buffer_invio, buffer_ricezione;
        
            // Impacchettamento dati da inviare
            buffer_invio.set_size(_n_migranti * dim);  // Allocazione esatta
            for (int k = 0, idx = 0; k < _n_migranti; k++) {
                const arma::Col<int>& percorso = migranti_inviare[k];
                for (int j = 0; j < dim; j++, idx++) {
                    buffer_invio[idx] = percorso[j];
                }
            }
        
            // Preparazione buffer ricezione
            buffer_ricezione.set_size(_n_migranti * dim);
        
            // Comunicazione non bloccante
            MPI_Isend(buffer_invio.memptr(), buffer_invio.n_elem, MPI_INT, 
                      rank_dest, 0, MPI_COMM_WORLD, &send_req);
            MPI_Irecv(buffer_ricezione.memptr(), buffer_ricezione.n_elem, MPI_INT, 
                      rank_src, 0, MPI_COMM_WORLD, &recv_req);
        
            // Attesa completamento comunicazione
            MPI_Status status;
            MPI_Wait(&send_req, &status);
            MPI_Wait(&recv_req, &status);
        
            // Spacchettamento dati ricevuti
            for (int j = 0; j < _n_migranti; j++) {
                arma::Col<int> percorso(dim);
                for (int k = 0; k < dim; k++) {
                    percorso[k] = buffer_ricezione[j * dim + k];
                }
                migranti_ricevuti[j] = percorso;
            }
        
            // Sostituzione dei peggiori individui con i migranti ricevuti
            for (int j = 0; j < _n_migranti; j++) {
                int idx = _npop - 1 - j;  // Prendi i peggiori individui
                _pop[idx].setroute(migranti_ricevuti[j]);
            }
        
            // Riordina la popolazione dopo la migrazione
            sort_by_length();
        }

        // Calcola la lunghezza media dei migliori N/2 individui
        double sum = 0;
        for (int j = 0; j < _npop/2; j++){
            sum += _pop[j].calculate_length(&dist_matrix);
        }

    
        // Salva su file: generazione, lunghezza del miglior individuo, media dei migliori N/2
        coutf << i << "\t\t" << _pop[0].calculate_length(&dist_matrix) << "\t\t" << sum/(_npop/2) << endl;
        
    }
    coutf.close();

    // Salva il miglior percorso trovato nell'ultima generazione
    ofstream out("./simulazioni-eseguite/best_route/best_route_" + to_string(rank) + ".");
    out << "#Best route \t \t x \t \t y" << endl;
    for (int i = 0; i < _pop[0].getdim(); i++){
        out << _pop[0].getroute()[i] << endl;
    }
    out.close();
    
}

