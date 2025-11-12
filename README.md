# Laboratorio N°3 - Procesamiento Paralelo y Consulta de Inventario (Cyber Day)
### Universidad de Santiago de Chile
### Estudiante: Joaquín Pozo
### Rut: 20.237.059-4
#### Ejecutado y compilado en Ubuntu 22.04.5 LTS

---

Este programa procesa un archivo de ventas del Cyber Day 2025 utilizando **procesamiento paralelo con múltiples hebras**.

Cada hebra analiza una parte equitativa del archivo, identifica los productos vendidos y actualiza de forma segura el conteo total de unidades vendidas para cada producto en oferta.

La sincronización entre hebras se realiza mediante **mutex** y **barreras**, garantizando un conteo correcto y evitando condiciones de carrera.
Al finalizar, el programa genera un archivo de reporte con los resultados totales y permite realizar consultas interactivas desde consola.

---

### Compilación

Para compilar el programa, se debe ejecutar el siguiente comando en la misma carpeta donde se encuentren los archivos **main.c**, **ventas_cyberday_2025.txt** y **ofertas_cyberday_2025.txt**:

```bash
system@ubuntu:~$ gcc main.c -o main -pthread
```

---

### Ejecución

Para ejecutar el programa, ingrese el nombre del archivo de ventas y la cantidad de hebras a utilizar. El programa generará un archivo **reporte_ventas_final.txt** con el resumen de los productos en oferta y las unidades vendidas, mostrando un mensaje de confirmación:

```bash
system@ubuntu:~$ ./main ventas_cyberday_2025.txt 5
Reporte generado éxitosamente.
```

---

### Salida esperada

El archivo **reporte_ventas_final.txt** contendrá lo siguiente:

```yml
Resultado Ventas Cyber Day - 2025:

Smart TV 55 Pulgadas: 185 unidades
Playstation 5: 198 unidades
Alexa Echo Dot 5ta generacion: 132 unidades
Iphone 17 Pro Max: 484 unidades
```

---

### Consultas por consola

Una vez generado el reporte, el usuario puede consultar las unidades vendidas de cualquier producto en oferta:

```bash
system@ubuntu:~$ Ingrese el nombre del producto en oferta para ver sus unidades vendidas: Alexa Echo Dot 5ta generacion
Unidades vendidas: 132.
```

Para finalizar el programa, ingrese salida:

```bash
system@ubuntu:~$ Ingrese el nombre del producto en oferta para ver sus unidades vendidas: salida
Ejecución finalizada.
```

---

### Consideraciones adicionales

El archivo de **ofertas_cyberday_2025.txt** contiene en su primera línea la cantidad de productos en oferta, seguida del listado de productos.

Ejemplo:
```yml
4
Smart TV 55 Pulgadas
Playstation 5
Alexa Echo Dot 5ta generacion
Iphone 17 Pro Max
```

Si se modifica este archivo, los productos listados deben coincidir con los nombres presentes en **ventas_cyberday_2025.txt**.