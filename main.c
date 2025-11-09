#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* ----------------- Structs utilizadas ----------------- */

// Struct compartido que contiene un producto en oferta y su cantidad de ventas
typedef struct {
    char *producto;
    // contador compartido
    int cantidadVentas;
} ResultadoVentas;

// Struct para almacenar datos en cada hebra
typedef struct {
    char **productosVenta;
    int inicio;
    int fin;
    int cantidadProductosOferta;
    ResultadoVentas *productosOferta;
} ThreadData;

// Mutex que protege el acceso al struct compartido ResultadoVentas
static pthread_mutex_t mutexVentas;
// Barrera que bloquea el paso de los threads trabajadores
pthread_barrier_t barreraVentas;

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
    int lineas = 100;
    *cantidadProductos = 0;

    char **arregloProductos = malloc(lineas * sizeof(char *));

    // Lee el archivo hasta el final
    while (fgets(string, sizeof(string), archivo)) {

        // Elimina el salto de linea y el caracter \r (en caso de que se haya pasado), intercambiandolo por el caracter nulo
        string[strcspn(string, "\r\n")] = '\0';
        
        // Si el arreglo se llena, aumenta su capacidad en un 50%
        if (*cantidadProductos >= lineas) {
            lineas += lineas/2;
            arregloProductos = realloc(arregloProductos, lineas * sizeof(char *));
        }

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
        arregloProductos[i].cantidadVentas = 0;
    }

    fclose(archivo);
    return arregloProductos;
}

// Hebra que procesa los productos asignados [inicio   --->  fin - 1]
void *procesarThread(void *arg) {
    ThreadData *threadData = (ThreadData *)arg;

    for (int i = threadData->inicio; i < threadData->fin; i++) {
        for (int j = 0; j < threadData->cantidadProductosOferta; j++) {
            if (strcmp(threadData->productosVenta[i], threadData->productosOferta[j].producto) == 0) {
                // Bloquea el mutex antes de acceder al contador compartido
                pthread_mutex_lock(&mutexVentas);
                // Seccion Critica (SC): incrementa el contador compartido
                threadData->productosOferta[j].cantidadVentas++;
                // Desbloquea el mutex
                pthread_mutex_unlock(&mutexVentas);
            }
        }
    }
    // Espera a que las demas hebras terminen su conteo para finalizar
    pthread_barrier_wait(&barreraVentas);

    pthread_exit(NULL);
}

// Función orquestadora que crea y procesa los threads para leer los productos en oferta y actualizar sus ventas
// Utiliza barreras para asegurar la correcta finalización de las hebras.
// Genera un archivo de salida que contiene los productos en oferta y el conteo de sus ventas
void conteoConThreads(char **productosVenta, int cantidadProductosVenta, int cantidadThreads, ResultadoVentas *productosOferta, int cantidadProductosOferta) {
    // Inicializa el mutex
    pthread_mutex_init(&mutexVentas, NULL);
    // Inicializa la barrera
    pthread_barrier_init(&barreraVentas, NULL, cantidadThreads);

    // Crea los threads
    pthread_t *threads = malloc(cantidadThreads * sizeof(pthread_t));
    ThreadData *threadData = malloc(cantidadThreads * sizeof(ThreadData));
    // base = cantidad minimima de productos que recibe cada thread
    int base = cantidadProductosVenta / cantidadThreads;
    // resto = cantidad de productos extras que se deben procesar
    int resto = cantidadProductosVenta % cantidadThreads;
    int inicio = 0;

    // Realiza una distribución equitativa de productos entre threads
    for (int i = 0; i < cantidadThreads; i++) {
        // Si el iterador es menor que el resto, le agrega 1 linea mas al thread, caso contrario, no suma nada
        int cantidadProductosRestantes = base + (i < resto ? 1 : 0);
        threadData[i].productosVenta = productosVenta;
        threadData[i].inicio = inicio;
        threadData[i].fin = inicio + cantidadProductosRestantes;
        threadData[i].cantidadProductosOferta = cantidadProductosOferta;
        threadData[i].productosOferta = productosOferta;

        // crear hebra
        pthread_create(&threads[i], NULL, procesarThread, &threadData[i]);

        inicio += cantidadProductosRestantes;
    }

    // Espera a que finalicen todas las hebras
    for (int i = 0; i < cantidadThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Escribe el archivo de reporte
    FILE *archivoSalida = fopen("reporte_ventas_final.txt", "w");
    if (archivoSalida == NULL) {
        printf("Error creando archivo de salida\n");
    } else {
        fprintf(archivoSalida, "Resultado Ventas Cyber Day - 2025:\n\n");
        for (int i = 0; i < cantidadProductosOferta; i++) {
            fprintf(archivoSalida, "%s: %d unidades\n", productosOferta[i].producto, productosOferta[i].cantidadVentas);
        }
        printf("Reporte generado éxitosamente.\n");
        fclose(archivoSalida);
    }

    // Destruye el mutex despues de su uso
    pthread_mutex_destroy(&mutexVentas);
    // Destruye la barrra despues de su uso
    pthread_barrier_destroy(&barreraVentas);

    // Libera memoria asignada con malloc
    free(threads);
    free(threadData);
}


int main (int argc, char *argv[]) {

    if (argc < 3) {
        printf("Para ejecutar el programa, ingrese: %s <ventas_cyberday_2025.txt> <cantidad_hebras>\n", argv[0]);
        return 1;
    }

    int cantidadVentasCyberday = 0;
    char *nombreArchivoVentas = strdup(argv[1]);
    int cantidadThreads = atoi(argv[2]);
    char **ventasCyberday = leerArchivoVentas(nombreArchivoVentas, &cantidadVentasCyberday);

    if (ventasCyberday == NULL || cantidadVentasCyberday == 0) {
        printf("El archivo '%s' se encuentra vacío o no existe.\n", nombreArchivoVentas);
        return 1;
    }

    int cantidadOfertasCyberday = 0;
    char *nombreArchivoOfertas = "ofertas_cyberday_2025.txt";
    ResultadoVentas *ofertasCyberday = leerArchivoOfertas(nombreArchivoOfertas, &cantidadOfertasCyberday);

    if (ofertasCyberday == NULL || cantidadOfertasCyberday == 0) {
        printf("El archivo '%s' se encuentra vacío o no existe.\n", nombreArchivoOfertas);
        return 1;
    }

    // Realiza el procesamiento de ventas utilizando hebras
    conteoConThreads(ventasCyberday, cantidadVentasCyberday, cantidadThreads, ofertasCyberday, cantidadOfertasCyberday);

    char input[50];
    // Flag para salir
    int salida = 1;
    // Loop para mostrar las unidades vendidas de los productos en oferta
    while (salida) {
        printf("Ingrese el nombre del producto en oferta para ver sus unidades vendidas: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "salida") == 0) {
            salida = 0;
            break;
        }
        for (int i = 0; i < cantidadOfertasCyberday; i++) {
            if (strcmp(input, ofertasCyberday[i].producto) == 0) {
                printf("Unidades vendidas: %i.\n", ofertasCyberday[i].cantidadVentas);
                salida = -1;
                break;
            }
        }
        if (salida != -1) {
            printf("El nombre ingresado no corresponde a ningún producto en oferta.\n");
        } else {
            salida = 1;
        }
    }
    
    // Libera memoria asignada con malloc
    for (int i = 0; i < cantidadVentasCyberday; i++) {
        free(ventasCyberday[i]);
    }

    free(ventasCyberday);
    free(ofertasCyberday);

    return 0;

}