# Flag per la compilazione nello standard c89 / -W disabilita warning su pedantic
CFLAGS = -std=c89 -pedantic
CFLAGS1 = 

# Nome dell'eseguibile
TARGET = Master
TARGET1 = Taxi

# Object files necessari per produrre l'eseguibile
OBJ = sem_lib.o list_lib.o utility.o master.o
OBJ1 = sem_lib.o list_lib.o utility.o taxi.o

HEADLER = sem_lib.h utility.h

# Regola che permette di creare i file oggetto unico e di produrre l'eseguibile
$(TARGET): $(OBJ) $(OBJ1)
	gcc $(OBJ1) $(CFLAGS1) $(CFLAGS) -o $(TARGET1) -lm
	gcc $(OBJ) $(CFLAGS1) $(CFLAGS) -o $(TARGET)

# Per produrre i file oggetto a bisogno di ingrediente headler
$(OBJ): $(HEADLER)
$(OBJ1): $(HEADLER)

# compila con -D DENSE
dense: 

# compila con -D LARGE
large: 

clean:
	rm -f *.o $(TARGET) *~
	rm -f *.o $(TARGET1) *~

# Regola che permette di eseguire
run: $(TARGET)
	./$(TARGET)

# Preset personalizzato secondo le esigenze di andrea :)
andrea:
	make clean
	clear
	make run

