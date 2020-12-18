# Flags per la compilazione
CFLAGS = -std=c89 -pedantic

# Elenco degli object file necessari
OBJ = utility.o sem_lib.o handling.o taxi.o master.o

# Target di compilazione finale
TARGET = master	# Potrebbero mancare eseguibili (da controllare)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

all: $(TARGET)

clear:				# Lascia soltanto i file sorgente
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

run: $(TARGET)	# Target per eseguire. Necessita dell'eseguibile
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

# Preset personalizzato secondo le esigenze di andrea :)
andrea:
	clear
	make clear
	make $(TARGET) 
	./$(TARGET)

# Compilazione delle librerie
sem_lib:
	gcc -c sem_lib.c

utility:
	gcc -c utility.c

handling:
	gcc -c handling.c