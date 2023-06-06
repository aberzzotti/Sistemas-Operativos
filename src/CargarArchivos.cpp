#ifndef CHM_CPP
#define CHM_CPP

#include "CargarArchivos.hpp"

#include <pthread.h>

#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath)
{
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

struct cargarArchivosThreadArgs {
    HashMapConcurrente &hash_map;
    std::vector<std::string> *file_paths;
    std::atomic<unsigned int> *proximo_index;
};

void *cargarArchivosThread(void *argv)
{
    struct cargarArchivosThreadArgs *args = (struct cargarArchivosThreadArgs *)argv;

    unsigned int i;
    while ((i = args->proximo_index->fetch_add(1)) < args->file_paths->size()) {
        cargarArchivo(args->hash_map, args->file_paths->at(i));
    }

    return nullptr;
}

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, std::vector<std::string> filePaths)
{
    std::atomic<unsigned int> proximo_index(0);
    struct cargarArchivosThreadArgs args = {hashMap, &filePaths, &proximo_index};
    std::vector<pthread_t> threads(cantThreads);

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_create(&threads[i], NULL, cargarArchivosThread, &args);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}
#endif
