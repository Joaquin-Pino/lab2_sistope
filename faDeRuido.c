#include <stdio.h>
#include <stdlib.h>
#include "faDeRuido.h"

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
Imagen* get_ruido(Imagen* original, Imagen* preprocesada) {
    if (original == NULL || preprocesada == NULL) return NULL;

    Imagen* resultado = (Imagen*)malloc(sizeof(Imagen));
    resultado->ancho = original->ancho;
    resultado->alto = original->alto;
    //calloc: los pixeles que no son ruido deben quedar en 0
    resultado->data = (unsigned char*)calloc(resultado->ancho * resultado->alto, sizeof(unsigned char));

    for (int i = 0; i < original->alto; i++) {
        for (int j = 0; j < original->ancho; j++) {
            int indice = i * original->ancho + j;

            if (original->data[indice] == 1) {
                //ambas imagenes tienen ese pixel, no es ruido
                if (preprocesada->data[indice] == 1) {
                    resultado->data[indice] = 0;
                }
                //solo la original lo tiene, es ruido
                else {
                    resultado->data[indice] = 1;
                }
            }
        }
    }

    return resultado;
}

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
int guardar_imagen_archivo(const char* ruta, Imagen* img) {
    FILE* archivo = fopen(ruta, "wb");
    if (archivo == NULL) {
        printf("aDeRuido: no se pudo abrir el archivo %s para escribir\n", ruta);
        return 0;
    }

    int ok = escribir_imagen_stream(archivo, img);
    fclose(archivo);
    return ok;
}
