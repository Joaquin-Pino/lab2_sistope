#include <stdio.h>
#include <stdlib.h>
#include "comun.h"
#include "ftHough.h"

//nodo "Votacion de Hough" del pipeline.
//argv[1]: radio (r) de los circulos a detectar, ya validado por lab2
//
//lee la imagen preprocesada de stdin, genera el plano acumulador de votos
//y lo escribe a stdout en el formato de acumulador (ver comun.h)
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "tHough: uso incorrecto (esperado: <radio>)\n");
        return 1;
    }

    int r = atoi(argv[1]);

    Imagen* img = leer_imagen_stream(stdin);
    if (img == NULL) {
        fprintf(stderr, "tHough: no se pudo leer la imagen de stdin\n");
        return 1;
    }

    int* acumulador = votar_hough(img, r);
    int ancho = img->ancho;
    int alto = img->alto;
    free_image(img);

    if (acumulador == NULL) {
        fprintf(stderr, "tHough: fallo al generar el acumulador de votos\n");
        return 1;
    }

    int ok = escribir_acumulador_stream(stdout, acumulador, ancho, alto);
    free(acumulador);

    if (!ok) {
        fprintf(stderr, "tHough: fallo al escribir el acumulador a stdout\n");
        return 1;
    }

    return 0;
}
