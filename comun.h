#ifndef COMUN_H
#define COMUN_H

#include <stdio.h>

//estructura para manejar la imagen en memoria
typedef struct {
    int ancho;
    int alto;
    unsigned char *data; //arreglo [ancho*alto]
} Imagen;

//estructura para almacenar las coordenadas
typedef struct {
    int x;
    int y;
} Punto;

/*
Entradas:
    stream (FILE*: origen ya abierto, p.ej. stdin o un archivo)
Salidas:
    Imagen* (estructura con los datos leidos, o NULL si hubo un error de lectura)
Descripcion:
    lee una imagen en el formato binario compartido por todo el pipeline
    (ancho:int, alto:int, data:ancho*alto bytes de 0/1) desde el stream indicado
*/
Imagen* leer_imagen_stream(FILE* stream);

/*
Entradas:
    stream (FILE*: destino ya abierto, p.ej. stdout o un archivo)
    img (Imagen*: imagen a escribir)
Salidas:
    int (0: no se pudo escribir completa, 1: se escribio correctamente)
Descripcion:
    escribe una imagen en el formato binario compartido por todo el pipeline
    (ancho:int, alto:int, data:ancho*alto bytes de 0/1) al stream indicado
*/
int escribir_imagen_stream(FILE* stream, Imagen* img);

/*
Entradas:
    stream (FILE*: origen ya abierto)
    ancho (int*: donde guardar el ancho leido)
    alto (int*: donde guardar el alto leido)
Salidas:
    int* (arreglo [ancho*alto] con los votos del acumulador, o NULL si hubo un error)
Descripcion:
    lee el plano acumulador de votos de Hough (ancho:int, alto:int,
    data:ancho*alto ints) desde el stream indicado. A diferencia de una
    Imagen, cada celda es un int porque los votos pueden superar 255
*/
int* leer_acumulador_stream(FILE* stream, int* ancho, int* alto);

/*
Entradas:
    stream (FILE*: destino ya abierto)
    acumulador (int*: arreglo [ancho*alto] con los votos)
    ancho (int: ancho del plano)
    alto (int: alto del plano)
Salidas:
    int (0: no se pudo escribir completo, 1: se escribio correctamente)
Descripcion:
    escribe el plano acumulador de votos de Hough al stream indicado, con
    el mismo formato que lee leer_acumulador_stream
*/
int escribir_acumulador_stream(FILE* stream, int* acumulador, int ancho, int alto);

/*
Entradas:
    img (Imagen*: dato que se desea liberar)
Salidas:
    void
Descripcion:
    Libera la memoria de los atributos y de la propia imagen recibida
*/
void free_image(Imagen* img);

#endif //COMUN_H
