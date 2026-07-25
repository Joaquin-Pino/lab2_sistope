# Laboratorio 2 — Pipeline de Detección de Círculos (Hough + Morfología)

Detección de círculos sobre imágenes binarias mediante un pipeline de procesos independientes
(`fork()` + `pipe()` + `execv()` + `dup2()`), en vez del programa de un solo proceso del Lab 1.

## Arquitectura

```
cargaDatos --original--> preprocesamiento --preprocesada--> tHough --acumulador--> resultados -> reporte.csv
     |                          |
     +--original (si -d)       +--preprocesada (si -d)
                    \                        /
                     v                      v
                  aDeRuido -> preprocesada.bin, ruido.bin
```

- **cargaDatos**: lee el `.bin` de entrada desde disco.
- **preprocesamiento**: apertura morfológica (erosión + dilatación) para limpiar ruido.
- **tHough**: genera el plano acumulador de votos de Hough para un radio `r` fijo (720 pasos
  angulares, incrementos de `2π/720`).
- **resultados**: supresión de no máximos (ventana `-v`) + filtro por umbral `-t` + exporta el CSV.
- **aDeRuido** (opcional, solo con `-d`): rama paralela que recibe la imagen original y la
  preprocesada, calcula el ruido (`original − preprocesada`) y exporta `preprocesada.bin` y
  `ruido.bin`.

Cada nodo es un ejecutable independiente que se comunica con el resto exclusivamente por pipes
(stdin/stdout, y el descriptor 3 para la rama de análisis de ruido).

## Compilación

```
$ make all
```

Genera los ejecutables `lab2`, `cargaDatos`, `preprocesamiento`, `tHough`, `resultados` y
`aDeRuido` en el directorio del proyecto. `lab2` invoca a los demás nodos con rutas relativas
(`./cargaDatos`, etc.), por lo que **debe ejecutarse desde ese mismo directorio**.

Para limpiar los binarios y archivos objeto:

```
$ make clean
```

## Dependencias de Python

Para ejecutar los scripts de validación y visualización de resultados (`VerificacionVisual.py`,
`Visualizer.py`, `GenerarDatos.py`), se debe tener instaladas las siguientes librerías:

* numpy
* Pillow
* opencv-python
* pandas
* matplotlib

## Explicación de la Ejecución

```
$ ./lab2 -i <imagen_entrada.bin> -r <radio> -t <umbral> [-v <vecindad>] [-o <reporte_salida.csv>] [-d]
```

### Detalle de las Flags

| Flag | Significado | Obligatoria | Default |
|------|-------------|-------------|---------|
| `-i` | Ruta del archivo de imagen binaria de entrada (.bin) | Sí | — |
| `-r` | Radio exacto de los círculos que se desean detectar (entero positivo) | Sí | — |
| `-t` | Umbral de confianza (votos mínimos en el plano de Hough) (entero positivo) | Sí | — |
| `-v` | Tamaño de vecindad para la supresión de no máximos (entero positivo **impar**) | No | `7` |
| `-o` | Nombre del archivo CSV de salida | No | `reporte.csv` |
| `-d` | Debug dump: exporta además `preprocesada.bin` y `ruido.bin` | No | (desactivado) |

Los valores de `-r`, `-t` y `-v` se validan estrictamente: valores no enteros (p.ej. `4.5`),
negativos, o un `-v` par, hacen que el programa informe el error por stderr y termine sin ejecutar
el pipeline.

## Ejemplos de Ejecución (Distintos Radios y Umbrales)

```
$ ./lab2 -i ejemplo1.bin -r 35 -t 100 -d -o reporte.csv

$ ./lab2 -i ejemplo1.bin -r 60 -t 103 -o reporte.csv

$ ./lab2 -i ejemplo2.bin -r 40 -t 103 -d -o reporte.csv

$ ./lab2 -i ejemplo2.bin -r 60 -t 100 -o reporte.csv

$ ./lab2 -i ejemplo3.bin -r 100 -t 100 -v 9 -o reporte.csv
```

## Verificación Visual

Para ver la salida gráfica de cada ejecución (superponiendo los centros detectados sobre la
imagen preprocesada), corriendo con `-d`, se debe ejecutar el siguiente comando de Python una vez
que `./lab2` haya finalizado exitosamente:

```
$ python3 python/VerificacionVisual.py -i preprocesada.bin -c reporte.csv
```
