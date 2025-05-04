#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>

// Funzione per processare un file singolo (stessa logica per tutti i tipi)
void process_file(const std::vector<std::string>& folders, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Errore nell'apertura di " << filename << " per scrittura\n";
        return;
    }

    for (const auto& folder : folders) {
        std::string filepath =  folder + "/OUTPUT/" + filename;
        std::ifstream infile(filepath);
        if (!infile) {
            std::cerr << "Impossibile aprire " << filepath << '\n';
            continue;
        }

        std::string line;
        int line_count = 0;
        while (std::getline(infile, line)) {
            ++line_count;
            if (line_count == 21) {
                std::istringstream iss(line);
                double a, b, c, d;
                if (iss >> a >> b >> c >> d) {
                    outfile << folder << " " << a << " " << b << " " << c << " " << d << "\n";
                } else {
                    std::cerr << "Riga 21 di " << filepath << " non contiene 4 valori\n";
                }
                break;
            }
        }

        if (line_count < 21) {
            std::cerr << "Il file " << filepath << " ha meno di 21 righe\n";
        }
    }

    outfile.close();
}

int main() {
    std::vector<std::string> folders;
    for (double x = 0.5; x <= 2.001; x += 0.15) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << x;
        folders.push_back(ss.str());
    }

    std::vector<std::string> filenames = {
        "total_energy.dat",
        "magnetization.dat",
        "susceptibility.dat",
        "specific_heat.dat",
    };

    for (const auto& filename : filenames) {
        process_file(folders, filename);
    }

    return 0;
}
