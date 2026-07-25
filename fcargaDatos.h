#ifndef FCARGADATOS_H
#define FCARGADATOS_H

#include "comun.h"

/*
Entradas:
    ruta (String: ruta del archivo .bin de entrada)
Salidas:
    Imagen* (imagen cargada, o NULL si el archivo no existe o esta incompleto)
Descripcion:
    abre el archivo de entrada y carga su contenido como una Imagen,
    usando el formato binario compartido (ver comun.h)
*/
Imagen* cargar_imagen_archivo(const char* ruta);

#endif //FCARGADATOS_H
