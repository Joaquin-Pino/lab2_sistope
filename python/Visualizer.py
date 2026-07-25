import sys
import numpy as np
from PIL import Image

def bin_to_png(input_path, output_path):
    with open(input_path, "rb") as f:
        # Leer cabecera
        width = int.from_bytes(f.read(4), byteorder='little')
        height = int.from_bytes(f.read(4), byteorder='little')
        
        # Leer pixeles 
        data = np.fromfile(f, dtype=np.uint8)
        
    # Reestructurar y escalar 
    img_array = data.reshape((height, width)) * 255
    img = Image.fromarray(img_array, mode='L')
    img.save(output_path)
    print(f"Imagen guardada en: {output_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python visualizer.py archivo.bin salida.png")
    else:
        bin_to_png(sys.argv[1], sys.argv[2])