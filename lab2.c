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
    imprime en stderr un recordatorio de como se usa el programa
*/
static void imprimir_uso(void) {
    fprintf(stderr, "Uso: ./lab2 -i entrada.bin -r radio -t umbral [-v vecindad] [-o salida.csv] [-d]\n");
    fprintf(stderr, "  -i  ruta del archivo .bin de entrada (obligatoria)\n");
    fprintf(stderr, "  -r  radio entero positivo de los circulos a detectar (obligatoria)\n");
    fprintf(stderr, "  -t  umbral de confianza (votos minimos), entero positivo (obligatoria)\n");
    fprintf(stderr, "  -v  vecindad de supresion de no maximos, entero positivo impar (default 7)\n");
    fprintf(stderr, "  -o  archivo .csv de salida (default reporte.csv)\n");
    fprintf(stderr, "  -d  exporta ademas preprocesada.bin y ruido.bin\n");
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

/*
Entradas:
    todos (int*: arreglo con todos los fd de pipes creados por el orquestador)
    n_todos (int: cantidad de elementos en 'todos')
    conservar (int*: arreglo con los fd que este hijo SI necesita mantener abiertos)
    n_conservar (int: cantidad de elementos en 'conservar')
Salidas:
    void
Descripcion:
    cierra, en el proceso hijo actual, todos los fd de pipes que no esten
    en la lista 'conservar'. Debe llamarse justo despues del fork(), antes
    de cualquier dup2(), para que ningun hijo herede descriptores de pipes
    que no le corresponden (si quedaran abiertos, algun lector del otro
    extremo podria no ver nunca EOF y el pipeline se colgaria)
*/
static void cerrar_no_necesarios(int* todos, int n_todos, int* conservar, int n_conservar) {
    for (int i = 0; i < n_todos; i++) {
        int es_necesario = 0;
        for (int j = 0; j < n_conservar; j++) {
            if (todos[i] == conservar[j]) {
                es_necesario = 1;
                break;
            }
        }
        if (!es_necesario) close(todos[i]);
    }
}

/*
Entradas:
    fuentes (int*: arreglo con los fd origen a duplicar; se modifica in-place)
    destinos (int*: arreglo con los fd destino deseados, p.ej. 0, 1 o 3)
    n (int: cantidad de pares fuente/destino)
Salidas:
    void
Descripcion:
    conecta cada fuentes[i] a destinos[i] mediante dup2(), dejando los
    descriptores 0/1/3 del proceso listos para execv(). Como pipe()
    reparte los numeros de fd mas bajos disponibles, es comun que un fd de
    pipe ya "sea" el mismo numero (0, 1 o 3) que otro par necesita como
    destino; por eso, antes de aplicar los dup2() reales, esta funcion
    mueve a un fd temporal (con dup()) cualquier fuente que colisione con
    el destino de otro par, para que ningun dup2() pise una fuente que
    todavia no se uso
*/
static void configurar_fds_hijo(int* fuentes, int* destinos, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j && fuentes[i] == destinos[j]) {
                int temporal = dup(fuentes[i]);
                close(fuentes[i]);
                fuentes[i] = temporal;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        dup2(fuentes[i], destinos[i]);
    }

    //cerrar las fuentes ya duplicadas (un close repetido sobre el mismo
    //numero que un destino seria incorrecto, por eso se compara antes)
    for (int i = 0; i < n; i++) {
        if (fuentes[i] != destinos[i]) close(fuentes[i]);
    }
}

int main(int argc, char* argv[]) {
    char* ruta_i = NULL;
    int r = 0, t = 0, v = V_DEFAULT;
    char* ruta_o = O_DEFAULT;
    int flag_d = 0;

    int i_seen = 0, r_seen = 0, t_seen = 0;

    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        switch (opt) {
            case 'i':
                ruta_i = optarg;
                i_seen = 1;
                break;
            case 'r':
                if (!parse_entero_positivo(optarg, &r)) {
                    fprintf(stderr, "lab2: -r debe ser un entero positivo (recibido: '%s')\n", optarg);
                    return 1;
                }
                r_seen = 1;
                break;
            case 't':
                if (!parse_entero_positivo(optarg, &t)) {
                    fprintf(stderr, "lab2: -t debe ser un entero positivo (recibido: '%s')\n", optarg);
                    return 1;
                }
                t_seen = 1;
                break;
            case 'v':
                if (!parse_entero_positivo(optarg, &v) || v % 2 == 0) {
                    fprintf(stderr, "lab2: -v debe ser un entero positivo impar (recibido: '%s')\n", optarg);
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

    if (!i_seen || !r_seen || !t_seen) {
        fprintf(stderr, "lab2: faltan flags obligatorias (-i, -r y -t son requeridas)\n");
        imprimir_uso();
        return 1;
    }

    if (access(ruta_i, R_OK) != 0) {
        fprintf(stderr, "lab2: no se puede leer el archivo de entrada '%s'\n", ruta_i);
        return 1;
    }

    //los parametros numericos se pasan a los nodos como argv (ya validados)
    char r_str[16], t_str[16], v_str[16];
    snprintf(r_str, sizeof(r_str), "%d", r);
    snprintf(t_str, sizeof(t_str), "%d", t);
    snprintf(v_str, sizeof(v_str), "%d", v);

    //pipes de la linea principal del pipeline
    int p_carga_prepro[2], p_prepro_hough[2], p_hough_resultados[2];
    //pipes de la rama opcional de analisis de ruido (solo si -d)
    int p_carga_ruido[2] = {-1, -1}, p_prepro_ruido[2] = {-1, -1};

    if (pipe(p_carga_prepro) == -1 || pipe(p_prepro_hough) == -1 || pipe(p_hough_resultados) == -1) {
        perror("lab2: pipe");
        return 1;
    }
    if (flag_d) {
        if (pipe(p_carga_ruido) == -1 || pipe(p_prepro_ruido) == -1) {
            perror("lab2: pipe");
            return 1;
        }
    }

    //lista de todos los fd de pipes que existen, usada para el cierre
    //selectivo en cada hijo (ver cerrar_no_necesarios)
    int todos[10];
    int n_todos = 0;
    todos[n_todos++] = p_carga_prepro[0];
    todos[n_todos++] = p_carga_prepro[1];
    todos[n_todos++] = p_prepro_hough[0];
    todos[n_todos++] = p_prepro_hough[1];
    todos[n_todos++] = p_hough_resultados[0];
    todos[n_todos++] = p_hough_resultados[1];
    if (flag_d) {
        todos[n_todos++] = p_carga_ruido[0];
        todos[n_todos++] = p_carga_ruido[1];
        todos[n_todos++] = p_prepro_ruido[0];
        todos[n_todos++] = p_prepro_ruido[1];
    }

    pid_t pids[5];
    int n_pids = 0;
    pid_t pid;

    //---- nodo cargaDatos ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        // soy el hijo, cargamos los dtaos de cargaDatos
        int fuentes[2], destinos[2], n = 0;
        fuentes[n] = p_carga_prepro[1]; 
        destinos[n] = 1; 
        n++;
        if (flag_d) { fuentes[n] = p_carga_ruido[1]; destinos[n] = 3; n++; }

        cerrar_no_necesarios(todos, n_todos, fuentes, n);
        configurar_fds_hijo(fuentes, destinos, n);

        char* args[] = { RUTA_CARGA_DATOS, ruta_i, flag_d ? "1" : "0", NULL };
        execv(RUTA_CARGA_DATOS, args);
        perror("lab2: execv cargaDatos");
        _exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo preprocesamiento ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        int fuentes[3], destinos[3], n = 0;
        fuentes[n] = p_carga_prepro[0]; destinos[n] = 0; n++;
        fuentes[n] = p_prepro_hough[1]; destinos[n] = 1; n++;
        if (flag_d) { fuentes[n] = p_prepro_ruido[1]; destinos[n] = 3; n++; }

        cerrar_no_necesarios(todos, n_todos, fuentes, n);
        configurar_fds_hijo(fuentes, destinos, n);

        char* args[] = { RUTA_PREPROCESAMIENTO, flag_d ? "1" : "0", NULL };
        execv(RUTA_PREPROCESAMIENTO, args);
        perror("lab2: execv preprocesamiento");
        _exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo tHough ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        int fuentes[2] = { p_prepro_hough[0], p_hough_resultados[1] };
        int destinos[2] = { 0, 1 };

        cerrar_no_necesarios(todos, n_todos, fuentes, 2);
        configurar_fds_hijo(fuentes, destinos, 2);

        char* args[] = { RUTA_THOUGH, r_str, NULL };
        execv(RUTA_THOUGH, args);
        perror("lab2: execv tHough");
        _exit(1);
    }
    pids[n_pids++] = pid;

    //---- nodo resultados ----
    pid = fork();
    if (pid == -1) {
        perror("lab2: fork");
        return 1;
    }
    if (pid == 0) {
        int fuentes[1] = { p_hough_resultados[0] };
        int destinos[1] = { 0 };

        cerrar_no_necesarios(todos, n_todos, fuentes, 1);
        configurar_fds_hijo(fuentes, destinos, 1);

        char* args[] = { RUTA_RESULTADOS, t_str, v_str, ruta_o, NULL };
        execv(RUTA_RESULTADOS, args);
        perror("lab2: execv resultados");
        _exit(1);
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
            int fuentes[2] = { p_prepro_ruido[0], p_carga_ruido[0] };
            int destinos[2] = { 0, 3 };

            cerrar_no_necesarios(todos, n_todos, fuentes, 2);
            configurar_fds_hijo(fuentes, destinos, 2);

            char* args[] = { RUTA_ADERUIDO, NULL };
            execv(RUTA_ADERUIDO, args);
            perror("lab2: execv aDeRuido");
            _exit(1);
        }
        pids[n_pids++] = pid;
    }

    //el padre no participa en la transferencia de datos: debe cerrar todos
    //los fd de pipes que le quedan abiertos, o algun lector nunca vera EOF
    for (int i = 0; i < n_todos; i++) {
        close(todos[i]);
    }

    //esperar a que todos los nodos terminen y revisar sus codigos de salida
    int hubo_error = 0;
    for (int i = 0; i < n_pids; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            hubo_error = 1;
        }
    }

    if (hubo_error) {
        fprintf(stderr, "lab2: uno o mas nodos del pipeline fallaron\n");
        return 1;
    }

    return 0;
}
