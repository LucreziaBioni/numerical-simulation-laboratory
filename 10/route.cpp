#include <iostream>
#include <math.h>
#include "route.h"

using namespace std;

// Operatore di assegnazione: copia i dati di un altro oggetto Route
Route& Route::operator=(const Route& other) {
    if (this != &other) {
        this->_route = other._route;           // vettore delle città nell'ordine del percorso
        this->_length = other._length;         // lunghezza totale del percorso
        this->_ndim = other._ndim;             // numero di città
        this->_point_rnd = other._point_rnd;   // puntatore al generatore di numeri casuali
    }
    return *this;
}

// Colloca una città in una posizione specifica del percorso
void Route::setstop(int stop, int city) {
    _route(stop) = city;
}

// Restituisce la città presente in una data posizione del percorso
int Route::getstop(int stop) {
    return _route(stop);
}

// Verifica che il percorso non contenga città ripetute
bool Route::check() {
    for (int i = 0; i < _ndim; i++) {
        for (int j = i + 1; j < _ndim; j++) {
            if (_route(i) == _route(j)) {
                return false;  // città duplicata trovata
            }
        }
    }
    return true;  // nessuna città duplicata
}

// Restituisce l'indice della città con condizioni periodiche al contorno
int Route::pbc(int city) const {
    if (city >= _ndim) {
        return city - _ndim + 1;
    }
    return city;
}

// Scambia due città in posizioni specifiche nel percorso
void Route::swap(int i, int j) {
    int temp = _route(i);
    _route(i) = _route(j);
    _route(j) = temp;
}

// Scambia due città in posizioni casuali (escludendo la prima posizione)
void Route::swap() {
    int i = (int) _point_rnd->Rannyu(1, _ndim);  // esclude la posizione 0
    int j = (int) _point_rnd->Rannyu(1, _ndim);
    int temp = _route(i);
    _route(i) = _route(j);
    _route(j) = temp;
}

// Calcola la lunghezza del percorso usando la matrice delle distanze
double Route::calculate_length(const arma::mat *distance_matrix) const {
    double length = 0;
    for (int i = 0; i < _ndim - 1; i++) {
        length += distance_matrix->at(_route(i) - 1, _route(pbc(i + 1)) - 1);
    }
    // Aggiunge distanza tra l’ultima città e la prima (circuito chiuso)
    length += distance_matrix->at(_route(_ndim - 1) - 1, _route(0) - 1);
    return length;
}

// Inizializza un percorso casuale e ne calcola la lunghezza
void Route::initialize(arma::mat *distance_matrix, Random &rnd) {
    _point_rnd = &rnd;
    _route.resize(_ndim);

    // Inizializza il percorso con l’ordine naturale delle città (1, 2, ..., N)
    for (int i = 0; i < _ndim; i++) {
        this->setstop(i, i + 1);
    }

    // Effettua n_swaps scambi casuali per randomizzare il percorso iniziale
    int n_swaps = _ndim;
    for (int i = 0; i < n_swaps; i++) {
        int j = (int) rnd.Rannyu(1, _ndim);
        int k = (int) rnd.Rannyu(1, _ndim);
        this->swap(j, k);
    }

    // Calcola e memorizza la lunghezza del percorso
    _length = this->calculate_length(distance_matrix);
}

// Sposta un blocco di città in avanti di n posizioni nel percorso
void Route::shift() {
    int m = (int) _point_rnd->Rannyu(1, _ndim - 2);              // lunghezza del blocco
    int start = (int) _point_rnd->Rannyu(1, _ndim - m);          // inizio del blocco
    int n = (int) _point_rnd->Rannyu(1, _ndim - m - start + 1);  // numero di posizioni da spostare

    arma::Col<int> new_route(_ndim);
    int idx = 0;

    // Copia le città prima del blocco
    for (int i = 0; i < start; ++i)
        new_route(idx++) = _route(i);

    // Copia le città tra blocco e posizione d'inserimento
    for (int i = start + m; i < start + m + n && i < _ndim; ++i)
        new_route(idx++) = _route(i);

    // Inserisce il blocco da spostare
    for (int i = start; i < start + m; ++i)
        new_route(idx++) = _route(i);

    // Copia le città rimanenti
    for (int i = start + m + n; i < _ndim; ++i)
        new_route(idx++) = _route(i);

    _route = new_route;

    // Verifica validità del nuovo percorso
    if (!check()) {
        cout << "Error: Route not valid after shift" << endl;
    }
}

// Scambia due blocchi di città non sovrapposti di lunghezza m
void Route::swap_block() {
    int m = (int)_point_rnd->Rannyu(1, _ndim / 2);
    int pos1 = (int)_point_rnd->Rannyu(1, _ndim - 2 * m);
    int pos2 = (int)_point_rnd->Rannyu(pos1 + m, _ndim - m);  // garantisce blocchi separati

    // Estrae i due blocchi
    arma::Col<int> block1 = _route.subvec(pos1, pos1 + m - 1);
    arma::Col<int> block2 = _route.subvec(pos2, pos2 + m - 1);

    // Scambia i blocchi
    _route.subvec(pos1, pos1 + m - 1) = block2;
    _route.subvec(pos2, pos2 + m - 1) = block1;

    if (!check()) cout << "Error: Route not valid after swap_block" << endl;
}

// Inverte l'ordine delle città tra due indici casuali
void Route::invert_block() {
    int start = pbc((int) _point_rnd->Rannyu(1, _ndim - 1));
    int end = pbc((int) _point_rnd->Rannyu(1, _ndim - 1));

    if (start > end) {
        std::swap(start, end);
    }

    // Crea copia temporanea del percorso
    arma::Col<int> temp(_ndim);
    for (int i = 0; i < _ndim; i++) {
        temp(i) = _route(i);
    }

    // Inverte l'intervallo selezionato
    while (start < end) {
        _route(start) = temp(end);
        _route(end) = temp(start);
        start++;
        end--;
    }

    if (!check()) {
        cout << "Error: Route not valid after invert_block" << endl;
    }
}

// Applica una mutazione casuale al percorso, con probabilità _pmut
void Route::mutate() {
    double i = _point_rnd->Rannyu();  // numero casuale tra 0 e 1
    if (i < _pmut) {                  // solo se è inferiore a _pmut
        double sel = _point_rnd->Rannyu();
        if (sel < 0.25) {
            swap();           // scambia due città
        } else if (sel < 0.5) {
            shift();          // sposta un blocco
        } else if (sel < 0.75) {
            swap_block();     // scambia due blocchi
        } else {
            invert_block();   // inverte un blocco
        }
    }
}
