#include <stdio.h>
#include "fcargaDatos.h"

/*
Entradas:
    ruta (String: ruta del archivo .bin de entrada)
Salidas:
    Imagen* (imagen cargada, o NULL si el archivo no existe o esta incompleto)
Descripcion:
    abre el archivo de entrada y carga su contenido como una Imagen,
    usando el formato binario compartido (ver comun.h)
*/
Imagen* cargar_imagen_archivo(const char* ruta) {
    //rb: lectura binaria
    FILE* archivo = fopen(ruta, "rb");
    if (archivo == NULL) {
        fprintf(stderr, "cargaDatos: no se pudo abrir el archivo %s\n", ruta);
        return NULL;
    }

    Imagen* img = leer_imagen_stream(archivo);
    fclose(archivo);

    if (img == NULL) {
        fprintf(stderr, "cargaDatos: el archivo %s esta corrupto o incompleto\n", ruta);
    }

    return img;
}
