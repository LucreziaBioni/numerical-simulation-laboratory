CC = mpic++
CFLAGS = -Wall -O3 --std=c++11

main.exe : main.o random.o route.o population.o
	$(CC) population.o route.o random.o main.o -o main.exe
main.o : main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
route.o: route.cpp route.h
	$(CC) -c route.cpp -o route.o $(CFLAGS)
population.o: population.cpp population.h
	$(CC) -c population.cpp -o population.o $(CFLAGS)
clean :
	rm *.o main.exe seed.out