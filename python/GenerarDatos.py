import struct
import numpy as np
import random
import argparse

def generar_imagen_con_ruido(nombre_archivo, ancho, alto, grosor, num_circulos, probabilidad_ruido, radio):
    # 1. Crear matriz de fondo negro
    imagen = np.zeros((alto, ancho), dtype=np.uint8)
    Y, X = np.ogrid[:alto, :ancho]

    # 2. Dibujar los círculos base
    for _ in range(num_circulos):
        cx, cy = random.randint(100, ancho - 100), random.randint(100, alto - 100)
        r_ext = radio
        r_int = r_ext - grosor
        dist_sq = (X - cx)**2 + (Y - cy)**2
        imagen[(dist_sq <= r_ext**2) & (dist_sq > r_int**2)] = 1

    # 3. Dibujar círculos adicionales 
    for _ in range(num_circulos // 2):
        cx, cy = random.randint(100, ancho - 100), random.randint(100, alto - 100)
        r_ext_rand = random.randint(max(5, int(radio*3/2//1)), radio*2)
        r_int_rand = r_ext_rand - grosor
        dist_sq = (X - cx)**2 + (Y - cy)**2
        imagen[(dist_sq <= r_ext_rand**2) & (dist_sq > r_int_rand**2)] = 1

    # 4. Generar RUIDO DE SAL
    ruido_aleatorio = np.random.rand(alto, ancho)
    imagen[ruido_aleatorio < probabilidad_ruido] = 1

    # 5. Guardar en formato binario
    with open(nombre_archivo, "wb") as f:
        f.write(struct.pack('i', ancho))
        f.write(struct.pack('i', alto))
        f.write(imagen.tobytes())
    
    print(f"Archivo '{nombre_archivo}' generado: {ancho}x{alto}, Radio: {radio}, Ruido: {probabilidad_ruido}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generador de imágenes binarias con ruido para erosión.")

    # Definicion de flags
    parser.add_argument("-o", "--output", type=str, default="imagen_ruido.bin", help="Nombre del archivo de salida")
    parser.add_argument("-r", "--radio", type=int, default=40, help="Radio base de los círculos")
    parser.add_argument("-g", "--grosor", type=int, default=3, help="Grosor del borde de los círculos")
    parser.add_argument("-n", "--num", type=int, default=8, help="Número de círculos base")
    parser.add_argument("-p", "--prob", type=float, default=0.01, help="Probabilidad de ruido (0.0 a 1.0)")
    parser.add_argument("-w", "--width", type=int, default=1024, help="Ancho de la imagen")
    parser.add_argument("-hi", "--height", type=int, default=1024, help="Alto de la imagen")

    args = parser.parse_args()

    # Llamada a la funcion con los argumentos
    generar_imagen_con_ruido(
        nombre_archivo=args.output,
        ancho=args.width,
        alto=args.height,
        grosor=args.grosor,
        num_circulos=args.num,
        probabilidad_ruido=args.prob,
        radio=args.radio
    )