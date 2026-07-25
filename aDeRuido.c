#include <stdio.h>
#include "comun.h"
#include "faDeRuido.h"

//nodo "Analisis de ruido" del pipeline (solo se ejecuta si se paso -d).
//lee la imagen original de stdin y la imagen preprocesada del fd 3,
//calcula el ruido (original - preprocesada) y exporta preprocesada.bin
//y ruido.bin en el directorio actual
int main(void) {
    Imagen* original = leer_imagen_stream(stdin);
    if (original == NULL) {
        fprintf(stderr, "aDeRuido: no se pudo leer la imagen original de stdin\n");
        return 1;
    }

    FILE* rama = fdopen(3, "rb");
    Imagen* preprocesada = (rama != NULL) ? leer_imagen_stream(rama) : NULL;
    if (rama != NULL) fclose(rama);

    if (preprocesada == NULL) {
        fprintf(stderr, "aDeRuido: no se pudo leer la imagen preprocesada del fd 3\n");
        free_image(original);
        return 1;
    }

    Imagen* ruido = get_ruido(original, preprocesada);

    int ok_preprocesada = guardar_imagen_archivo("preprocesada.bin", preprocesada);
    int ok_ruido = guardar_imagen_archivo("ruido.bin", ruido);

    free_image(original);
    free_image(preprocesada);
    free_image(ruido);

    if (!ok_preprocesada || !ok_ruido) {
        fprintf(stderr, "aDeRuido: no se pudo exportar una de las imagenes de depuracion\n");
        return 1;
    }

    return 0;
}
