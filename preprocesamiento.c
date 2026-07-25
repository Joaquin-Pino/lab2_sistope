#include <stdio.h>
#include "comun.h"
#include "fpreprocesamiento.h"

//nodo "Preprocesamiento" del pipeline.
//argv[1]: "1" si la rama de analisis de ruido esta activa (-d), "0" si no
//
//lee la imagen original de stdin, aplica apertura morfologica (erosion +
//dilatacion) y escribe la imagen preprocesada a stdout (siempre, para
//tHough) y, si la rama esta activa, tambien al fd 3 (para aDeRuido)
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "preprocesamiento: uso incorrecto (esperado: <rama_activa>)\n");
        return 1;
    }

    int rama_activa = (argv[1][0] == '1');

    Imagen* original = leer_imagen_stream(stdin);
    if (original == NULL) {
        fprintf(stderr, "preprocesamiento: no se pudo leer la imagen de stdin\n");
        return 1;
    }

    Imagen* erosionada = erosion(original);
    Imagen* preprocesada = dilatar(erosionada);
    free_image(original);
    free_image(erosionada);

    if (preprocesada == NULL) {
        fprintf(stderr, "preprocesamiento: fallo al procesar la imagen\n");
        return 1;
    }

    if (!escribir_imagen_stream(stdout, preprocesada)) {
        fprintf(stderr, "preprocesamiento: fallo al escribir la imagen a stdout\n");
        free_image(preprocesada);
        return 1;
    }

    if (rama_activa) {
        FILE* rama = fdopen(3, "wb");
        if (rama == NULL) {
            fprintf(stderr, "preprocesamiento: fallo al abrir el fd de la rama de ruido\n");
            free_image(preprocesada);
            return 1;
        }
        int ok = escribir_imagen_stream(rama, preprocesada);
        fclose(rama);
        if (!ok) {
            fprintf(stderr, "preprocesamiento: fallo al escribir la imagen a la rama de ruido\n");
            free_image(preprocesada);
            return 1;
        }
    }

    free_image(preprocesada);
    return 0;
}
