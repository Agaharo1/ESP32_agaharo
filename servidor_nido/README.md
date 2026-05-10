# 🐣 Proyecto Monitor de Nido

Este proyecto permite recibir, almacenar y visualizar fotografías capturadas por una **ESP32-CAM** en un servidor local **Ubuntu** utilizando contenedores **Docker**.

## 🚀 Comandos de Control (Docker)

Para poner en marcha el servidor, aplicar cambios en el código de Python o actualizar la configuración, sitúate en la carpeta `servidor_nido` y ejecuta:

```bash
sudo docker-compose up -d --build



Entra en la carpeta de las fotos:

Bash
cd ~/Documents/Github/Esp32_agaharo/servidor_nido/fotos_guardadas
Inicia el servidor en el puerto 8080:

Bash
python3 -m http.server 8080
Accede desde tu ordenador en:
👉 http://192.168.1.34:8080