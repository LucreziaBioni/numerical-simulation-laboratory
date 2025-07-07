# Compilatore e flag
CC = mpic++
CFLAGS = -Wall -O3 --std=c++11
LDFLAGS = -larmadillo

# Nomi file
TARGET = tsp_parallel
SRCS = main.cpp population.cpp route.cpp random.cpp
OBJS = $(SRCS:.cpp=.o)

# Regole principali
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Regole di compilazione
main.o: main.cpp population.h route.h random.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

population.o: population.cpp population.h route.h random.h
	$(CC) $(CFLAGS) -c population.cpp -o population.o

route.o: route.cpp route.h random.h
	$(CC) $(CFLAGS) -c route.cpp -o route.o

random.o: random.cpp random.h
	$(CC) $(CFLAGS) -c random.cpp -o random.o

# Esecuzione
run: $(TARGET)
	@echo "Avvio esecuzione MPI con 12 processi..."
	mpirun -np 12 ./$(TARGET)

# Esecuzione con numero personalizzato di processi
run-%: $(TARGET)
	@echo "Avvio esecuzione MPI con $* processi..."
	mpirun -np $* ./$(TARGET)

# Pulizia
clean:
	rm -f $(OBJS) $(TARGET) *.dat

.PHONY: all clean run run-%