# Documentación de ejemplo
Aqui podeis poner y editar cualquier archivo markdown referente a el proyecto
* Explicaciones sobre el hardware
* Aclaraciones sobre el código y su funcionamiento
* Tutoriales de calibración, diagnosis, fallos comunes...
* Fotos, diagramas, comentarios... Cualquier cosa que ayude al que viene detrás a entender,modificar y mejorar el proyecto

En este fichero debe figurar un resumen de que hace la placa

Meter los distintos archivos markdown en este directorio, para su creación recomiendo [StackEdit](https://stackedit.io)

Pueden ser vistos con cualquier utilidad que soporte markdown o bien utilizando 
la herramienta docsify incluida en el repositorio. Para añadirlos a la barra de navegación poner en el archivo _sidebar.md:
```markdown
...
* [Titulo](miarchivo.md)
```


* Para instalar la herramienta
```bash
$ sudo apt-get install npm
$ npm i docsify-cli -g
```
* Para ver la documentación (ejecutar en el directorio)
```bash
$ docsify serve
```
