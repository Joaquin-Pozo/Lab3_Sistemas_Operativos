#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------- Structs utilizadas ----------------- */

// Struct que contiene los productos en oferta y la cantidad de ventas
typedef struct {
    char *producto;
    int cantidad;
} ResultadoVentas;

// Struct para almacenar datos en cada hebra
typedef struct {
    char **arregloProductos;
    int inicio;
    int fin;
    ResultadoVentas *productosOferta;
} ThreadData;

// Función que lee el archivo de ventas cyberday y almacena la información en un arreglo de strings
char **leerArchivoVentas(const char *nombreArchivo, int *cantidadProductos) {
    FILE *archivo = fopen(nombreArchivo, "r");
    
    // Comprueba que el archivo exista en el mismo path
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s\n", nombreArchivo);
        *cantidadProductos = 0;
        return NULL;
    }
    // Lee los primeros 50 caracteres de cada linea (mas que suficiente para resolver este laboratorio)
    char string[50];
    int lineas = 1000;
    *cantidadProductos = 0;

    char **arregloProductos = malloc(lineas * sizeof(char *));

    // Lee el archivo hasta el final
    while (fgets(string, sizeof(string), archivo)) {
        // Si el arreglo se llena, aumenta su capacidad en un 50%
        if (*cantidadProductos >= lineas) {
            lineas += lineas/2;
            arregloProductos = realloc(arregloProductos, lineas * sizeof(char *));
        }

        // Elimina el salto de linea, intercambiandolo por el caracter nulo
        string[strcspn(string, "\n")] = '\0';

        // Copia y pega el string en el arreglo
        arregloProductos[*cantidadProductos] = strdup(string);

        (*cantidadProductos)++;

    }

    fclose(archivo);
    return arregloProductos;
}

// Función que lee el archivo de ofertas cyberday y almacena la información en un arreglo de structs
ResultadoVentas *leerArchivoOfertas(const char *nombreArchivo, int *cantidadProductos) {
    FILE *archivo = fopen(nombreArchivo, "r");
    
    // Comprueba que el archivo exista en el mismo path
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s\n", nombreArchivo);
        *cantidadProductos = 0;
        return NULL;
    }
    // Lee los primeros 50 caracteres de cada linea (mas que suficiente para resolver este laboratorio)
    char string[50];
    // Almacena la cantidad de productos en oferta
    fscanf(archivo, "%d\n", cantidadProductos);

    ResultadoVentas *arregloProductos = malloc(*cantidadProductos * sizeof(ResultadoVentas));

    // Lee y almacena las ofertas en el struct ResultadoVentas
    for (int i = 0; i < *cantidadProductos; i++) {
        fgets(string, sizeof(string), archivo);

        // Elimina el salto de linea, intercambiandolo por el caracter nulo
        string[strcspn(string, "\n")] = '\0';

        arregloProductos[i].producto = strdup(string);
        arregloProductos[i].cantidad = 0;
    }

    fclose(archivo);
    return arregloProductos;
}


int main (int argc, char *argv[]) {

    int cantidadVentasCyberday = 0;
    char *nombreArchivoVentas = strdup(argv[1]);
    char **ventasCyberday = leerArchivoVentas(nombreArchivoVentas, &cantidadVentasCyberday);

    if (ventasCyberday == NULL || cantidadVentasCyberday == 0) {
        printf("El archivo '%s' se encuentra vacío o no existe.\n", nombreArchivoVentas);
        return 0;
    }

    int cantidadOfertasCyberday = 0;
    char *nombreArchivoOfertas = "ofertas_cyberday_2025.txt";
    ResultadoVentas *ofertasCyberday = leerArchivoOfertas(nombreArchivoOfertas, &cantidadOfertasCyberday);

    if (ofertasCyberday == NULL || cantidadOfertasCyberday == 0) {
        printf("El archivo '%s' se encuentra vacío o no existe.\n", nombreArchivoOfertas);
        return 0;
    }

    for (int i = 0; i < cantidadOfertasCyberday; i++) {
        printf("Oferta %i: Producto: '%s' Cantidad: %i.\n", i + 1, ofertasCyberday[i].producto, ofertasCyberday[i].cantidad);
    }

    
    // Libera memoria asignada con malloc
    for (int i = 0; i < cantidadVentasCyberday; i++) {
        free(ventasCyberday[i]);
    }

    free(ventasCyberday);
    free(ofertasCyberday);

    return 0;

}