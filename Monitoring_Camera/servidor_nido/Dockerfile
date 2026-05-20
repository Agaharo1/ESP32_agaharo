# Usamos una versión ligera de Python
FROM python:3.10-slim

# Directorio de trabajo dentro del contenedor
WORKDIR /app

# Copiamos las dependencias e instalamos
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copiamos el script del servidor
COPY app.py .

# Exponemos el puerto que usa Flask
EXPOSE 5000

# Comando para arrancar el servidor
CMD ["python", "app.py"]