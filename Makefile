# Flags per la compilazione
CFLAGS = -sdt=c89 -pedantic



# con "make clean" si eliminano i profotti della compilazione
# e si ripristina il conenuto originale
clean:
	rm -f *.o  *~	# aggiungere eseguibili prodotti
# *~ elimina tutti i file di tipo ~ (backup files)

all: #file1 #file2 #ecc.

# exec: file.c header.h Makefile
#		gcc $(CFLAGS) file.c -o exec


