#include <stdio.h>
#include <stdlib.h>
#include "comun.h"

/*
Entradas:
    stream (FILE*: origen ya abierto, p.ej. stdin o un archivo)
Salidas:
    Imagen* (estructura con los datos leidos, o NULL si hubo un error de lectura)
Descripcion:
    lee una imagen en el formato binario compartido por todo el pipeline
    (ancho:int, alto:int, data:ancho*alto bytes de 0/1) desde el stream indicado
*/
Imagen* leer_imagen_stream(FILE* stream) {
    if (stream == NULL) return NULL;

    Imagen* img = (Imagen*)malloc(sizeof(Imagen));
    if (img == NULL) return NULL;

    //leer ancho y alto; si el stream se corta antes, se aborta
    if (fread(&(img->ancho), sizeof(int), 1, stream) != 1 ||
        fread(&(img->alto), sizeof(int), 1, stream) != 1) {
        free(img);
        return NULL;
    }

    int cant_pixeles = img->ancho * img->alto;
    img->data = (unsigned char*)malloc(cant_pixeles * sizeof(unsigned char));
    if (img->data == NULL) {
        free(img);
        return NULL;
    }

    //leer todos los pixeles; si llegan menos de los esperados, se aborta
    if (fread(img->data, sizeof(unsigned char), cant_pixeles, stream) != (size_t)cant_pixeles) {
        free(img->data);
        free(img);
        return NULL;
    }

    return img;
}

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
int escribir_imagen_stream(FILE* stream, Imagen* img) {
    if (stream == NULL || img == NULL) return 0;

    int cant_pixeles = img->ancho * img->alto;

    if (fwrite(&(img->ancho), sizeof(int), 1, stream) != 1) return 0;
    if (fwrite(&(img->alto), sizeof(int), 1, stream) != 1) return 0;
    if (fwrite(img->data, sizeof(unsigned char), cant_pixeles, stream) != (size_t)cant_pixeles) return 0;

    //aseguramos que los datos salgan del buffer de stdio (importante al escribir a un pipe)
    fflush(stream);
    return 1;
}

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
int* leer_acumulador_stream(FILE* stream, int* ancho, int* alto) {
    if (stream == NULL || ancho == NULL || alto == NULL) return NULL;

    if (fread(ancho, sizeof(int), 1, stream) != 1 ||
        fread(alto, sizeof(int), 1, stream) != 1) {
        return NULL;
    }

    int total = (*ancho) * (*alto);
    int* acumulador = (int*)malloc(total * sizeof(int));
    if (acumulador == NULL) return NULL;

    if (fread(acumulador, sizeof(int), total, stream) != (size_t)total) {
        free(acumulador);
        return NULL;
    }

    return acumulador;
}

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
int escribir_acumulador_stream(FILE* stream, int* acumulador, int ancho, int alto) {
    if (stream == NULL || acumulador == NULL) return 0;

    int total = ancho * alto;

    if (fwrite(&ancho, sizeof(int), 1, stream) != 1) return 0;
    if (fwrite(&alto, sizeof(int), 1, stream) != 1) return 0;
    if (fwrite(acumulador, sizeof(int), total, stream) != (size_t)total) return 0;

    fflush(stream);
    return 1;
}

/*
Entradas:
    img (Imagen*: dato que se desea liberar)
Salidas:
    void
Descripcion:
    Libera la memoria de los atributos y de la propia imagen recibida
*/
void free_image(Imagen* img) {
    if (img != NULL) {
        free(img->data);
        free(img);
    }
}
