#include <stdio.h>
#include <stdlib.h>
#include "comun.h"
#include "fresultados.h"

//nodo "Salida de resultados" del pipeline.
//argv[1]: umbral de confianza (t), ya validado por lab2
//argv[2]: tamano de vecindad de supresion de no maximos (v), ya validado
//argv[3]: ruta del archivo .csv de salida
//
//lee el acumulador de votos de stdin, aplica supresion de no maximos +
//umbral, y escribe el reporte final directamente al archivo de salida
int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "resultados: uso incorrecto (esperado: <t> <v> <ruta_salida>)\n");
        return 1;
    }

    int t = atoi(argv[1]);
    int v = atoi(argv[2]);
    const char* ruta_salida = argv[3];

    int ancho, alto;
    int* acumulador = leer_acumulador_stream(stdin, &ancho, &alto);
    if (acumulador == NULL) {
        fprintf(stderr, "resultados: no se pudo leer el acumulador de stdin\n");
        return 1;
    }

    int count = 0;
    Punto* centros = suprimir_no_maximos(acumulador, ancho, alto, t, v, &count);
    free(acumulador);

    if (!generar_reporte(ruta_salida, centros, count)) {
        free(centros);
        return 1;
    }

    printf("resultados: %d centro(s) detectado(s), reporte guardado en %s\n", count, ruta_salida);

    free(centros);
    return 0;
}
