#include "fpreprocesamiento.h"

//elemento estructurante cruz 3x3 usado por erosion() y dilatar()
static const int EL_ESTRUCTURANTE[9] = {0,1,0,1,1,1,0,1,0};

/*
Entradas:
    img (Imagen*: imagen a la que se desea aplicar "erosion")
Salidas:
    Imagen* (resultado de aplicar "erosion" a la img de entrada)
Descripcion:
    se le aplica erosion a la img de entrada (con el elemento estructurante
    cruz 3x3), lo cual elimina el ruido de la imagen
*/
Imagen* erosion(Imagen* img) {
    if (img == NULL) return NULL;

    Imagen* salida = (Imagen*)malloc(sizeof(Imagen));
    salida->ancho = img->ancho;
    salida->alto = img->alto;
    salida->data = (unsigned char*)malloc(img->ancho * img->alto * sizeof(unsigned char));

    //verificar por cada pixel
    for (int y = 0; y < img->alto; y++) {
        for (int x = 0; x < img->ancho; x++) {

            //bandera para ver si el pixel coincide con el elemento
            int coincide = 1;

            //recorrer el elemento 3x3
            //usamos "coincide" en la condicion para detener el ciclo antes si ya fallo
            for (int ky = -1; ky <= 1 && coincide; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    //calcular indice en el elemento 3x3
                    int indice = (ky + 1) * 3 + (kx + 1);

                    //solo si hay un 1 en el elemento, verificamos la imagen
                    if (EL_ESTRUCTURANTE[indice] == 1) {
                        int nx = x + kx;
                        int ny = y + ky;

                        //verificamos si se sale de los bordes O si el pixel de la imagen es 0
                        if (nx < 0 || nx >= img->ancho || ny < 0 || ny >= img->alto ||
                            img->data[ny * img->ancho + nx] == 0) {

                            coincide = 0; //no coincide, marcamos como 0
                            break; //rompemos el ciclo kx
                        }
                    }
                }
            }

            salida->data[y * salida->ancho + x] = coincide;
        }
    }

    return salida;
}

/*
Entradas:
    img (Imagen*: imagen a la que se desea aplicar "dilatar")
Salidas:
    Imagen* (resultado de aplicar "dilatar" a la img de entrada)
Descripcion:
    se le aplica dilatacion a la img de entrada (con el elemento
    estructurante cruz 3x3), lo cual restaura los circulos a su tamano ideal
*/
Imagen* dilatar(Imagen* img) {
    if (img == NULL) return NULL;

    Imagen* salida = (Imagen*)malloc(sizeof(Imagen));
    salida->ancho = img->ancho;
    salida->alto = img->alto;
    salida->data = (unsigned char*)malloc(img->ancho * img->alto * sizeof(unsigned char));

    for (int y = 0; y < img->alto; y++) {
        for (int x = 0; x < img->ancho; x++) {

            //bandera para ver si el pixel coincide con el elemento
            int coincide = 0;

            for (int ky = -1; ky <= 1 && !coincide; ky++) {
                for (int kx = -1; kx <= 1 && !coincide; kx++) {

                    int indice = (ky + 1) * 3 + (kx + 1);

                    if (EL_ESTRUCTURANTE[indice] == 1) {
                        int nx = x + kx;
                        int ny = y + ky;

                        //verificamos si no se sale de los bordes Y si el pixel de la imagen es 1
                        if (nx >= 0 && nx < img->ancho && ny >= 0 && ny < img->alto &&
                            img->data[ny * img->ancho + nx] == 1) {

                            coincide = 1; //coincide, marcamos como 1
                            break;
                        }
                    }
                }
            }

            salida->data[y * salida->ancho + x] = coincide;
        }
    }

    return salida;
}
