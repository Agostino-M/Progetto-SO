# Flags per la compilazione
CFLAGS = -sdt=c89 -pedantic

# elenco degli object file necessari
OBJS = utility.o sem_lib.o	# potrebbero mancare moduli oggetto (da controllare)

# target di compilazione finale
TARGET = master	# potrebbero mancare eseguibili (da controllare)

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $(TARGET)

all: $(TARGET)

clean:				# lascia soltanto i file sorgente
	rm -f *.o $(TARGET) *~

run: $(TARGET)	# target per eseguire. Necessita dell'eseguibile
	./$(TARGET)

# compilazione delle librerie
sem_lib.o:
	gcc -c sem_lib.c

utility.o:
	gcc -c utility.c

