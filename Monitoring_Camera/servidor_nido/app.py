from flask import Flask, request
import os
from datetime import datetime

app = Flask(__name__)
# Esta es la ruta DENTRO del contenedor
SAVE_DIR = "/app/fotos"

if not os.path.exists(SAVE_DIR):
    os.makedirs(SAVE_DIR)

@app.route('/upload', methods=['POST'])
def upload_file():
    if request.data:
        # Obtener fecha y hora actual
        now = datetime.now()
        fecha_hora = now.strftime("%Y-%m-%d_%H-%M-%S")
        
        # Crear nombre del archivo con fecha y hora
        filename = f"foto_nido_{fecha_hora}.jpg"
        filepath = os.path.join(SAVE_DIR, filename)
        
        # Guardar la imagen
        with open(filepath, 'wb') as f:
            f.write(request.data)
            
        print(f"Foto guardada: {filename}")
        return "OK", 200
    return "Sin datos", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)