# Flag per la compilazione nello standard c89 / -W disabilita warning su pedantic
CFLAGS = -std=c89 -pedantic #-Wpedantic

# Nome dell'eseguibile
TARGET = Master
TARGET1 = Taxi

# Object files necessari per produrre l'eseguibile
OBJ = sem_lib.o utility.o master.o
OBJ1 = sem_lib.o utility.o taxi.o

HEADLER = sem_lib.h utility.h

# Regola che permette di creare i file oggetto unico e di produrre l'eseguibile
$(TARGET): $(OBJ) $(OBJ1)
	gcc $(OBJ1) $(LDFLAGS) -o $(TARGET1) -lm
	gcc $(OBJ) $(LDFLAGS) -o $(TARGET)

# Per produrre i file oggetto a bisogno di ingrediente headler
$(OBJ): $(HEADLER)
$(OBJ1): $(HEADLER)

dense: $(TARGET)
	export SO_HOLES = 10;                                   \
	export SO_TOP_CELLS = 40;                               \
	export SO_SOURCES = SO_WIDTH * SO_HEIGHT - SO_HOLES;    \
	export SO_CAP_MIN = 1;                                  \
	export SO_CAP_MAX = 1;                                  \
	export SO_TAXI = SO_SOURCES / 2;                        \
	export SO_TIMENSEC_MIN = 100000000;                     \
	export SO_TIMENSEC_MAX = 300000000;                     \
	export SO_TIMEOUT = 1;                                  \
	export SO_DURATION = 20;                                \
	./$(TARGET)

large: $(TARGET)
	export SO_HOLES = 50;                                   \
	export SO_TOP_CELLS = 40;                               \
	export SO_SOURCES = 10;                                 \
	export SO_CAP_MIN = 3;                                  \
	export SO_CAP_MAX = 5;                                  \
	export SO_TAXI = 1000;                                  \
	export SO_TIMENSEC_MIN = 10000000;                      \
	export SO_TIMENSEC_MAX = 100000000;                     \
	export SO_TIMEOUT = 3;                                  \
	export SO_DURATION = 20;                                \
	./$(TARGET)

clean:
	rm -f *.o $(TARGET) *~
	rm -f *.o $(TARGET1) *~
	unset SO_HOLES;                                     \
	unset SO_TOP_CELLS;                                 \
	unset SO_SOURCES;                                   \
	unset SO_CAP_MIN;                                   \
	unset SO_CAP_MAX;                                   \
	unset SO_TAXI;                                      \
	unset SO_TIMENSEC_MIN;                              \
	unset SO_TIMENSEC_MAX;                              \
	unset SO_TIMEOUT;                                   \
	unset SO_DURATION;                                  \

# Regola che permette di eseguire
run: $(TARGET)
	./$(TARGET)

# Preset personalizzato secondo le esigenze di andrea :)
andrea:
	clear
	make clean
	make run

# Compilazione delle librerie
sem_lib:
	gcc -c sem_lib.c

utility:
	gcc -c utility.c
