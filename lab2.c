#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

//rutas de los ejecutables de cada nodo del pipeline (se espera correr
//./lab2 desde el mismo directorio donde quedaron compilados)
#define RUTA_CARGA_DATOS "./cargaDatos"
#define RUTA_PREPROCESAMIENTO "./preprocesamiento"
#define RUTA_THOUGH "./tHough"
#define RUTA_RESULTADOS "./resultados"
#define RUTA_ADERUIDO "./aDeRuido"

//valores por defecto de las flags opcionales
#define V_DEFAULT 7
#define O_DEFAULT "reporte.csv"

/*
Entradas:
    void
Salidas:
    void
Descripcion:
    imprime un recordatorio de como se usa el programa
*/
static void imprimir_uso(void) {
    printf("Uso: ./lab2 -i entrada.bin -r radio -t umbral [-v vecindad] [-o salida.csv] [-d]\n");
    printf("  -i  ruta del archivo .bin de entrada (obligatoria)\n");
    printf("  -r  radio entero positivo de los circulos a detectar (obligatoria)\n");
    printf("  -t  umbral de confianza (votos minimos), entero positivo (obligatoria)\n");
    printf("  -v  vecindad de supresion de no maximos, entero positivo impar (default 7)\n");
    printf("  -o  archivo .csv de salida (default reporte.csv)\n");
    printf("  -d  exporta ademas preprocesada.bin y ruido.bin\n");
}

/*
Entradas:
    str (String: texto recibido desde argv, optarg de una flag)
    out (int*: donde guardar el valor parseado si es valido)
Salidas:
    int (0: 'str' no es un entero positivo valido, 1: se parseo correctamente)
Descripcion:
    valida que 'str' sea solo digitos (nada de puntos, signos ni letras) y,
    si es asi, lo convierte con atoi().
    Asi se rechazan casos como "4.5" o "3abc".
*/
static int parse_entero_positivo(const char* str, int* out) {
    if (str == NULL || str[0] == '\0') return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return 0; //encontramos algo que no es digito
    }

    int valor = atoi(str);
    if (valor <= 0) return 0; //el enunciado exige un entero positivo

    *out = valor;
    return 1;
}

int main(int argc, char* argv[]) {
    //flags obligatorias
    char* ruta_i = NULL;
    int r = 0;
    int t = 0;
    //flags opcionales
    int v = V_DEFAULT;
    char* ruta_o = O_DEFAULT;
    int flag_d = 0;

    //revisar toda la entrada
    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        switch (opt) {
            case 'i':
                ruta_i = optarg;
                break;
            case 'r':
                if (!parse_entero_positivo(optarg, &r)) { //verificar que sea un entero positivo
                    printf("lab2: -r debe ser un entero positivo (recibido: '%s')\n", optarg);
                    return 1;
                }
                break;
            case 't':
                if (!parse_entero_positivo(optarg, &t)) { //verificar que sea un entero positivo
                    printf("lab2: -t debe ser un entero positivo (recibido: '%s')\n", optarg);
                    return 1;
                }
                break;
            case 'v':
                if (!parse_entero_positivo(optarg, &v) || v % 2 == 0) { //verificar que sea un entero positivo impar
                    printf("lab2: -v debe ser un entero positivo impar (recibido: '%s')\n", optarg);
                    return 1;
                }
                break;
            case 'o':
                ruta_o = optarg;
                break;
            case 'd':
                flag_d = 1;
                break;
            default:
                imprimir_uso();
                return 1;
        }
    }

    //si alguna de las flags obligatorias no se paso, mostrar mensaje de error y uso, cancelar el programa
    if (ruta_i == NULL || r == 0 || t == 0) {
        printf("lab2: faltan flags obligatorias (-i, -r y -t son requeridas)\n");
        imprimir_uso();
        return 1;
    }

    //los parametros numericos se pasan a los nodos como argv (ya validados)
    char r_str[16], t_str[16], v_str[16]; //buffers para los valores
    snprintf(r_str, sizeof(r_str), "%d", r); //convertir el valor r a stirng y guardarlo en r_str
    snprintf(t_str, sizeof(t_str), "%d", t); //convertir el valor t a stirng y guardarlo en t_str
    snprintf(v_str, sizeof(v_str), "%d", v); //convertir el valor v a stirng y guardarlo en v_str

    //crear pipes para cada proceso
    //pipes de la linea principal del pipeline
    int p_carga_prepro[2], p_prepro_hough[2], p_hough_resultados[2];
    //pipes de la rama opcional de analisis de ruido (solo si -d)
    int p_carga_ruido[2], p_prepro_ruido[2];

    //si algun pipe falla, no se puede continuar: imprimir error y salir
    if (pipe(p_carga_prepro) == -1 || pipe(p_prepro_hough) == -1 || pipe(p_hough_resultados) == -1) {
        perror("lab2: pipe");
        return 1;
    }
    //si algun pipe de la rama de ruido falla, no se puede continuar: imprimir error y salir
    if (flag_d) {
        if (pipe(p_carga_ruido) == -1 || pipe(p_prepro_ruido) == -1) {
            perror("lab2: pipe");
            return 1;
        }
    }

    //en cada hijo se cierran primero los extremos de pipe que ese nodo no
    //usa y recien despues se hacen los dup2(). Ese orden importa: los fd de
    //los pipes son siempre >= 3 (el 0, 1 y 2 ya estan ocupados), asi que el
    //fd 3 se libera al cerrar los extremos sobrantes y queda disponible como
    //destino para la rama de analisis de ruido
    
    //guardar los pids de cada hijo para esperar a que terminen al final
    pid_t pids[5];
    int n_pids = 0; //cotador de pids guardados
    pid_t pid; //pid temporal para cada fork

    //---- nodo cargaDatos ----
    pid = fork(); //se crea el proceso hijo para el nodo cargaDatos
    if (pid == -1) { //verificar que el fork() no haya fallado
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) { //si es el hijo, ejecutar el nodo cargaDatos
        //cerrar los extremos de pipe que no se usan en este nodo
        //solo escribe: hacia preprocesamiento y, si corresponde, hacia aDeRuido
        close(p_carga_prepro[0]); //solo escribe en este pipe, no lee
        close(p_prepro_hough[0]); //no participa en este pipe
        close(p_prepro_hough[1]);
        close(p_hough_resultados[0]); //no participa en este pipe
        close(p_hough_resultados[1]);
        if (flag_d) {
            close(p_carga_ruido[0]); //solo escribe en este pipe, no lee
            close(p_prepro_ruido[0]); //no participa en este pipe
            close(p_prepro_ruido[1]);
        }

        //redirigir stdout al pipe correspondiente (escribir en preprocesamiento)
        dup2(p_carga_prepro[1], 1); //stdout -> preprocesamiento
        close(p_carga_prepro[1]);
        
        if (flag_d) {
            //redirigir stdout al pipe correspondiente (escribir en aDeRuido)
            dup2(p_carga_ruido[1], 3); //fd 3 -> aDeRuido (imagen original)
            close(p_carga_ruido[1]);
        }

        //preparar argumentos de entrada para el nodo cargaDatos
        char* args[] = { RUTA_CARGA_DATOS, ruta_i, flag_d ? "1" : "0", NULL };
        execv(RUTA_CARGA_DATOS, args); //reemplazar el proceso hijo con cargaDatos
        perror("lab2: execv cargaDatos"); //error si execv() falla
        exit(1); //terminar el proceso hijo
    }
    //guardar el pid del hijo y continuar con el siguiente nodo del pipeline
    pids[n_pids++] = pid;

    //---- nodo preprocesamiento ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        close(p_carga_prepro[1]);
        close(p_prepro_hough[0]);
        close(p_hough_resultados[0]);
        close(p_hough_resultados[1]);
        if (flag_d) {
            close(p_carga_ruido[0]);
            close(p_carga_ruido[1]);
            close(p_prepro_ruido[0]);
        }

        dup2(p_carga_prepro[0], 0); //stdin <- cargaDatos
        close(p_carga_prepro[0]);   //aca se libera el fd 3

        dup2(p_prepro_hough[1], 1); //stdout -> tHough
        close(p_prepro_hough[1]);

        if (flag_d) {
            dup2(p_prepro_ruido[1], 3); //fd 3 -> aDeRuido (imagen preprocesada)
            close(p_prepro_ruido[1]);
        }

        char* args[] = { RUTA_PREPROCESAMIENTO, flag_d ? "1" : "0", NULL };
        execv(RUTA_PREPROCESAMIENTO, args);
        perror("lab2: execv preprocesamiento");
        exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo tHough ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        close(p_carga_prepro[0]);
        close(p_carga_prepro[1]);
        close(p_prepro_hough[1]);
        close(p_hough_resultados[0]);
        if (flag_d) {
            close(p_carga_ruido[0]);
            close(p_carga_ruido[1]);
            close(p_prepro_ruido[0]);
            close(p_prepro_ruido[1]);
        }

        dup2(p_prepro_hough[0], 0); //stdin <- preprocesamiento
        close(p_prepro_hough[0]);

        dup2(p_hough_resultados[1], 1); //stdout -> resultados
        close(p_hough_resultados[1]);

        char* args[] = { RUTA_THOUGH, r_str, NULL };
        execv(RUTA_THOUGH, args);
        perror("lab2: execv tHough");
        exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo resultados ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        close(p_carga_prepro[0]);
        close(p_carga_prepro[1]);
        close(p_prepro_hough[0]);
        close(p_prepro_hough[1]);
        close(p_hough_resultados[1]);
        if (flag_d) {
            close(p_carga_ruido[0]);
            close(p_carga_ruido[1]);
            close(p_prepro_ruido[0]);
            close(p_prepro_ruido[1]);
        }

        dup2(p_hough_resultados[0], 0); //stdin <- tHough
        close(p_hough_resultados[0]);

        char* args[] = { RUTA_RESULTADOS, t_str, v_str, ruta_o, NULL };
        execv(RUTA_RESULTADOS, args);
        perror("lab2: execv resultados");
        exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo aDeRuido (rama opcional, solo si -d) ----
    if (flag_d) {
        pid = fork();
        if (pid == -1) {
            perror("lab2: fork");
            return 1;
        }
        if (pid == 0) {
            close(p_carga_prepro[0]);
            close(p_carga_prepro[1]);
            close(p_prepro_hough[0]);
            close(p_prepro_hough[1]);
            close(p_hough_resultados[0]);
            close(p_hough_resultados[1]);
            close(p_carga_ruido[1]);
            close(p_prepro_ruido[1]);

            dup2(p_carga_ruido[0], 0); //stdin <- cargaDatos (imagen original)
            close(p_carga_ruido[0]);

            dup2(p_prepro_ruido[0], 3); //fd 3 <- preprocesamiento (imagen preprocesada)
            close(p_prepro_ruido[0]);

            char* args[] = { RUTA_ADERUIDO, NULL };
            execv(RUTA_ADERUIDO, args);
            perror("lab2: execv aDeRuido");
            exit(1);
        }
        pids[n_pids++] = pid;
    }

    //el padre no participa en la transferencia de datos: debe cerrar todos
    //los fd de pipes que le quedan abiertos, o algun lector nunca vera EOF
    close(p_carga_prepro[0]);
    close(p_carga_prepro[1]);
    close(p_prepro_hough[0]);
    close(p_prepro_hough[1]);
    close(p_hough_resultados[0]);
    close(p_hough_resultados[1]);
    if (flag_d) {
        close(p_carga_ruido[0]);
        close(p_carga_ruido[1]);
        close(p_prepro_ruido[0]);
        close(p_prepro_ruido[1]);
    }

    //esperar a que todos los nodos del pipeline terminen
    for (int i = 0; i < n_pids; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
