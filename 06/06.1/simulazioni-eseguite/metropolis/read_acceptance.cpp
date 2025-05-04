#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>

int main() {
    std::vector<std::string> folders;
    for (double x = 0.5; x <= 2.001; x += 0.15) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << x;
        folders.push_back(ss.str());
    }

    std::ofstream outfile("acceptance.dat");
    if (!outfile) {
        std::cerr << "Errore nell'apertura del file di output acceptance.dat\n";
        return 1;
    }

    for (const auto& folder : folders) {
        std::string filepath = folder + "/OUTPUT/acceptance.dat";
        std::ifstream infile(filepath);
        if (!infile) {
            std::cerr << "⚠️ Impossibile aprire " << filepath << '\n';
            continue;
        }

        std::string line;
        int current_line = 0;
        while (std::getline(infile, line)) {
            ++current_line;
            if (current_line == 21) {
                std::istringstream iss(line);
                int block;
                double acceptance;
                if (iss >> block >> acceptance) {
                    outfile << folder << " " << acceptance << "\n";
                } else {
                    std::cerr << "⚠️ Riga 21 malformata in " << filepath << '\n';
                }
                break;
            }
        }

        if (current_line < 21) {
            std::cerr << "⚠️ File " << filepath << " ha meno di 21 righe\n";
        }
    }

    outfile.close();
    return 0;
}
