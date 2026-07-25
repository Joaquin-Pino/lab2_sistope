#ifndef FTHOUGH_H
#define FTHOUGH_H

#include "comun.h"

#define PI 3.14159265358979323846
//cantidad de pasos angulares exigidos por el enunciado del Lab 2
//(theta de 0 a 719, incrementos de 2*PI/720, en vez de los 360 pasos del Lab 1)
#define PASOS_THETA 720

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
int* votar_hough(Imagen* img, int r);

#endif //FTHOUGH_H
