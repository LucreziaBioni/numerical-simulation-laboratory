# Compilatore e flag
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Sorgenti e binari
SRC1 = process_all.cpp
SRC2 = read_acceptance.cpp

BIN1 = process_all
BIN2 = read_acceptance

# Regola di default
all: $(BIN1) $(BIN2)

# Compilazione dei due eseguibili
$(BIN1): $(SRC1)
	$(CXX) $(CXXFLAGS) -o $(BIN1) $(SRC1)

$(BIN2): $(SRC2)
	$(CXX) $(CXXFLAGS) -o $(BIN2) $(SRC2)

# Esecuzione di entrambi
run: $(BIN1) $(BIN2)
	./$(BIN1)
	./$(BIN2)

# Esecuzione solo del secondo (acceptance)
run_acceptance: $(BIN2)
	./$(BIN2)

# Pulizia
clean:
	rm -f $(BIN1) $(BIN2)
