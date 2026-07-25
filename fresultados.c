#include <stdio.h>
#include <stdlib.h>
#include "fresultados.h"

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
Punto* suprimir_no_maximos(int* acumulador, int ancho, int alto, int t, int v, int* count) {
    if (acumulador == NULL || count == NULL) {
        if (count != NULL) *count = 0;
        return NULL;
    }

    //ventana de vecindad: v es el tamano completo (impar), win es el radio
    int win = v / 2;

    //copia de los votos originales, usada para leer valores sin que la
    //supresion de unas celdas afecte la evaluacion de las demas
    int* acum_orig = (int*)malloc(ancho * alto * sizeof(int));
    if (acum_orig == NULL) {
        *count = 0;
        return NULL;
    }
    for (int i = 0; i < ancho * alto; i++) {
        acum_orig[i] = acumulador[i];
    }

    //por cada pixel
    for (int b = 0; b < alto; b++) {
        for (int a = 0; a < ancho; a++) {
            int val = acum_orig[b * ancho + a];

            //no cumple la cantidad de votos exigida: no puede ser maximo de nadie
            if (val < t) {
                acum_orig[b * ancho + a] = 0;
                continue;
            }

            int es_maximo = 1;
            //ver el alrededor de ese pixel en una ventana de v*v
            for (int ky = -win; ky <= win && es_maximo; ky++) {
                for (int kx = -win; kx <= win && es_maximo; kx++) {
                    if (kx == 0 && ky == 0) continue;
                    int nb = b + ky, na = a + kx;

                    if (nb >= 0 && nb < alto && na >= 0 && na < ancho) {
                        int vecino = acum_orig[nb * ancho + na];
                        //empate: pierde la celda que aparece despues en orden de barrido
                        if (vecino > val || (vecino == val && (nb < b || (nb == b && na < a)))) {
                            es_maximo = 0;
                        }
                    }
                }
            }
            if (!es_maximo) acumulador[b * ancho + a] = 0;
        }
    }
    free(acum_orig);

    //contar la cantidad de centros
    int total_centros = 0;
    for (int i = 0; i < ancho * alto; i++) {
        if (acumulador[i] >= t) {
            total_centros++;
        }
    }
    *count = total_centros;

    if (total_centros == 0) {
        return NULL;
    }

    Punto* centros = (Punto*)malloc(total_centros * sizeof(Punto));
    if (centros == NULL) {
        printf("resultados: fallo al asignar memoria para los centros detectados\n");
        *count = 0;
        return NULL;
    }

    int index = 0;
    for (int i = 0; i < ancho * alto; i++) {
        if (acumulador[i] >= t) {
            centros[index].x = i % ancho;
            centros[index].y = i / ancho;
            index++;
        }
    }

    return centros;
}

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
int generar_reporte(const char* ruta, Punto* centros, int count) {
    FILE* file = fopen(ruta, "w");
    if (file == NULL) {
        printf("resultados: no se pudo abrir el archivo %s para escribir\n", ruta);
        return 0;
    }

    fprintf(file, "X,Y\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%d\n", centros[i].x, centros[i].y);
    }

    fclose(file);
    return 1;
}
