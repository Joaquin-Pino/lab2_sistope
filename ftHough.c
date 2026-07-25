#include <stdlib.h>
#include <math.h>
#include "ftHough.h"

/*
Entradas:
    img (Imagen*: imagen preprocesada sobre la que se va a votar)
    r (int: radio de los circulos que se desea buscar)
Salidas:
    int* (arreglo [ancho*alto] con la cantidad de votos de cada pixel,
          o NULL si hubo un error)
Descripcion:
    por cada pixel blanco de la imagen, vota en el acumulador de Hough
    recorriendo PASOS_THETA angulos (theta de 0 a 719, paso 2*PI/720).
    No aplica supresion de no maximos ni umbral: esa logica le corresponde
    al nodo "resultados"
*/
int* votar_hough(Imagen* img, int r) {
    if (img == NULL || img->data == NULL || r <= 0) return NULL;

    int ancho = img->ancho;
    int alto = img->alto;

    //memoria dinamica inicializada en 0 (calloc)
    int* acumulador = (int*)calloc(ancho * alto, sizeof(int));
    if (acumulador == NULL) return NULL;

    //votamos por cada pixel que sea 1 (borde)
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {

            if (img->data[y * ancho + x] == 1) {

                //recorremos los PASOS_THETA angulos exigidos por el enunciado
                for (int theta = 0; theta < PASOS_THETA; theta++) {

                    double radianes = theta * 2.0 * PI / PASOS_THETA;

                    //redondeo simetrico al entero mas cercano (en lugar de truncar)
                    int a = x - (int)lround(r * cos(radianes));
                    int b = y - (int)lround(r * sin(radianes));

                    //verificamos limites
                    if (a >= 0 && a < ancho && b >= 0 && b < alto) {
                        acumulador[b * ancho + a]++;
                    }
                }
            }
        }
    }

    return acumulador;
}
