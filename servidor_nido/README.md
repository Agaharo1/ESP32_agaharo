# 🐣 Proyecto Monitor de Nido

Este proyecto permite recibir, almacenar y visualizar fotografías capturadas por una **ESP32-CAM** en un servidor local **Ubuntu** utilizando contenedores **Docker**.

## 🚀 Comandos de Control (Docker)

Para poner en marcha el servidor, aplicar cambios en el código de Python o actualizar la configuración, sitúate en la carpeta `servidor_nido` y ejecuta:

```bash
docker-compose up -d --build
docker-compose stop
```

##  Visualizar las Fotos Guardadas
Entra en la carpeta de las fotos:

```bash
cd ~/Documents/Github/Esp32_agaharo/servidor_nido/fotos_guardadas
```
Inicia el servidor HTTP en el puerto 8080:

```bash
python3 -m http.server 8080
```
Accede desde tu ordenador en:
```bash
👉 http://192.168.1.34:8080
```