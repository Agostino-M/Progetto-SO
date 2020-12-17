# Flags per la compilazione
CFLAGS = -std=c89 -pedantic

# elenco degli object file necessari
OBJ = utility.o sem_lib.o handling.o master.o	# potrebbero mancare moduli oggetto (da controllare)

# target di compilazione finale
TARGET = master	# potrebbero mancare eseguibili (da controllare)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

all: $(TARGET)

clear:				# lascia soltanto i file sorgente
	rm -f *.o $(TARGET) *~
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

run: $(TARGET)	# target per eseguire. Necessita dell'eseguibile
	./$(TARGET)

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

andrea:
	clear
	make clear
	make $(TARGET) 
	./$(TARGET)

# compilazione delle librerie
sem_lib.o:
	gcc -c sem_lib.c

utility.o:
	gcc -c utility.c

