#ifndef FADERUIDO_H
#define FADERUIDO_H

#include "comun.h"

/*
Entradas:
    original (Imagen*: imagen original, antes del preprocesamiento)
    preprocesada (Imagen*: imagen despues de aplicar erosion y dilatacion)
Salidas:
    Imagen* (resultado de la resta de ambas imagenes, es el "ruido")
Descripcion:
    a la imagen original se le resta la preprocesada para obtener el
    ruido que elimino la apertura morfologica
*/
Imagen* get_ruido(Imagen* original, Imagen* preprocesada);

/*
Entradas:
    ruta (String: nombre deseado para el archivo .bin)
    img (Imagen*: datos de la imagen que se desea guardar)
Salidas:
    int (0: no se pudo guardar, 1: se guardo correctamente)
Descripcion:
    guarda la Imagen recibida como un archivo binario en la ruta indicada,
    usando el formato binario compartido (ver comun.h)
*/
int guardar_imagen_archivo(const char* ruta, Imagen* img);

#endif //FADERUIDO_H
