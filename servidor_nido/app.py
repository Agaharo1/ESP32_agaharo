from flask import Flask, request, send_from_directory
import os
import time

app = Flask(__name__)
SAVE_DIR = "/app/fotos"

if not os.path.exists(SAVE_DIR):
    os.makedirs(SAVE_DIR)

# 1. Ruta para recibir las fotos de la ESP32
@app.route('/upload', methods=['POST'])
def upload_file():
    if request.data:
        filename = f"foto_nido_{int(time.time())}.jpg"
        filepath = os.path.join(SAVE_DIR, filename)
        with open(filepath, 'wb') as f:
            f.write(request.data)
        print(f"Foto guardada: {filename}")
        return "OK", 200
    return "Sin datos", 400

# 2. Ruta auxiliar para cargar la imagen en el navegador
@app.route('/foto/<filename>')
def mostrar_foto(filename):
    return send_from_directory(SAVE_DIR, filename)

# 3. Ruta principal: ¡LA GALERÍA!
@app.route('/')
def galeria():
    archivos = os.listdir(SAVE_DIR)
    # Filtramos solo los jpg y los ordenamos por los más nuevos primero
    fotos = [f for f in archivos if f.endswith('.jpg')]
    fotos.sort(reverse=True) 
    
    # Creamos una página web sencilla (HTML)
    html = "<h1 style='text-align: center; font-family: sans-serif;'>🐣 Monitor del Nido</h1>"
    html += "<div style='display: flex; flex-wrap: wrap; justify-content: center; font-family: sans-serif;'>"
    for f in fotos:
        html += f"<div style='margin: 10px; border: 1px solid #ccc; padding: 10px; border-radius: 8px;'>"
        html += f"  <img src='/foto/{f}' style='width: 400px; display: block;'>"
        html += f"  <p style='text-align: center; color: #555; margin-top: 8px;'>{f}</p>"
        html += f"</div>"
    html += "</div>"
    return html

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)