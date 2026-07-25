#ifndef FPREPROCESAMIENTO_H
#define FPREPROCESAMIENTO_H

#include "comun.h"

/*
Entradas:
    img (Imagen*: imagen a la que se desea aplicar "erosion")
Salidas:
    Imagen* (resultado de aplicar "erosion" a la img de entrada)
Descripcion:
    se le aplica erosion a la img de entrada (con el elemento estructurante
    cruz 3x3), lo cual elimina el ruido de la imagen
*/
Imagen* erosion(Imagen* img);

/*
Entradas:
    img (Imagen*: imagen a la que se desea aplicar "dilatar")
Salidas:
    Imagen* (resultado de aplicar "dilatar" a la img de entrada)
Descripcion:
    se le aplica dilatacion a la img de entrada (con el elemento
    estructurante cruz 3x3), lo cual restaura los circulos a su tamano ideal
*/
Imagen* dilatar(Imagen* img);

#endif //FPREPROCESAMIENTO_H
