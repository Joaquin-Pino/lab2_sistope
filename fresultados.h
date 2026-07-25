#ifndef FRESULTADOS_H
#define FRESULTADOS_H

#include "comun.h"

/*
Entradas:
    acumulador (int*: arreglo [ancho*alto] con los votos de Hough, se
                modifica: las celdas que no son maximo local quedan en 0)
    ancho (int: ancho del plano)
    alto (int: alto del plano)
    t (int: cantidad de votos necesarios para que se considere un centro valido)
    v (int: tamano de la vecindad de supresion, impar y >= 1)
    count (int*: guarda la cantidad de centros encontrados)
Salidas:
    Punto* (arreglo de centros encontrados, o NULL si no se encontro ninguno)
Descripcion:
    aplica supresion de no maximos sobre el acumulador usando una vecindad
    de v*v centrada en cada pixel, y extrae los centros cuyo valor de votos
    sea >= t
*/
Punto* suprimir_no_maximos(int* acumulador, int ancho, int alto, int t, int v, int* count);

/*
Entradas:
    ruta (String: nombre del archivo .csv de salida)
    centros (Punto*: arreglo de centros encontrados)
    count (int: cantidad de centros en el arreglo centros)
Salidas:
    int (0: si no se pudo guardar el archivo, 1: se guardo el archivo correctamente)
Descripcion:
    genera el archivo .csv con el reporte de todos los centros encontrados
*/
int generar_reporte(const char* ruta, Punto* centros, int count);

#endif //FRESULTADOS_H
