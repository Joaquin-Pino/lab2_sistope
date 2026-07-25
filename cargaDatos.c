#include <stdio.h>
#include "comun.h"
#include "fcargaDatos.h"

//nodo "Carga de datos" del pipeline.
//argv[1]: ruta del archivo .bin de entrada
//argv[2]: "1" si la rama de analisis de ruido esta activa (-d), "0" si no
//
//escribe la imagen original a stdout (siempre, para preprocesamiento) y,
//si la rama esta activa, tambien al fd 3 (para aDeRuido)
int main(int argc, char* argv[]) {
    //espera exactamente 3 argumentos: nombre del programa, ruta de entrada y flag de rama
    if (argc != 3) {
        fprintf(stderr, "cargaDatos: uso incorrecto (esperado: <ruta_entrada> <rama_activa>)\n");
        return 1;
    }

    //guardar los argumentos en variables locales para mayor claridad
    const char* ruta = argv[1]; //ruta del archivo .bin de entrada
    int rama_activa = (argv[2][0] == '1'); //flag para el analisis de ruido (1 si esta activa, 0 si no)

    //cargar la imagen desde el archivo especificado
    Imagen* img = cargar_imagen_archivo(ruta);
    if (img == NULL) {
        return 1;
    }

    //escribir la imagen original a stdout (para preprocesamiento)
    if (!escribir_imagen_stream(stdout, img)) {
        fprintf(stderr, "cargaDatos: fallo al escribir la imagen a stdout\n");
        free_image(img);
        return 1;
    }

    //si está el analisis de ruido activo, escribir la imagen original al fd 3 (para aDeRuido)
    if (rama_activa) {
        //abrir el fd 3 como un stream para escritura
        FILE* rama = fdopen(3, "wb");
        if (rama == NULL) { //error al abrir el fd 3 como stream
            fprintf(stderr, "cargaDatos: fallo al abrir el fd de la rama de ruido\n");
            free_image(img);
            return 1;
        }
        //escribir la imagen original al stream de la rama de ruido
        int ok = escribir_imagen_stream(rama, img);
        fclose(rama); //cerrar el fd 3
        if (!ok) { //error al escribir la imagen a la rama de ruido
            fprintf(stderr, "cargaDatos: fallo al escribir la imagen a la rama de ruido\n");
            free_image(img);
            return 1;
        }
    }

    //liberar la memoria de la imagen original y terminar el proceso
    free_image(img);
    return 0;
}
