CC = g++
CFLAGS = -Wall -O3 --std=c++11
LDFLAGS = -larmadillo

EXEC = main.exe
OBJ = main.o random.o population.o route.o

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

main.o: main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAGS)

random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

population.o: population.cpp population.h route.h random.h
	$(CC) -c population.cpp -o population.o $(CFLAGS)

route.o: route.cpp route.h random.h
	$(CC) -c route.cpp -o route.o $(CFLAGS)

clean:
	rm -f *.o $(EXEC) seed.out
