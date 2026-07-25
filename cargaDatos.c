#include <stdio.h>
#include <stdlib.h>
#include "comun.h"
#include "fcargaDatos.h"

//nodo "Carga de datos" del pipeline.
//argv[1]: ruta del archivo .bin de entrada
//argv[2]: "1" si la rama de analisis de ruido esta activa (-d), "0" si no
//
//escribe la imagen original a stdout (siempre, para preprocesamiento) y,
//si la rama esta activa, tambien al fd 3 (para aDeRuido)
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "cargaDatos: uso incorrecto (esperado: <ruta_entrada> <rama_activa>)\n");
        return 1;
    }

    const char* ruta = argv[1];
    int rama_activa = (argv[2][0] == '1');

    Imagen* img = cargar_imagen_archivo(ruta);
    if (img == NULL) {
        return 1;
    }

    if (!escribir_imagen_stream(stdout, img)) {
        fprintf(stderr, "cargaDatos: fallo al escribir la imagen a stdout\n");
        free_image(img);
        return 1;
    }

    if (rama_activa) {
        FILE* rama = fdopen(3, "wb");
        if (rama == NULL) {
            fprintf(stderr, "cargaDatos: fallo al abrir el fd de la rama de ruido\n");
            free_image(img);
            return 1;
        }
        int ok = escribir_imagen_stream(rama, img);
        fclose(rama);
        if (!ok) {
            fprintf(stderr, "cargaDatos: fallo al escribir la imagen a la rama de ruido\n");
            free_image(img);
            return 1;
        }
    }

    free_image(img);
    return 0;
}
