import cv2
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
import sys

def main():
    # Configuracion del parser de argumentos
    parser = argparse.ArgumentParser(description="Visualizador de deteccion de circulos para Laboratorio de SO")
    
    parser.add_argument("-i", "--image", required=True, help="Ruta de la imagen binaria (.bin)")
    parser.add_argument("-c", "--csv", required=True, help="Ruta del archivo de reporte (.csv)")
    parser.add_argument("-r", "--radio", type=int, default=50, help="Radio de los circulos (default: 50)")
    parser.add_argument("-s", "--show_radio", action="store_true", help="Si se incluye, dibuja el contorno del circulo")
    parser.add_argument("-o", "--output", help="Nombre del archivo para guardar la imagen resultante")

    args = parser.parse_args()

    # 1. Lectura del archivo binario
    try:
        with open(args.image, "rb") as f:
            width = int.from_bytes(f.read(4), byteorder='little')
            height = int.from_bytes(f.read(4), byteorder='little')
            data = np.fromfile(f, dtype=np.uint8)
            
        img = data.reshape((height, width)) * 255
        img_color = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    except Exception as e:
        print(f"Error al procesar la imagen binaria: {e}")
        sys.exit(1)

    # 2. Lectura de centros desde CSV
    try:
        df = pd.read_csv(args.csv)
    except Exception as e:
        print(f"Error al leer el archivo CSV: {e}")
        sys.exit(1)

    # 3. Dibujado de resultados
    for _, row in df.iterrows():
        # Asegurar coordenadas enteras
        center = (int(row['X']), int(row['Y']))
        
        # Dibujar centro
        cv2.drawMarker(img_color, center, (0, 0, 255), 
                       markerType=cv2.MARKER_CROSS, 
                       markerSize=12, thickness=2)
        
        # Dibujar radio si se especifica
        if args.show_radio:
            cv2.circle(img_color, center, args.radio, (0, 255, 0), 2)

    # 4. Mostrar o Guardar
    if args.output:
        cv2.imwrite(args.output, img_color)
        print(f"Resultado guardado en: {args.output}")
    
    plt.figure(figsize=(10, 7))
    plt.imshow(cv2.cvtColor(img_color, cv2.COLOR_BGR2RGB))
    plt.title(f"Deteccion en {args.image}")
    plt.axis('off')
    plt.show()

if __name__ == "__main__":
    main()