# common è di default, provare con "make BUILD=dense" o "make BUILD=large"
BUILD := common

cflags.common := -std=c89 -pedantic
cflags.dense := -DDENSE
cflags.large := -DLARGE
CFLAGS := ${cflags.${BUILD}} ${cflags.common}

# Nome dell'eseguibile
TARGET = Master
TARGET1 = Taxi

# Object files necessari per produrre l'eseguibile
OBJ = sem_lib.o list_lib.o utility.o master.o
OBJ1 = sem_lib.o list_lib.o utility.o taxi.o

HEADLER = sem_lib.h utility.h

# Regola che permette di creare i file oggetto unico e di produrre l'eseguibile
$(TARGET): $(OBJ) $(OBJ1)
	gcc  $(OBJ1) $(CFLAGS) -o $(TARGET1) -lm
	gcc $(OBJ) $(CFLAGS) -o $(TARGET)

# Per produrre i file oggetto a bisogno di ingrediente headler
$(OBJ): $(HEADLER)
$(OBJ1): $(HEADLER)

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

