# Nome dell'eseguibile
EXEC = process_all

# Compilatore
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# File sorgente
SRC = process_all.cpp

# Regola principale
all: $(EXEC)

# Regola per compilare l'eseguibile
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)

# Pulizia dei file compilati
clean:
	rm -f $(EXEC)

# Esegui il programma dopo la compilazione
run: $(EXEC)
	./$(EXEC)
