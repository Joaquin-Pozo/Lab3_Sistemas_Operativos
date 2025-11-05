#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------- Estructuras utilizadas ----------------- */

// Estructura para almacenar datos en cada hebra
typedef struct {
    char *registros;
    int inicio;
    int fin;
    // arreglo que contiene los productos en oferta
    char *productosOferta;
} ThreadData;

typedef struct {
    char producto;
    int cantidad;
} ResultadoVentas;

// Función que lee un archivo y almacena la información en un arreglo de strings
char **leerArchivo(const char *nombreArchivo, int *cantidadProductos) {
    FILE *archivo = fopen(nombreArchivo, "r");
    
    // Comprueba que el archivo exista en el mismo path
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s\n", nombreArchivo);
        *cantidadProductos = 0;
        return NULL;
    }

    char linea[50];
    int capacidad = 1000;
    *cantidadProductos = 0;

    char **arregloProductos = malloc(capacidad * sizeof(char *));

    // Lee el archivo hasta que no queden lineas por leer
    while (fgets(linea, sizeof(linea), archivo)) {
        // Si el arreglo se llena, aumenta su capacidad en un 50%
        if (*cantidadProductos >= capacidad) {
            capacidad += capacidad/2;
            arregloProductos = realloc(arregloProductos, capacidad * sizeof(char *));
        }

        // Elimina el salto de línea
        linea[strcspn(linea, "\n")] = '\0';

        // Lee el producto y lo almacena en el arreglo
        arregloProductos[*cantidadProductos] = strdup(linea);

        (*cantidadProductos)++;

    }

    fclose(archivo);
    return arregloProductos;
}


int main (int argc, char *argv[]) {

    int cantidadProductos = 0;
    char **arregloProductos = leerArchivo(argv[1], &cantidadProductos);

    if (arregloProductos == NULL || cantidadProductos == 0) {
        printf("El archivo se encuentra vacío o no existe.\n");
        return 0;
    }

    for (int i = 0; i < cantidadProductos; i++) {
        printf("Linea %d: Producto: %s\n", i + 1, arregloProductos[i]);
        free(arregloProductos[i]);
    }

    free(arregloProductos);

    return 0;

}