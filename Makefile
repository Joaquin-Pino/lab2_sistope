CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

NODOS = cargaDatos preprocesamiento tHough resultados aDeRuido

all: lab2 $(NODOS)

# ---- orquestador ----

lab2: lab2.o
	$(CC) $(CFLAGS) -o lab2 lab2.o

lab2.o: lab2.c
	$(CC) $(CFLAGS) -c lab2.c

# ---- modulo compartido ----

comun.o: comun.c comun.h
	$(CC) $(CFLAGS) -c comun.c

# ---- nodo cargaDatos ----

cargaDatos: cargaDatos.o fcargaDatos.o comun.o
	$(CC) $(CFLAGS) -o cargaDatos cargaDatos.o fcargaDatos.o comun.o $(LDFLAGS)

cargaDatos.o: cargaDatos.c fcargaDatos.h comun.h
	$(CC) $(CFLAGS) -c cargaDatos.c

fcargaDatos.o: fcargaDatos.c fcargaDatos.h comun.h
	$(CC) $(CFLAGS) -c fcargaDatos.c

# ---- nodo preprocesamiento ----

preprocesamiento: preprocesamiento.o fpreprocesamiento.o comun.o
	$(CC) $(CFLAGS) -o preprocesamiento preprocesamiento.o fpreprocesamiento.o comun.o $(LDFLAGS)

preprocesamiento.o: preprocesamiento.c fpreprocesamiento.h comun.h
	$(CC) $(CFLAGS) -c preprocesamiento.c

fpreprocesamiento.o: fpreprocesamiento.c fpreprocesamiento.h comun.h
	$(CC) $(CFLAGS) -c fpreprocesamiento.c

# ---- nodo tHough ----

tHough: tHough.o ftHough.o comun.o
	$(CC) $(CFLAGS) -o tHough tHough.o ftHough.o comun.o $(LDFLAGS)

tHough.o: tHough.c ftHough.h comun.h
	$(CC) $(CFLAGS) -c tHough.c

ftHough.o: ftHough.c ftHough.h comun.h
	$(CC) $(CFLAGS) -c ftHough.c

# ---- nodo resultados ----

resultados: resultados.o fresultados.o comun.o
	$(CC) $(CFLAGS) -o resultados resultados.o fresultados.o comun.o $(LDFLAGS)

resultados.o: resultados.c fresultados.h comun.h
	$(CC) $(CFLAGS) -c resultados.c

fresultados.o: fresultados.c fresultados.h comun.h
	$(CC) $(CFLAGS) -c fresultados.c

# ---- nodo aDeRuido ----

aDeRuido: aDeRuido.o faDeRuido.o comun.o
	$(CC) $(CFLAGS) -o aDeRuido aDeRuido.o faDeRuido.o comun.o $(LDFLAGS)

aDeRuido.o: aDeRuido.c faDeRuido.h comun.h
	$(CC) $(CFLAGS) -c aDeRuido.c

faDeRuido.o: faDeRuido.c faDeRuido.h comun.h
	$(CC) $(CFLAGS) -c faDeRuido.c

clean:
	rm -f *.o lab2 $(NODOS)
